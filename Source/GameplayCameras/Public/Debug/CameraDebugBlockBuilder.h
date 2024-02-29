// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "CoreTypes.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockStorage.h"
#include "GameplayCameras.h"
#include "Templates/SharedPointer.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

class FCameraDebugBlock;

/**
 * Builder class for camera debug drawing blocks.
 */
struct FCameraDebugBlockBuilder
{
public:

	/** Creates a new builder structure. */
	FCameraDebugBlockBuilder(FCameraDebugBlockStorage& InStorage, FCameraDebugBlock& InRootBlock);

	/**
	 * Creates a new debug drawing block.
	 * This sets the new block as the "active" block, and adds it as a child of the
	 * previously active block.
	 */
	template<typename BlockType, typename ...ArgTypes>
	BlockType& StartBlock(ArgTypes&& ...InArgs)
	{
		BlockType* NewBlock = Storage.BuildDebugBlock<BlockType>(Forward<ArgTypes>(InArgs)...);
		OnStartBlock(*NewBlock);
		return *NewBlock;
	}

	/** Ends the currently active debug drawing block. */
	void EndBlock();

private:

	void OnStartBlock(FCameraDebugBlock& InNewBlock);

private:

	FCameraDebugBlockStorage& Storage;
	TArray<FCameraDebugBlock*> CurrentHierarchy;
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

