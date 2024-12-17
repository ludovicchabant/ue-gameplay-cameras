// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraAssetBuilder.h"

#include "Core/CameraAsset.h"
#include "Core/CameraDirector.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetBuilder.h"
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

	// Get the list of all the camera rigs' interface parameters, and rebuild our
	// parameters property bag.
	int32 NextParameterPropertyIndex = 0;
	TArray<FPropertyBagPropertyDesc> DefaultParameterProperties;
	TArray<TWeakObjectPtr<const UCameraRigAsset>> ParameterOwners;

	for (const UCameraRigAsset* CameraRig : CameraRigs)
	{
		FCameraRigAssetBuilder::AppendDefaultParameters(CameraRig->Interface, DefaultParameterProperties);

		for (int32 Index = NextParameterPropertyIndex; Index < DefaultParameterProperties.Num(); ++Index)
		{
			ParameterOwners.Add(CameraRig);
		}
		NextParameterPropertyIndex = DefaultParameterProperties.Num();
	}

	FInstancedPropertyBag DefaultParameters;
	DefaultParameters.AddProperties(DefaultParameterProperties);

	if (DefaultParameters.GetPropertyBagStruct() != CameraAsset->DefaultParameters.GetPropertyBagStruct() ||
			ParameterOwners != CameraAsset->ParameterOwners)
	{
		CameraAsset->Modify();
		CameraAsset->DefaultParameters = DefaultParameters;
		CameraAsset->ParameterOwners = ParameterOwners;
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

