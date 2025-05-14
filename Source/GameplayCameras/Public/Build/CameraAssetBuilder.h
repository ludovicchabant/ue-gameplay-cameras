// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Build/CameraBuildLog.h"
#include "CoreTypes.h"
#include "GameplayCameras.h"
#include "Templates/Tuple.h"

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

	DECLARE_DELEGATE_TwoParams(FCustomBuildStep, UCameraAsset*, FCameraBuildLog&);

	/** Creates a new camera builder. */
	UE_API FCameraAssetBuilder(FCameraBuildLog& InBuildLog);

	/** Builds the given camera. */
	UE_API void BuildCamera(UCameraAsset* InCameraAsset);

	/** Builds the given camera. */
	UE_API void BuildCamera(UCameraAsset* InCameraAsset, FCustomBuildStep InCustomBuildStep);

private:

	void BuildCameraImpl();

	void UpdateBuildStatus();

private:

	FCameraBuildLog& BuildLog;

	UCameraAsset* CameraAsset = nullptr;
};

}  // namespace UE::Cameras

#undef UE_API
