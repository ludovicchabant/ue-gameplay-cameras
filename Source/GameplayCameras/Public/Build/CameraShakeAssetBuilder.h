// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Build/CameraBuildLog.h"
#include "Core/CameraNodeHierarchy.h"

class UCameraShakeAsset;

namespace UE::Cameras
{

class GAMEPLAYCAMERAS_API FCameraShakeAssetBuilder
{
public:

	FCameraShakeAssetBuilder(FCameraBuildLog& InBuildLog);

	void BuildCameraShake(UCameraShakeAsset* InCameraShake);

private:

	void BuildCameraShakeImpl();

	void UpdateBuildStatus();

private:

	FCameraBuildLog& BuildLog;

	UCameraShakeAsset* CameraShake = nullptr;

	FCameraNodeHierarchy CameraNodeHierarchy;
};

}  // namespace UE::Cameras

