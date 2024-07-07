// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraBuildLog.h"
#include "CoreTypes.h"
#include "GameplayCameras.h"
#include "Templates/Tuple.h"

class UCameraAsset;

namespace UE::Cameras
{

/**
 * A class that can prepare a camera asset for runtime use.
 */
class GAMEPLAYCAMERAS_API FCameraAssetBuilder
{
public:

	DECLARE_DELEGATE_TwoParams(FCustomBuildStep, UCameraAsset*, FCameraBuildLog&);

	/** Creates a new camera builder. */
	FCameraAssetBuilder(FCameraBuildLog& InBuildLog);

	/** Builds the given camera. */
	void BuildCamera(UCameraAsset* InCameraAsset);

	/** Builds the given camera. */
	void BuildCamera(UCameraAsset* InCameraAsset, FCustomBuildStep InCustomBuildStep);

private:

	void BuildCameraImpl();

	void UpdateBuildStatus();

private:

	FCameraBuildLog& BuildLog;

	UCameraAsset* CameraAsset = nullptr;
};

}  // namespace UE::Cameras

