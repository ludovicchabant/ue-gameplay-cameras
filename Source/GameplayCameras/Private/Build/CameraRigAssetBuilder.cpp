// Copyright Epic Games, Inc. All Rights Reserved.

#include "Build/CameraRigAssetBuilder.h"

#include "Build/CameraNodeHierarchyBuilder.h"
#include "Build/CameraObjectInterfaceBuilder.h"
#include "Build/CameraObjectInterfaceParameterBuilder.h"
#include "Core/CameraRigAsset.h"
#include "GameplayCamerasDelegates.h"

#define LOCTEXT_NAMESPACE "CameraRigAssetBuilder"

namespace UE::Cameras
{

FCameraRigAssetBuilder::FCameraRigAssetBuilder(FCameraBuildLog& InBuildLog)
	: BuildLog(InBuildLog)
{
}

void FCameraRigAssetBuilder::BuildCameraRig(UCameraRigAsset* InCameraRig)
{
	BuildCameraRig(InCameraRig, FCustomBuildStep::CreateLambda([](UCameraRigAsset*, FCameraBuildLog&) {}));
}

void FCameraRigAssetBuilder::BuildCameraRig(UCameraRigAsset* InCameraRig, FCustomBuildStep InCustomBuildStep)
{
	if (!ensure(InCameraRig))
	{
		return;
	}

	CameraRig = InCameraRig;

	BuildLog.SetLoggingPrefix(CameraRig->GetPathName() + TEXT(": "));
	{
		BuildCameraRigImpl();

		InCustomBuildStep.ExecuteIfBound(CameraRig, BuildLog);

		CameraRig->EventHandlers.Notify(&ICameraRigAssetEventHandler::OnCameraRigBuilt, CameraRig);
	}
	BuildLog.SetLoggingPrefix(FString());
	UpdateBuildStatus();

	FGameplayCamerasDelegates::OnCameraRigAssetBuilt().Broadcast(CameraRig);

	CameraRig = nullptr;
}

void FCameraRigAssetBuilder::BuildCameraRigImpl()
{
	FCameraNodeHierarchyBuilder NodeBuilder(BuildLog, CameraRig);
	NodeBuilder.PreBuild();

	FCameraObjectInterfaceBuilder InterfaceBuilder(BuildLog);
	InterfaceBuilder.BuildInterface(CameraRig, NodeBuilder.GetHierarchy(), true);

	NodeBuilder.Build();

	FCameraObjectInterfaceParameterBuilder ParameterBuilder;
	ParameterBuilder.BuildParameters(CameraRig);
}

void FCameraRigAssetBuilder::UpdateBuildStatus()
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
	CameraRig->BuildStatus = BuildStatus;
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

