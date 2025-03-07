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

void FCameraAssetBuilder::BuildCamera(UCameraAsset* InCameraAsset)
{
	BuildCamera(InCameraAsset, FCustomBuildStep::CreateLambda([](UCameraAsset*, FCameraBuildLog&) {}));
}
	
void FCameraAssetBuilder::BuildCamera(UCameraAsset* InCameraAsset, FCustomBuildStep InCustomBuildStep)
{
	if (!ensure(InCameraAsset))
	{
		return;
	}

	CameraAsset = InCameraAsset;
	BuildLog.SetLoggingPrefix(InCameraAsset->GetPathName() + TEXT(": "));
	{
		BuildCameraImpl();

		InCustomBuildStep.ExecuteIfBound(CameraAsset, BuildLog);
	}
	BuildLog.SetLoggingPrefix(FString());
	UpdateBuildStatus();

	FGameplayCamerasDelegates::OnCameraAssetBuilt().Broadcast(CameraAsset);
}

void FCameraAssetBuilder::BuildCameraImpl()
{
	TArray<UCameraRigAsset*> CameraRigs;

	// Build the camera director and get the list of camera rigs it references.
	if (UCameraDirector* CameraDirector = CameraAsset->GetCameraDirector())
	{
		CameraDirector->BuildCameraDirector(BuildLog);

		FCameraDirectorRigUsageInfo UsageInfo;
		CameraDirector->GatherRigUsageInfo(UsageInfo);
		CameraRigs = UsageInfo.CameraRigs;
	}
	else
	{
		BuildLog.AddMessage(EMessageSeverity::Error, LOCTEXT("MissingDirector", "Camera has no director set."));
	}

	if (CameraRigs.IsEmpty())
	{
		BuildLog.AddMessage(EMessageSeverity::Warning, LOCTEXT("MissingRigs", "Camera isn't using any camera rigs."));
	}

	// Build each of the camera rigs.
	for (UCameraRigAsset* CameraRig : CameraRigs)
	{
		FCameraRigAssetBuilder CameraRigBuilder(BuildLog);
		CameraRigBuilder.BuildCameraRig(CameraRig);
	}

	// Get the list of all the camera rigs' interface parameters, and cache some information
	// about them.
	TArray<FCameraObjectInterfaceParameterDefinition> ParameterDefinitions;
	TArray<TObjectPtr<const UCameraRigAsset>> ParameterOwners;

	for (const UCameraRigAsset* CameraRig : CameraRigs)
	{
		for (const FCameraObjectInterfaceParameterDefinition& Definition : CameraRig->GetParameterDefinitions())
		{
			ParameterDefinitions.Add(Definition);
			ParameterOwners.Add(CameraRig);
		}
	}

	if (ParameterDefinitions != CameraAsset->ParameterDefinitions || ParameterOwners != CameraAsset->ParameterOwners)
	{
		CameraAsset->Modify();
		CameraAsset->ParameterDefinitions = ParameterDefinitions;
		CameraAsset->ParameterOwners = ParameterOwners;
	}

	// Get the list of all the camera rigs' interface parameters, and rebuild our
	// parameters property bag.
	TArray<FPropertyBagPropertyDesc> DefaultParameterProperties;
	for (const UCameraRigAsset* CameraRig : CameraRigs)
	{
		FCameraObjectInterfaceParameterBuilder::AppendDefaultParameterProperties(CameraRig, DefaultParameterProperties);
	}
	FInstancedPropertyBag DefaultParameters;
	DefaultParameters.AddProperties(DefaultParameterProperties);
	for (const UCameraRigAsset* CameraRig : CameraRigs)
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

	for (const UCameraRigAsset* CameraRig : CameraRigs)
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

