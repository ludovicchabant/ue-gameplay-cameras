// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DefaultRootCameraNode.h"

#include "Core/BlendStackCameraNode.h"
#include "Core/CameraRigAsset.h"
#include "Debug/BlendStacksCameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/RootCameraDebugBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DefaultRootCameraNode)

namespace UE::Cameras::Private
{

TObjectPtr<UBlendStackCameraNode> CreateBlendStack(
		UObject* This, const FObjectInitializer& ObjectInit,
		const FName& Name, bool bAutoPop = true, bool bBlendFirstCameraRig = false)
{
	TObjectPtr<UBlendStackCameraNode> NewBlendStack = ObjectInit.CreateDefaultSubobject<UBlendStackCameraNode>(
			This, Name);
	NewBlendStack->bAutoPop = bAutoPop;
	NewBlendStack->bBlendFirstCameraRig = bBlendFirstCameraRig;
	return NewBlendStack;
}

}  // namespace UE::Cameras::Private

UDefaultRootCameraNode::UDefaultRootCameraNode(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	using namespace UE::Cameras::Private;

	BaseLayer = CreateBlendStack(this, ObjectInit, TEXT("BaseLayer"), false, true);
	MainLayer = CreateBlendStack(this, ObjectInit, TEXT("MainLayer"));
	GlobalLayer = CreateBlendStack(this, ObjectInit, TEXT("GlobalLayer"), false, true);
	VisualLayer = CreateBlendStack(this, ObjectInit, TEXT("VisualLayer"), false, true);
}

FCameraNodeEvaluatorPtr UDefaultRootCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FDefaultRootCameraNodeEvaluator>();
}

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FDefaultRootCameraNodeEvaluator)

void FDefaultRootCameraNodeEvaluator::OnBuild(const FCameraNodeEvaluatorBuildParams& Params)
{
	const UDefaultRootCameraNode* Data = GetCameraNodeAs<UDefaultRootCameraNode>();
	BaseLayer = Params.BuildEvaluatorAs<FBlendStackCameraNodeEvaluator>(Data->BaseLayer);
	MainLayer = Params.BuildEvaluatorAs<FBlendStackCameraNodeEvaluator>(Data->MainLayer);
	GlobalLayer = Params.BuildEvaluatorAs<FBlendStackCameraNodeEvaluator>(Data->GlobalLayer);
	VisualLayer = Params.BuildEvaluatorAs<FBlendStackCameraNodeEvaluator>(Data->VisualLayer);
}

FCameraNodeEvaluatorChildrenView FDefaultRootCameraNodeEvaluator::OnGetChildren()
{
	return FCameraNodeEvaluatorChildrenView({ BaseLayer, MainLayer, GlobalLayer, VisualLayer });
}

void FDefaultRootCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	BaseLayer->Run(Params, OutResult);
	MainLayer->Run(Params, OutResult);
	GlobalLayer->Run(Params, OutResult);
	VisualLayer->Run(Params, OutResult);
}

void FDefaultRootCameraNodeEvaluator::OnActivateCameraRig(const FActivateCameraRigParams& Params)
{
	FBlendStackCameraNodeEvaluator* TargetStack = nullptr;
	switch (Params.Layer)
	{
		case ECameraRigLayer::Base:
			TargetStack = BaseLayer;
			break;
		case ECameraRigLayer::Main:
			TargetStack = MainLayer;
			break;
		case ECameraRigLayer::Global:
			TargetStack = GlobalLayer;
			break;
		case ECameraRigLayer::Visual:
			TargetStack = VisualLayer;
			break;
	}

	if (ensure(TargetStack))
	{
		FBlendStackCameraPushParams PushParams;
		PushParams.Evaluator = Params.Evaluator;
		PushParams.EvaluationContext = Params.EvaluationContext;
		PushParams.CameraRig = Params.CameraRig;
		TargetStack->Push(PushParams);
	}
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

UE_DECLARE_CAMERA_DEBUG_BLOCK_START(GAMEPLAYCAMERAS_API, FDefaultRootCameraNodeEvaluatorDebugBlock)
UE_DECLARE_CAMERA_DEBUG_BLOCK_END()

UE_DEFINE_CAMERA_DEBUG_BLOCK_WITH_FIELDS(FDefaultRootCameraNodeEvaluatorDebugBlock)

void FDefaultRootCameraNodeEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	// Create the debug block that shows the overall blend stack layers.
	FBlendStacksCameraDebugBlock& DebugBlock = Builder.BuildDebugBlock<FBlendStacksCameraDebugBlock>();
	{
		DebugBlock.AddBlendStack(TEXT("Base Layer"), BaseLayer->BuildDetailedDebugBlock(Params, Builder));
		DebugBlock.AddBlendStack(TEXT("Main Layer"), MainLayer->BuildDetailedDebugBlock(Params, Builder));
		DebugBlock.AddBlendStack(TEXT("Global Layer"), GlobalLayer->BuildDetailedDebugBlock(Params, Builder));
		DebugBlock.AddBlendStack(TEXT("Visual Layer"), VisualLayer->BuildDetailedDebugBlock(Params, Builder));
	}

	Builder.GetRootDebugBlock().AddChild(&DebugBlock);
}

void FDefaultRootCameraNodeEvaluatorDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

