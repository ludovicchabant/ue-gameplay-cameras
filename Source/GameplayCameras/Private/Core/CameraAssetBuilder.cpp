// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraAssetBuilder.h"

#include "Core/CameraDirector.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetBuilder.h"
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
}

void FCameraAssetBuilder::BuildCameraImpl()
{
	if (UCameraDirector* CameraDirector = CameraAsset->GetCameraDirector())
	{
		CameraDirector->BuildCameraDirector(BuildLog);
	}
	else
	{
		BuildLog.AddMessage(EMessageSeverity::Error, LOCTEXT("MissingDirector", "Camera has no director set."));
	}

	if (CameraAsset->GetCameraRigs().IsEmpty())
	{
		BuildLog.AddMessage(EMessageSeverity::Warning, LOCTEXT("MissingRigs", "Camera has no camera rigs defined."));
	}

	for (UCameraRigAsset* CameraRig : CameraAsset->GetCameraRigs())
	{
		FCameraRigAssetBuilder CameraRigBuilder(BuildLog);
		CameraRigBuilder.BuildCameraRig(CameraRig);
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

