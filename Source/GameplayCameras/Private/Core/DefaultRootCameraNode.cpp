// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DefaultRootCameraNode.h"

#include "Core/BlendStackCameraNode.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/RootCameraNodeCameraRigEvent.h"
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
	BaseLayer = BuildBlendStackEvaluator(Params, Data->BaseLayer);
	MainLayer = BuildBlendStackEvaluator(Params, Data->MainLayer);
	GlobalLayer = BuildBlendStackEvaluator(Params, Data->GlobalLayer);
	VisualLayer = BuildBlendStackEvaluator(Params, Data->VisualLayer);
}

FBlendStackCameraNodeEvaluator* FDefaultRootCameraNodeEvaluator::BuildBlendStackEvaluator(const FCameraNodeEvaluatorBuildParams& Params, UBlendStackCameraNode* BlendStackNode)
{
	FBlendStackCameraNodeEvaluator* BlendStackEvaluator = Params.BuildEvaluatorAs<FBlendStackCameraNodeEvaluator>(BlendStackNode);
	BlendStackEvaluator->OnCameraRigEvent().AddRaw(this, &FDefaultRootCameraNodeEvaluator::OnBlendStackEvent);
	return BlendStackEvaluator;
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
	FBlendStackCameraNodeEvaluator* TargetStack = GetBlendStackEvaluator(Params.Layer);
	if (ensure(TargetStack))
	{
		FBlendStackCameraPushParams PushParams;
		PushParams.Evaluator = Params.Evaluator;
		PushParams.EvaluationContext = Params.EvaluationContext;
		PushParams.CameraRig = Params.CameraRig;
		TargetStack->Push(PushParams);
	}
}

void FDefaultRootCameraNodeEvaluator::OnRunSingleCameraRig(const FSingleCameraRigEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	BaseLayer->Run(Params.EvaluationParams, OutResult);

	FCameraNodeEvaluator* RootEvaluator = Params.CameraRigInfo.RootEvaluator;

	{
		const FCameraNodeEvaluationResult* CameraRigResult = Params.CameraRigInfo.LastResult;
		FCameraBlendedParameterUpdateParams InputParams(Params.EvaluationParams, CameraRigResult->CameraPose);
		FCameraBlendedParameterUpdateResult InputResult(OutResult.VariableTable);
		RootEvaluator->UpdateParameters(InputParams, InputResult);
	}

	{
		OutResult.CameraPose.ClearAllChangedFlags();
		OutResult.VariableTable.ClearAllWrittenThisFrameFlags();

		const FCameraNodeEvaluationResult& InitialResult = Params.CameraRigInfo.EvaluationContext->GetInitialResult();
		OutResult.CameraPose.OverrideChanged(InitialResult.CameraPose);
		OutResult.VariableTable.OverrideAll(InitialResult.VariableTable);

		OutResult.bIsValid = true;

		RootEvaluator->Run(Params.EvaluationParams, OutResult);
	}

	GlobalLayer->Run(Params.EvaluationParams, OutResult);
	// Don't run the visual layer.
}

FBlendStackCameraNodeEvaluator* FDefaultRootCameraNodeEvaluator::GetBlendStackEvaluator(ECameraRigLayer Layer) const
{
	switch (Layer)
	{
		case ECameraRigLayer::Base:
			return BaseLayer;
		case ECameraRigLayer::Main:
			return MainLayer;
		case ECameraRigLayer::Global:
			return GlobalLayer;
		case ECameraRigLayer::Visual:
			return VisualLayer;
		default:
			return nullptr;
	}
}

void FDefaultRootCameraNodeEvaluator::OnBlendStackEvent(const FBlendStackCameraRigEvent& InEvent)
{
	if (InEvent.EventType == EBlendStackCameraRigEventType::Pushed ||
			InEvent.EventType == EBlendStackCameraRigEventType::Popped)
	{
		FRootCameraNodeCameraRigEvent RootEvent;
		RootEvent.CameraRigInfo = InEvent.CameraRigInfo;
		RootEvent.Transition = InEvent.Transition;

		switch (InEvent.EventType)
		{
		case EBlendStackCameraRigEventType::Pushed:
			RootEvent.EventType = ERootCameraNodeCameraRigEventType::Activated;
			break;
		case EBlendStackCameraRigEventType::Popped:
			RootEvent.EventType = ERootCameraNodeCameraRigEventType::Deactivated;
			break;
		}

		if (InEvent.BlendStackEvaluator == BaseLayer)
		{
			RootEvent.EventLayer = ECameraRigLayer::Base;
		}
		else if (InEvent.BlendStackEvaluator == MainLayer)
		{
			RootEvent.EventLayer = ECameraRigLayer::Main;
		}
		else if (InEvent.BlendStackEvaluator == GlobalLayer)
		{
			RootEvent.EventLayer = ECameraRigLayer::Global;
		}
		else if (InEvent.BlendStackEvaluator == VisualLayer)
		{
			RootEvent.EventLayer = ECameraRigLayer::Visual;
		}

		BroadcastCameraRigEvent(RootEvent);
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

