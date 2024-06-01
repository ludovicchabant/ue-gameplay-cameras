// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraBuildLog.h"
#include "CoreTypes.h"
#include "Templates/Tuple.h"

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
	FCameraAssetBuilder(FCameraBuildLog& InBuildLog);

	/** Builds the given camera. */
	void BuildCamera(UCameraAsset* InCameraAsset);

private:

	void BuildCameraImpl();

	void UpdateBuildStatus();

private:

	FCameraBuildLog& BuildLog;

	UCameraAsset* CameraAsset = nullptr;

	bool bHasErrors;
	bool bHasWarnings;
};

}  // namespace UE::Cameras

