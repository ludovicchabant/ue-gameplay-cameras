// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BlendStackRootCameraNode.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraRigAsset.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlendStackRootCameraNode)

FCameraNodeChildrenView UBlendStackRootCameraNode::OnGetChildren()
{
	FCameraNodeChildrenView Children;
	if (Blend)
	{
		Children.Add(Blend);
	}
	if (RootNode)
	{
		Children.Add(RootNode);
	}
	return Children;
}

FCameraNodeEvaluatorPtr UBlendStackRootCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FBlendStackRootCameraNodeEvaluator>();
}

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBlendStackRootCameraNodeEvaluator)

UE_DECLARE_CAMERA_DEBUG_BLOCK_START(GAMEPLAYCAMERAS_API, FBlendStackRootCameraDebugBlock)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FString, CameraRigAssetName)
UE_DECLARE_CAMERA_DEBUG_BLOCK_END()

UE_DEFINE_CAMERA_DEBUG_BLOCK_WITH_FIELDS(FBlendStackRootCameraDebugBlock)

FCameraNodeEvaluatorChildrenView FBlendStackRootCameraNodeEvaluator::OnGetChildren()
{
	FCameraNodeEvaluatorChildrenView Children;
	if (BlendEvaluator)
	{
		Children.Add(BlendEvaluator);
	}
	if (RootEvaluator)
	{
		Children.Add(RootEvaluator);
	}
	return Children;
}

void FBlendStackRootCameraNodeEvaluator::OnBuild(const FCameraNodeEvaluatorBuildParams& Params)
{
	const UBlendStackRootCameraNode* RootNode = GetCameraNodeAs<UBlendStackRootCameraNode>();
	BlendEvaluator = Params.BuildEvaluatorAs<FBlendCameraNodeEvaluator>(RootNode->Blend);
	RootEvaluator = Params.BuildEvaluator(RootNode->RootNode);
}

void FBlendStackRootCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
#if UE_GAMEPLAY_CAMERAS_DEBUG
	const UBlendStackRootCameraNode* RootNode = GetCameraNodeAs<UBlendStackRootCameraNode>();
	if (RootNode->RootNode)
	{
		if (const UCameraRigAsset* CameraRig = RootNode->RootNode->GetTypedOuter<UCameraRigAsset>())
		{
			CameraRigAssetName = GetNameSafe(CameraRig);
		}
	}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
}

void FBlendStackRootCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	if (BlendEvaluator)
	{
		BlendEvaluator->Run(Params, OutResult);
	}
	if (RootEvaluator)
	{
		RootEvaluator->Run(Params, OutResult);
	}
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FBlendStackRootCameraNodeEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	FBlendStackRootCameraDebugBlock& DebugBlock = Builder.StartChildDebugBlock<FBlendStackRootCameraDebugBlock>();
	DebugBlock.CameraRigAssetName = CameraRigAssetName;

	if (BlendEvaluator)
	{
		BlendEvaluator->BuildDebugBlocks(Params, Builder);
	}
	else
	{
		// Dummy block.
		Builder.StartChildDebugBlock<FCameraDebugBlock>();
		Builder.EndChildDebugBlock();
	}

	if (RootEvaluator)
	{
		RootEvaluator->BuildDebugBlocks(Params, Builder);
	}
	else
	{
		// Dummy block.
		Builder.StartChildDebugBlock<FCameraDebugBlock>();
		Builder.EndChildDebugBlock();
	}

	Builder.EndChildDebugBlock();
	Builder.SkipChildren();
}

void FBlendStackRootCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	TArrayView<FCameraDebugBlock*> ChildrenView(GetChildren());

	Renderer.AddText("{cam_passive}<Blend>\n");
	Renderer.AddIndent();
	ChildrenView[0]->DebugDraw(Params, Renderer);
	Renderer.RemoveIndent();

	Renderer.AddText(TEXT("{cam_passive}<CameraRig %s>\n"), *CameraRigAssetName);
	Renderer.AddIndent();
	ChildrenView[1]->DebugDraw(Params, Renderer);
	Renderer.RemoveIndent();

	Renderer.SkipAllBlocks();
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

