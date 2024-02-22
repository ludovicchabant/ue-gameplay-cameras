// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraSystemEvaluator.h"

#include "Camera/CameraTypes.h"
#include "Core/CameraAsset.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraRigAsset.h"
#include "Core/DefaultRootCameraNode.h"
#include "GameplayCameras.h"
#include "IGameplayCamerasModule.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraSystemEvaluator)

DECLARE_CYCLE_STAT(TEXT("Camera System Eval"), CameraSystemEval_Total, STATGROUP_CameraSystem);

UCameraSystemEvaluator::UCameraSystemEvaluator(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	RootNode = ObjectInit.CreateDefaultSubobject<UDefaultRootCameraNode>(this, TEXT("RootNode"));

	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		ContextStack.Initialize(this);

		FCameraNodeEvaluatorTreeBuilderParams BuildParams;
		BuildParams.Evaluator = this;
		BuildParams.RootCameraNode = RootNode;
		RootEvaluator = static_cast<FRootCameraNodeEvaluator*>(RootEvaluatorStorage.BuildEvaluatorTree(BuildParams));
	}
}

void UCameraSystemEvaluator::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UCameraSystemEvaluator* TypedThis = CastChecked<UCameraSystemEvaluator>(InThis);
	TypedThis->ContextStack.AddReferencedObjects(Collector);
	if (TypedThis->RootEvaluator)
	{
		TypedThis->RootEvaluator->AddReferencedObjects(Collector);
	}
}

void UCameraSystemEvaluator::PushEvaluationContext(UCameraEvaluationContext* EvaluationContext)
{
	ContextStack.PushContext(EvaluationContext);
}

void UCameraSystemEvaluator::RemoveEvaluationContext(UCameraEvaluationContext* EvaluationContext)
{
	ContextStack.RemoveContext(EvaluationContext);
}

void UCameraSystemEvaluator::PopEvaluationContext()
{
	ContextStack.PopContext();
}

void UCameraSystemEvaluator::Update(const FCameraSystemEvaluationUpdateParams& Params)
{
	SCOPE_CYCLE_COUNTER(CameraSystemEval_Total);

	// Get the active evaluation context.
	FCameraEvaluationContextInfo ActiveContextInfo = ContextStack.GetActiveContext();
	if (UNLIKELY(!ActiveContextInfo.IsValid()))
	{
		Result.bIsValid = false;
		return;
	}

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
			CameraRigParams.Evaluator = this;
			CameraRigParams.EvaluationContext = ActiveContextInfo.EvaluationContext;
			CameraRigParams.CameraRig = DirectorResult.ActiveCameraRigs[0];
			RootEvaluator->ActivateCameraRig(CameraRigParams);
		}
	}

	// Run the root camera node.
	FCameraNodeEvaluationParams NodeParams;
	NodeParams.Evaluator = this;
	NodeParams.DeltaTime = Params.DeltaTime;

	RootNodeResult.Reset();

	RootEvaluator->Run(NodeParams, RootNodeResult);

	Result.CameraPose = RootNodeResult.CameraPose;
	Result.bIsCameraCut = RootNodeResult.bIsCameraCut;
	Result.bIsValid = true;
}

void UCameraSystemEvaluator::GetEvaluatedCameraView(FMinimalViewInfo& DesiredView)
{
	const FCameraPose& CameraPose = Result.CameraPose;
	DesiredView.Location = CameraPose.GetLocation();
	DesiredView.Rotation = CameraPose.GetRotation();
	DesiredView.FOV = CameraPose.GetEffectiveFieldOfView();
}

