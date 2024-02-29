// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDebugBlockStorage.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

void FCameraDebugBlockStorage::DestroyDebugBlocks(bool bFreeAllocations)
{
	Super::DestroyObjects(bFreeAllocations);
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

