// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/RootCameraNode.h"

#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNodeObserver.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RootCameraNode)

namespace UE::Cameras
{

void FRootCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	OwningEvaluator = Params.Evaluator;
}

void FRootCameraNodeEvaluator::ActivateCameraRig(const FActivateCameraRigParams& Params)
{
	OnActivateCameraRig(Params);
}

void FRootCameraNodeEvaluator::RegisterObserver(IRootCameraNodeObserver* Observer)
{
	Observers.Add(Observer);
}

void FRootCameraNodeEvaluator::UnregisterObserver(IRootCameraNodeObserver* Observer)
{
	Observers.Remove(Observer);
}

bool FRootCameraNodeEvaluator::HasObservers() const
{
	return !Observers.IsEmpty();
}

void FRootCameraNodeEvaluator::NotifyObservers(const FRootCameraNodeCameraRigEvent& InEvent) const
{
	if (ensure(OwningEvaluator))
	{
		OwningEvaluator->NotifyRootCameraNodeEvent(InEvent);
	}

	for (IRootCameraNodeObserver* Observer : Observers)
	{
		Observer->OnRootCameraNodeEvent(InEvent);
	}
}

}  // namespace UE::Cameras

