// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/ObjectPtr.h"

class UCameraRigAsset;
struct FCameraRigAllocationInfo;

namespace UE::Cameras
{

class FCameraBuildLog;

/**
 * Camera rig asset build context.
 */
struct FCameraRigBuildContext
{
	FCameraRigBuildContext(
			FCameraRigAllocationInfo& InAllocationInfo, FCameraBuildLog& InBuildLog)
		: AllocationInfo(InAllocationInfo)
		, BuildLog(InBuildLog)
	{}

	/** The allocation information to be determined. */
	FCameraRigAllocationInfo& AllocationInfo;
	/** The build log for emitting messages. */
	FCameraBuildLog& BuildLog;
};

}  // namespace UE::Cameras

