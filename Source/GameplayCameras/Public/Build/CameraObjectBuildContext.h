// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BaseCameraObject.h"

namespace UE::Cameras
{

class FCameraBuildLog;

/**
 * Camera object build context.
 */
struct FCameraObjectBuildContext
{
	FCameraObjectBuildContext(FCameraBuildLog& InBuildLog)
		: BuildLog(InBuildLog)
	{}

	/** The build log for emitting messages. */
	FCameraBuildLog& BuildLog;

	/** The allocation information for the camera rig. */
	FCameraObjectAllocationInfo AllocationInfo;
};

}  // namespace UE::Cameras

