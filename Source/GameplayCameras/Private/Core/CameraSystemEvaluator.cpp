// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraSystemEvaluator.h"

#include "Camera/CameraTypes.h"
#include "Core/CameraAsset.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraRigAsset.h"
#include "Core/DefaultRootCameraNode.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "Debug/CameraSystemTrace.h"
#include "Debug/CategoryTitleDebugBlock.h"
#include "Debug/RootCameraDebugBlock.h"
#include "HAL/IConsoleManager.h"
#include "IGameplayCamerasModule.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

DECLARE_CYCLE_STAT(TEXT("Camera System Eval"), CameraSystemEval_Total, STATGROUP_CameraSystem);

namespace UE::Cameras
{

extern bool GGameplayCamerasDebugEnable;

FCameraSystemEvaluator::FCameraSystemEvaluator()
{
}

void FCameraSystemEvaluator::Initialize(TObjectPtr<UObject> InOwner)
{
	FCameraSystemEvaluatorCreateParams Params;
	Params.Owner = InOwner;
	Initialize(Params);
}

void FCameraSystemEvaluator::Initialize(const FCameraSystemEvaluatorCreateParams& Params)
{
	UObject* Owner = Params.Owner;
	if (!Owner)
	{
		Owner = GetTransientPackage();
	}
	WeakOwner = Owner;

	if (Params.RootNodeFactory)
	{
		RootNode = Params.RootNodeFactory();
	}
	else
	{
		RootNode = NewObject<UDefaultRootCameraNode>(Owner, TEXT("RootNode"));
	}

	ContextStack.Initialize(*this);

	FCameraNodeEvaluatorTreeBuildParams BuildParams;
	BuildParams.RootCameraNode = RootNode;
	RootEvaluator = static_cast<FRootCameraNodeEvaluator*>(RootEvaluatorStorage.BuildEvaluatorTree(BuildParams));

	if (ensure(RootEvaluator))
	{
		FCameraNodeEvaluatorInitializeParams InitParams;
		InitParams.Evaluator = this;
		RootEvaluator->Initialize(InitParams);
	}
}

void FCameraSystemEvaluator::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(RootNode);
	ContextStack.AddReferencedObjects(Collector);
	if (RootEvaluator)
	{
		RootEvaluator->AddReferencedObjects(Collector);
	}
}

void FCameraSystemEvaluator::PushEvaluationContext(TSharedRef<FCameraEvaluationContext> EvaluationContext)
{
	ContextStack.PushContext(EvaluationContext);
}

void FCameraSystemEvaluator::RemoveEvaluationContext(TSharedRef<FCameraEvaluationContext> EvaluationContext)
{
	ContextStack.RemoveContext(EvaluationContext);
}

void FCameraSystemEvaluator::PopEvaluationContext()
{
	ContextStack.PopContext();
}

void FCameraSystemEvaluator::Update(const FCameraSystemEvaluationUpdateParams& Params)
{
	SCOPE_CYCLE_COUNTER(CameraSystemEval_Total);

	// Get the active evaluation context.
	TSharedPtr<FCameraEvaluationContext> ActiveContext = ContextStack.GetActiveContext();
	if (UNLIKELY(!ActiveContext.IsValid()))
	{
		Result.bIsValid = false;
		return;
	}

	// Run the camera director, and activate any camera rig(s) it returns to us.
	FCameraDirectorEvaluator* ActiveDirectorEvaluator = ActiveContext->GetDirectorEvaluator();
	if (ActiveDirectorEvaluator)
	{
		FCameraDirectorEvaluationParams DirectorParams;
		DirectorParams.DeltaTime = Params.DeltaTime;
		DirectorParams.OwnerContext = ActiveContext;

		FCameraDirectorEvaluationResult DirectorResult;

		ActiveDirectorEvaluator->Run(DirectorParams, DirectorResult);

		if (DirectorResult.ActiveCameraRigs.Num() == 1)
		{
			const FActiveCameraRigInfo& ActiveCameraRig = DirectorResult.ActiveCameraRigs[0];

			FActivateCameraRigParams CameraRigParams;
			CameraRigParams.Evaluator = this;
			CameraRigParams.EvaluationContext = ActiveCameraRig.EvaluationContext;
			CameraRigParams.CameraRig = ActiveCameraRig.CameraRig;
			RootEvaluator->ActivateCameraRig(CameraRigParams);
		}
		// TODO: handle the case of composite camera rigs.
	}

	// Setup the params/result for running the root camera node.
	FCameraNodeEvaluationParams NodeParams;
	NodeParams.Evaluator = this;
	NodeParams.DeltaTime = Params.DeltaTime;

	RootNodeResult.Reset();

	// Run the root camera node.
	RootEvaluator->Run(NodeParams, RootNodeResult);

	// Harvest the result.
	Result.CameraPose = RootNodeResult.CameraPose;
	Result.bIsCameraCut = RootNodeResult.bIsCameraCut;
	Result.bIsValid = true;
}

void FCameraSystemEvaluator::GetEvaluatedCameraView(FMinimalViewInfo& DesiredView)
{
	const FCameraPose& CameraPose = Result.CameraPose;
	DesiredView.Location = CameraPose.GetLocation();
	DesiredView.Rotation = CameraPose.GetRotation();
	DesiredView.FOV = CameraPose.GetEffectiveFieldOfView();
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FCameraSystemEvaluator::DebugUpdate(const FCameraSystemDebugUpdateParams& Params)
{
#if UE_GAMEPLAY_CAMERAS_TRACE
	const bool bTraceEnabled = FCameraSystemTrace::IsTraceEnabled();
#else
	const bool bTraceEnabled = false;
#endif  // UE_GAMEPLAY_CAMERAS_TRACE
	if (!bTraceEnabled && !GGameplayCamerasDebugEnable)
	{
		return;
	}

#if UE_GAMEPLAY_CAMERAS_TRACE
	if (FCameraSystemTrace::IsTraceReplay())
	{
		return;
	}
#endif  // UE_GAMEPLAY_CAMERAS_TRACE

	// Clear previous frame's debug info and make room for this frame's.
	DebugBlockStorage.DestroyDebugBlocks();

	// Create the root debug block and start building more.
	RootDebugBlock = DebugBlockStorage.BuildDebugBlock<FRootCameraDebugBlock>();

	FCameraDebugBlockBuildParams BuildParams;
	FCameraDebugBlockBuilder DebugBlockBuilder(DebugBlockStorage, *RootDebugBlock);
	RootDebugBlock->BuildDebugBlocks(*this, BuildParams, DebugBlockBuilder);

	UObject* Owner = WeakOwner.Get();
	UWorld* OwnerWorld = Owner ? Owner->GetWorld() : nullptr;

#if UE_GAMEPLAY_CAMERAS_TRACE
	if (bTraceEnabled)
	{
		FCameraSystemTrace::TraceEvaluation(OwnerWorld, Result, *RootDebugBlock);
	}
#endif
	
	FCameraDebugRenderer Renderer(OwnerWorld, Params.Canvas);
	RootDebugBlock->RootDebugDraw(Renderer);
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

