// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Build/CameraBuildLog.h"
#include "CoreTypes.h"
#include "GameplayCameras.h"

#define UE_API GAMEPLAYCAMERAS_API

class UCameraAsset;

namespace UE::Cameras
{

/**
 * A class that can prepare a camera asset for runtime use.
 */
class FCameraAssetBuilder
{
public:

	/** Creates a new camera builder. */
	UE_API FCameraAssetBuilder(FCameraBuildLog& InBuildLog);

	/** Builds the given camera. */
	UE_API void BuildCamera(UCameraAsset* InCameraAsset, bool bBuildReferencedAssets = true);

private:

	void BuildCameraImpl(bool bBuildReferencedAssets);

	void UpdateBuildStatus();

private:

	FCameraBuildLog& BuildLog;

	UCameraAsset* CameraAsset = nullptr;
};

}  // namespace UE::Cameras

#undef UE_API
