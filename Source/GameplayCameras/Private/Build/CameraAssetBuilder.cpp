// Copyright Epic Games, Inc. All Rights Reserved.

#include "Build/CameraAssetBuilder.h"

#include "Build/CameraObjectInterfaceParameterBuilder.h"
#include "Build/CameraRigAssetBuilder.h"
#include "Core/CameraAsset.h"
#include "Core/CameraDirector.h"
#include "Core/CameraObjectInterfaceParameterDefinition.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "GameplayCamerasDelegates.h"
#include "Logging/TokenizedMessage.h"

#define LOCTEXT_NAMESPACE "CameraAssetBuilder"

namespace UE::Cameras
{

FCameraAssetBuilder::FCameraAssetBuilder(FCameraBuildLog& InBuildLog)
	: BuildLog(InBuildLog)
{
}

void FCameraAssetBuilder::BuildCamera(UCameraAsset* InCameraAsset, bool bBuildReferencedAssets)
{
	if (!ensure(InCameraAsset))
	{
		return;
	}

	CameraAsset = InCameraAsset;
	BuildLog.SetLoggingPrefix(InCameraAsset->GetPathName() + TEXT(": "));
	{
		BuildCameraImpl(bBuildReferencedAssets);
	}
	BuildLog.SetLoggingPrefix(FString());
	UpdateBuildStatus();

	FGameplayCamerasDelegates::OnCameraAssetBuilt().Broadcast(CameraAsset);
}

void FCameraAssetBuilder::BuildCameraImpl(bool bBuildReferencedAssets)
{
	TSet<UCameraRigAsset*> AllCameraRigs;
	TSet<UCameraRigProxyAsset*> AllCameraRigProxies;

	// Build the camera director and get the list of camera rigs it references.
	UCameraDirector* CameraDirector = CameraAsset->GetCameraDirector();
	if (CameraDirector)
	{
		CameraDirector->BuildCameraDirector(BuildLog);

		FCameraDirectorRigUsageInfo UsageInfo;
		CameraDirector->GatherRigUsageInfo(UsageInfo);

		AllCameraRigs.Append(UsageInfo.CameraRigs);
		AllCameraRigProxies.Append(UsageInfo.CameraRigProxies);
	}
	else
	{
		BuildLog.AddMessage(EMessageSeverity::Error, LOCTEXT("MissingDirector", "Camera has no director set."));
	}

	if (AllCameraRigs.IsEmpty() && AllCameraRigProxies.IsEmpty())
	{
		BuildLog.AddMessage(EMessageSeverity::Warning, LOCTEXT("MissingRigs", "Camera director isn't using any camera rigs or proxies."));
	}

	// Get also the rigs from the proxy table.
	if (CameraDirector)
	{
		for (const FCameraRigProxyRedirectTableEntry& Entry : CameraDirector->CameraRigProxyRedirectTable.Entries)
		{
			if (Entry.CameraRig)
			{
				AllCameraRigs.Add(Entry.CameraRig);
			}
		}
	}

	if (bBuildReferencedAssets)
	{
		// Build each of the camera rigs.
		for (UCameraRigAsset* CameraRig : AllCameraRigs)
		{
			FCameraRigAssetBuilder CameraRigBuilder(BuildLog);
			CameraRigBuilder.BuildCameraRig(CameraRig);
		}
	}

	// Get the list of all the camera rigs' interface parameters.
	TMap<FName, TArray<const UCameraRigAsset*>> UsedParameterNames;
	TMap<const UCameraRigAsset*, TArray<FCameraObjectInterfaceParameterDefinition>> DefinitionsByCameraRig;
	for (const UCameraRigAsset* CameraRig : AllCameraRigs)
	{
		if (ensure(!DefinitionsByCameraRig.Contains(CameraRig)))
		{
			TArray<FCameraObjectInterfaceParameterDefinition>& DefinitionsForCameraRig = DefinitionsByCameraRig.Add(CameraRig);
			DefinitionsForCameraRig.Append(CameraRig->GetParameterDefinitions());

			for (const FCameraObjectInterfaceParameterDefinition& Definition : DefinitionsForCameraRig)
			{
				UsedParameterNames.FindOrAdd(Definition.ParameterName).Add(CameraRig);
			}
		}
	}
	// Resolve name conflicts.
	// We can safely change the parameter definitions and property bag property names because the look-ups
	// for setting the default values afterwards are using Guids.
	for (const TPair<FName, TArray<const UCameraRigAsset*>>& Pair : UsedParameterNames)
	{
		const TArray<const UCameraRigAsset*>& ConflictCameraRigs(Pair.Value);
		if (ConflictCameraRigs.Num() > 1)
		{
			const FName ConflictName(Pair.Key);
			for (const UCameraRigAsset* CameraRig : ConflictCameraRigs)
			{
				TArray<FCameraObjectInterfaceParameterDefinition>& Definitions = DefinitionsByCameraRig.FindChecked(CameraRig);
				FCameraObjectInterfaceParameterDefinition* ConflictDefinition = Definitions.FindByPredicate(
						[ConflictName](FCameraObjectInterfaceParameterDefinition& Item)
						{
							return Item.ParameterName == ConflictName;
						});
				if (ensure(ConflictDefinition))
				{
					const FString NewName = FString::Format(TEXT("{0}_{1}"), { *GetNameSafe(CameraRig), *ConflictName.ToString() });
					ConflictDefinition->ParameterName = FName(NewName);
				}
			}
		}
	}

	// Build the final list of parameter definitions.
	TArray<FCameraObjectInterfaceParameterDefinition> ParameterDefinitions;
	for (const TPair<const UCameraRigAsset*, TArray<FCameraObjectInterfaceParameterDefinition>>& Pair : DefinitionsByCameraRig)
	{
		const TArray<FCameraObjectInterfaceParameterDefinition>& DefinitionsForCameraRig(Pair.Value);
		for (const FCameraObjectInterfaceParameterDefinition& Definition : DefinitionsForCameraRig)
		{
			ParameterDefinitions.Add(Definition);
		}
	}

	if (ParameterDefinitions != CameraAsset->ParameterDefinitions)
	{
		CameraAsset->Modify();
		CameraAsset->ParameterDefinitions = ParameterDefinitions;
	}

	// Rebuild the default parameters property bag.
	TArray<FPropertyBagPropertyDesc> DefaultParameterProperties;
	FCameraObjectInterfaceParameterBuilder::AppendDefaultParameterProperties(
			CameraAsset->ParameterDefinitions, DefaultParameterProperties);

	FInstancedPropertyBag DefaultParameters;
	DefaultParameters.AddProperties(DefaultParameterProperties);

	for (const UCameraRigAsset* CameraRig : AllCameraRigs)
	{
		FCameraObjectInterfaceParameterBuilder::SetDefaultParameterValues(CameraRig, DefaultParameters);
	}

	if (!DefaultParameters.Identical(&CameraAsset->DefaultParameters, 0))
	{
		CameraAsset->Modify();
		CameraAsset->DefaultParameters = DefaultParameters;
	}

	// Accumulate all the camera rigs' allocation infos and store that on the asset.
	FCameraAssetAllocationInfo AllocationInfo;

	for (const UCameraRigAsset* CameraRig : AllCameraRigs)
	{
		AllocationInfo.VariableTableInfo.Combine(CameraRig->AllocationInfo.VariableTableInfo);
		AllocationInfo.ContextDataTableInfo.Combine(CameraRig->AllocationInfo.ContextDataTableInfo);
	}

	if (AllocationInfo != CameraAsset->AllocationInfo)
	{
		CameraAsset->Modify();
		CameraAsset->AllocationInfo = AllocationInfo;
	}
}

void FCameraAssetBuilder::UpdateBuildStatus()
{
	ECameraBuildStatus BuildStatus = ECameraBuildStatus::Clean;
	if (BuildLog.HasErrors())
	{
		BuildStatus = ECameraBuildStatus::WithErrors;
	}
	else if (BuildLog.HasWarnings())
	{
		BuildStatus = ECameraBuildStatus::CleanWithWarnings;
	}

	// Don't modify the camera rig: BuildStatus is transient.
	CameraAsset->SetBuildStatus(BuildStatus);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

