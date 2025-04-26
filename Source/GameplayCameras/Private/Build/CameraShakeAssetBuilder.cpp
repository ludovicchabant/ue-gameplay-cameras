// Copyright Epic Games, Inc. All Rights Reserved.

#include "Build/CameraShakeAssetBuilder.h"

#include "Build/CameraNodeHierarchyBuilder.h"
#include "Build/CameraObjectInterfaceBuilder.h"
#include "Build/CameraObjectInterfaceParameterBuilder.h"
#include "Core/CameraShakeAsset.h"
#include "GameplayCamerasDelegates.h"

#define LOCTEXT_NAMESPACE "CameraShakeAssetBuilder"

namespace UE::Cameras
{

FCameraShakeAssetBuilder::FCameraShakeAssetBuilder(FCameraBuildLog& InBuildLog)
	: BuildLog(InBuildLog)
{
}

void FCameraShakeAssetBuilder::BuildCameraShake(UCameraShakeAsset* InCameraShake)
{
	if (!ensure(InCameraShake))
	{
		return;
	}

	CameraShake = InCameraShake;

	BuildLog.SetLoggingPrefix(CameraShake->GetPathName() + TEXT(": "));
	{
		BuildCameraShakeImpl();
	}
	BuildLog.SetLoggingPrefix(FString());
	UpdateBuildStatus();

	FGameplayCamerasDelegates::OnCameraShakeAssetBuilt().Broadcast(CameraShake);

	CameraShake = nullptr;
}

void FCameraShakeAssetBuilder::BuildCameraShakeImpl()
{
	FCameraNodeHierarchyBuilder NodeBuilder(BuildLog, CameraShake);
	NodeBuilder.PreBuild();

	FCameraObjectInterfaceBuilder InterfaceBuilder(BuildLog);
	InterfaceBuilder.BuildInterface(CameraShake, NodeBuilder.GetHierarchy(), true);

	NodeBuilder.Build();

	FCameraObjectInterfaceParameterBuilder ParameterBuilder;
	ParameterBuilder.BuildParameters(CameraShake);
}

void FCameraShakeAssetBuilder::UpdateBuildStatus()
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

	// Don't modify the camera shake: BuildStatus is transient.
	CameraShake->BuildStatus = BuildStatus;
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

