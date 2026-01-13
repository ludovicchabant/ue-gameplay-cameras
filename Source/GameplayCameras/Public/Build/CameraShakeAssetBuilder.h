// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Build/CameraBuildContext.h"
#include "Core/CameraNodeHierarchy.h"

#define UE_API GAMEPLAYCAMERAS_API

class UCameraShakeAsset;

namespace UE::Cameras
{

class FCameraShakeAssetBuilder
{
public:

	UE_API FCameraShakeAssetBuilder(FCameraBuildContext& InBuildContext);

	UE_API void BuildCameraShake(UCameraShakeAsset* InCameraShake);

private:

	void BuildCameraShakeImpl();

	void UpdateBuildStatus();

private:

	FCameraBuildContext BuildContext;

	UCameraShakeAsset* CameraShake = nullptr;

	FCameraNodeHierarchy CameraNodeHierarchy;
};

}  // namespace UE::Cameras

#undef UE_API
