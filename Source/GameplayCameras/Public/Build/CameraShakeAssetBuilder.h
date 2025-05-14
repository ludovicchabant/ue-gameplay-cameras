// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Build/CameraBuildLog.h"
#include "Core/CameraNodeHierarchy.h"

#define UE_API GAMEPLAYCAMERAS_API

class UCameraShakeAsset;

namespace UE::Cameras
{

class FCameraShakeAssetBuilder
{
public:

	UE_API FCameraShakeAssetBuilder(FCameraBuildLog& InBuildLog);

	UE_API void BuildCameraShake(UCameraShakeAsset* InCameraShake);

private:

	void BuildCameraShakeImpl();

	void UpdateBuildStatus();

private:

	FCameraBuildLog& BuildLog;

	UCameraShakeAsset* CameraShake = nullptr;

	FCameraNodeHierarchy CameraNodeHierarchy;
};

}  // namespace UE::Cameras

#undef UE_API
