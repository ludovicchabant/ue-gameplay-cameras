// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CategoryTitleDebugBlock.h"

#include "Debug/CameraDebugRenderer.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

UE_DEFINE_CAMERA_DEBUG_BLOCK(FCategoryTitleDebugBlock)

FCategoryTitleDebugBlock::FCategoryTitleDebugBlock()
	: TitleColor(FColor::Green)
{
}

FCategoryTitleDebugBlock::FCategoryTitleDebugBlock(const FString& InCategory, const FString& InTitle)
	: Category(InCategory)
	, Title(InTitle)
	, TitleColor(FColor::Green)
{
}

void FCategoryTitleDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	if (Params.IsCategoryActive(Category))
	{
		if (!Title.IsEmpty())
		{
			Renderer.AddText(TEXT("{cam_title}%s{cam_default}\n"), *Title);
		}
	}
	else
	{
		if (bSkipAttachedBlocksIfInactive)
		{
			Renderer.SkipAttachedBlocks();
		}
		if (bSkipChildrenBlocksIfInactive)
		{
			Renderer.SkipChildrenBlocks();
		}
	}
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

