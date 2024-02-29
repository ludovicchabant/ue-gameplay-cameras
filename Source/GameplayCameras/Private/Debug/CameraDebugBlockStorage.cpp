// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDebugBlockStorage.h"

namespace UE::Cameras
{

void FCameraDebugBlockStorage::DestroyDebugBlocks(bool bFreeAllocations)
{
	Super::DestroyObjects(bFreeAllocations);
}

}  // namespace UE::Cameras

