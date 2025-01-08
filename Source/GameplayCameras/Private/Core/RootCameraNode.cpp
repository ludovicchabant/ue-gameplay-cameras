// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/RootCameraNode.h"

#include "Core/CameraEvaluationService.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNodeCameraRigEvent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RootCameraNode)

namespace UE::Cameras
{

void FRootCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	OwningEvaluator = Params.Evaluator;
}

void FRootCameraNodeEvaluator::ActivateCameraRig(const FActivateCameraRigParams& Params)
{
	OnActivateCameraRig(Params);
}

void FRootCameraNodeEvaluator::DeactivateCameraRig(const FDeactivateCameraRigParams& Params)
{
	OnDeactivateCameraRig(Params);
}

void FRootCameraNodeEvaluator::GetActiveCameraRigInfo(FCameraRigEvaluationInfo& OutCameraRigInfo) const
{
	OnGetActiveCameraRigInfo(OutCameraRigInfo);
}

void FRootCameraNodeEvaluator::BuildSingleCameraRigHierarchy(const FSingleCameraRigHierarchyBuildParams& Params, FCameraNodeEvaluatorHierarchy& OutHierarchy)
{
	OnBuildSingleCameraRigHierarchy(Params, OutHierarchy);
}

void FRootCameraNodeEvaluator::RunSingleCameraRig(const FSingleCameraRigEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	// Before we do the actual evaluation we need to auto-reset any camera variable 
	// that needs auto-resetting. Otherwise, we might end up with an update result 
	// that isn't representative of what would happen normally.
	// TODO: we might have to reset variables on the context's initial result too?
	OutResult.VariableTable.AutoResetValues();

	OnRunSingleCameraRig(Params, OutResult);
}

void FRootCameraNodeEvaluator::BroadcastCameraRigEvent(const FRootCameraNodeCameraRigEvent& InEvent) const
{
	if (ensure(OwningEvaluator))
	{
		OwningEvaluator->NotifyRootCameraNodeEvent(InEvent);
	}

	OnCameraRigEventDelegate.Broadcast(InEvent);
}

}  // namespace UE::Cameras

