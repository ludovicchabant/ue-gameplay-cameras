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
#include "HAL/IConsoleManager.h"
#include "IGameplayCamerasModule.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

DECLARE_CYCLE_STAT(TEXT("Camera System Eval"), CameraSystemEval_Total, STATGROUP_CameraSystem);

#if UE_GAMEPLAY_CAMERAS_DEBUG

bool GGameplayCamerasDebugEnable = false;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugEnable(
	TEXT("GameplayCameras.Debug.Enable"),
	GGameplayCamerasDebugEnable,
	TEXT("(Default: false. Enables debug drawing for the GamplayCameras system."));

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

#if UE_GAMEPLAY_CAMERAS_DEBUG

class FRootCameraDebugBlock : public FCameraDebugBlock
{
public:

	float LastDeltaTime = 0.f;

protected:

	virtual EDebugDrawResult OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override
	{
		return GGameplayCamerasDebugEnable ?
			EDebugDrawResult::Default : EDebugDrawResult::SkipChildren;
	}
};

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

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

	if (Params.RootNodeFactory)
	{
		RootNode = Params.RootNodeFactory();
	}
	else
	{
		RootNode = NewObject<UDefaultRootCameraNode>(Owner, TEXT("RootNode"));
	}

	TSharedRef<FCameraSystemEvaluator> This(SharedThis(this));

	ContextStack.Initialize(This);

	FCameraNodeEvaluatorTreeBuildParams BuildParams;
	BuildParams.Evaluator = This;
	BuildParams.RootCameraNode = RootNode;
	RootEvaluator = static_cast<FRootCameraNodeEvaluator*>(RootEvaluatorStorage.BuildEvaluatorTree(BuildParams));
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
	FCameraEvaluationContextInfo ActiveContextInfo = ContextStack.GetActiveContext();
	if (UNLIKELY(!ActiveContextInfo.IsValid()))
	{
		Result.bIsValid = false;
		return;
	}

	TSharedPtr<FCameraSystemEvaluator> This(SharedThis(this));

	// Run the camera director, and activate any camera rig(s) it returns to us.
	FCameraDirectorEvaluator* ActiveDirectorEvaluator = ActiveContextInfo.Evaluator;
	if (ActiveDirectorEvaluator)
	{
		FCameraDirectorEvaluationParams DirectorParams;
		DirectorParams.DeltaTime = Params.DeltaTime;
		DirectorParams.OwnerContext = ActiveContextInfo.EvaluationContext;

		FCameraDirectorEvaluationResult DirectorResult;

		ActiveDirectorEvaluator->Run(DirectorParams, DirectorResult);

		if (DirectorResult.ActiveCameraRigs.Num() == 1)
		{
			FActivateCameraRigParams CameraRigParams;
			CameraRigParams.Evaluator = This;
			CameraRigParams.EvaluationContext = ActiveContextInfo.EvaluationContext;
			CameraRigParams.CameraRig = DirectorResult.ActiveCameraRigs[0];
			RootEvaluator->ActivateCameraRig(CameraRigParams);
		}
	}

	// Setup the params/result for running the root camera node.
	FCameraNodeEvaluationParams NodeParams;
	NodeParams.Evaluator = This;
	NodeParams.DeltaTime = Params.DeltaTime;

	RootNodeResult.Reset();

#if UE_GAMEPLAY_CAMERAS_DEBUG
	// Clear previous frame's debug info and make room for this frame's.
	DebugBlockStorage.DestroyDebugBlocks();
	RootDebugBlock = DebugBlockStorage.BuildDebugBlock<FRootCameraDebugBlock>();
	RootDebugBlock->LastDeltaTime = Params.DeltaTime;

	FCameraDebugBlockBuilder DebugBlockBuilder(DebugBlockStorage, *RootDebugBlock);
	RootNodeResult.DebugBlockBuilder = &DebugBlockBuilder;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

	// Run the root camera node.
	RootEvaluator->Run(NodeParams, RootNodeResult);

	// Harvest the result.
	Result.CameraPose = RootNodeResult.CameraPose;
	Result.bIsCameraCut = RootNodeResult.bIsCameraCut;
	Result.bIsValid = true;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	RootNodeResult.DebugBlockBuilder = nullptr;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
}

void FCameraSystemEvaluator::GetEvaluatedCameraView(FMinimalViewInfo& DesiredView)
{
	const FCameraPose& CameraPose = Result.CameraPose;
	DesiredView.Location = CameraPose.GetLocation();
	DesiredView.Rotation = CameraPose.GetRotation();
	DesiredView.FOV = CameraPose.GetEffectiveFieldOfView();
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FCameraSystemEvaluator::DebugDraw(const FCameraSystemDebugDrawParams& Params)
{
	if (RootDebugBlock)
	{
		FCameraDebugBlockDrawParams BlockParams;
		BlockParams.DeltaTime = RootDebugBlock->LastDeltaTime;

		FCameraDebugRenderer Renderer(Params.Canvas);

		RootDebugBlock->DebugDraw(BlockParams, Renderer);
	}
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

