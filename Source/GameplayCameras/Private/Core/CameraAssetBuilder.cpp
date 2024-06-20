// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraAssetBuilder.h"

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraParameters.h"
#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetBuilder.h"
#include "Core/CameraVariableAssets.h"
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
	if (!ensure(InCameraAsset))
	{
		return;
	}

	CameraAsset = InCameraAsset;
	BuildLog.SetLoggingPrefix(InCameraAsset->GetPathName() + TEXT(": "));
	{
		BuildCameraImpl();
	}
	BuildLog.SetLoggingPrefix(FString());
	UpdateBuildStatus();
}

void FCameraAssetBuilder::BuildCameraImpl()
{
	if (!CameraAsset->CameraDirector)
	{
		BuildLog.AddMessage(EMessageSeverity::Error, LOCTEXT("MissingDirector", "Camera has no director set."));
	}

	if (CameraAsset->CameraRigs.IsEmpty())
	{
		BuildLog.AddMessage(EMessageSeverity::Warning, LOCTEXT("MissingRigs", "Camera has no camera rigs defined."));
	}

	for (UCameraRigAsset* CameraRig : CameraAsset->CameraRigs)
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

	if (CameraAsset->BuildStatus != BuildStatus)
	{
		CameraAsset->Modify();
		CameraAsset->BuildStatus = BuildStatus;
	}
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

