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
	bHasErrors = false;
	bHasWarnings = false;
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
		bHasErrors = true;
	}

	if (CameraAsset->CameraRigs.IsEmpty())
	{
		BuildLog.AddMessage(EMessageSeverity::Warning, LOCTEXT("MissingRigs", "Camera has no camera rigs defined."));
		bHasWarnings = true;
	}

	for (UCameraRigAsset* CameraRig : CameraAsset->CameraRigs)
	{
		FCameraRigAssetBuilder CameraRigBuilder(BuildLog);
		CameraRigBuilder.BuildCameraRig(CameraRig);
		bHasErrors |= CameraRigBuilder.LastBuildHadErrors();
		bHasWarnings |= CameraRigBuilder.LastBuildHadWarnings();
	}
}

void FCameraAssetBuilder::UpdateBuildStatus()
{
	ECameraBuildStatus BuildStatus = ECameraBuildStatus::Clean;
	if (bHasErrors)
	{
		BuildStatus = ECameraBuildStatus::WithErrors;
	}
	else if (bHasWarnings)
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

