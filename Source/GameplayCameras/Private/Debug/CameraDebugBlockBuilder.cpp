// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDebugBlockBuilder.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

FCameraDebugBlockBuilder::FCameraDebugBlockBuilder(FCameraDebugBlockStorage& InStorage, FCameraDebugBlock& InRootBlock)
	: Storage(InStorage)
{
	CurrentHierarchy.Add(&InRootBlock);
}

void FCameraDebugBlockBuilder::OnStartBlock(FCameraDebugBlock& InNewBlock)
{
	checkf(CurrentHierarchy.Num() > 0, TEXT("There should always be the root block in the current hierarchy!"));

	// Add the new block under the current parent.
	FCameraDebugBlock* ParentBlock = CurrentHierarchy.Last();
	ParentBlock->AddChild(&InNewBlock);
	CurrentHierarchy.Add(&InNewBlock);
}

void FCameraDebugBlockBuilder::EndBlock()
{
	checkf(CurrentHierarchy.Num() > 0, TEXT("Can't end block, no current block defined!"));

	CurrentHierarchy.Pop();
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

