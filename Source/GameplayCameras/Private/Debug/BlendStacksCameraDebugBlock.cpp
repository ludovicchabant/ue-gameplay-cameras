// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/BlendStacksCameraDebugBlock.h"

#include "Core/BlendStackCameraNode.h"
#include "Debug/CameraDebugCategories.h"
#include "Debug/CameraDebugColors.h"
#include "Debug/CameraDebugRenderer.h"
#include "Debug/CameraNodeEvaluatorDebugBlock.h"
#include "Math/ColorList.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

UE_DEFINE_CAMERA_DEBUG_BLOCK(FBlendStacksCameraDebugBlock)

FBlendStacksCameraDebugBlock::FBlendStacksCameraDebugBlock()
{
}

void FBlendStacksCameraDebugBlock::AddBlendStack(const FString& InBlendStackName, FBlendStackCameraDebugBlock* InDebugBlock)
{
	BlendStackNames.Add(InBlendStackName);
	AddChild(InDebugBlock);
}

void FBlendStacksCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	if (!Params.IsCategoryActive(FCameraDebugCategories::BlendStacks))
	{
		Renderer.SkipAllBlocks();
		return;
	}
	
	const FCameraDebugColors& Colors = FCameraDebugColors::Get();

	Renderer.SetTextColor(Colors.Title);
	Renderer.AddText(TEXT("Blend Stacks\n\n"));
	Renderer.SetTextColor(Colors.Default);

	TArrayView<FCameraDebugBlock*> BlendStackBlocks(GetChildren());

	const int32 MaxIndex = FMath::Max(BlendStackBlocks.Num(), BlendStackNames.Num());

	for (int32 Index = 0; Index < MaxIndex; ++Index)
	{
		Renderer.AddText(BlendStackNames.IsValidIndex(Index) ? BlendStackNames[Index] : TEXT("<unnamed blend stack>"));
		Renderer.NewLine();

		if (BlendStackBlocks.IsValidIndex(Index))
		{
			FCameraDebugBlock* BlendStackBlock = BlendStackBlocks[Index];
			BlendStackBlock->DebugDraw(Params, Renderer);
		}
		else
		{
			Renderer.AddText(TEXT("<missing blend stack>"));
		}
	}

	// We've already manually renderered our children blocks.
	Renderer.SkipAllBlocks();
}

void FBlendStacksCameraDebugBlock::OnSerialize(FArchive& Ar)
{
	Ar << BlendStackNames;
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

