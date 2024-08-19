// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraDirectorEvaluator.h"

#include "Core/CameraDirector.h"
#include "Core/CameraEvaluationContext.h"

namespace UE::Cameras
{

void FCameraDirectorEvaluatorStorage::DestroyEvaluator()
{
	Evaluator.Reset();
}

UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(FCameraDirectorEvaluator)

FCameraDirectorEvaluator::FCameraDirectorEvaluator()
{
}

void FCameraDirectorEvaluator::SetPrivateCameraDirector(const UCameraDirector* InCameraDirector)
{
	PrivateCameraDirector = InCameraDirector;
}

void FCameraDirectorEvaluator::Initialize(const FCameraDirectorInitializeParams& Params)
{
	WeakOwnerContext = Params.OwnerContext;

	OnInitialize(Params);
}

void FCameraDirectorEvaluator::Activate(const FCameraDirectorActivateParams& Params)
{
	WeakOwnerContext = Params.OwnerContext;

	OnActivate(Params);
}

void FCameraDirectorEvaluator::Deactivate(const FCameraDirectorDeactivateParams& Params)
{
	OnDeactivate(Params);

	WeakOwnerContext.Reset();
}

void FCameraDirectorEvaluator::Run(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult)
{
	OnRun(Params, OutResult);
}

bool FCameraDirectorEvaluator::AddChildEvaluationContext(TSharedRef<FCameraEvaluationContext> InContext)
{
	TSharedPtr<FCameraEvaluationContext> OwnerContext = WeakOwnerContext.Pin();
	if (!ensureMsgf(OwnerContext, TEXT("Can't add child evaluation context when the parent/owner context is invalid!")))
	{
		return false;
	}

	FChildContextManulationParams Params;
	Params.ParentContext = OwnerContext;
	Params.ChildContext = InContext.ToSharedPtr();
	FChildContextManulationResult Result;
	OnAddChildEvaluationContext(Params, Result);

	bool bReturn = false;
	bool bRegisterAndActivateChildContext = false;
	switch (Result.Result)
	{
		case EChildContextManipulationResult::Failure:
		default:
			// Nothing to do.
			break;
		case EChildContextManipulationResult::Success:
			// Our director evaluator accepted the child context.
			bRegisterAndActivateChildContext = true;
			bReturn = true;
			break;
		case EChildContextManipulationResult::ChildContextSuccess:
			// A sub-director of our director accepted the child context, so it already
			// activate it and we don't need to do it ourselves.
			bReturn = true;
			break;
	}
	if (bRegisterAndActivateChildContext)
	{
		OwnerContext->RegisterChildContext(InContext);

		FCameraEvaluationContextActivateParams ActivateParams;
		InContext->Activate(ActivateParams);
	}
	return bReturn;
}

bool FCameraDirectorEvaluator::RemoveChildEvaluationContext(TSharedRef<FCameraEvaluationContext> InContext)
{
	TSharedPtr<FCameraEvaluationContext> OwnerContext = WeakOwnerContext.Pin();
	if (!ensureMsgf(OwnerContext, TEXT("Can't remove child evaluation context when the parent/owner context is invalid!")))
	{
		return false;
	}

	FChildContextManulationParams Params;
	Params.ParentContext = OwnerContext;
	Params.ChildContext = InContext;
	FChildContextManulationResult Result;
	OnRemoveChildEvaluationContext(Params, Result);

	bool bReturn = false;
	bool bUnregisterAndDeactivateChildContext = false;
	switch (Result.Result)
	{
		case EChildContextManipulationResult::Failure:
		default:
			break;
		case EChildContextManipulationResult::Success:
			bUnregisterAndDeactivateChildContext = true;
			bReturn = true;
			break;
		case EChildContextManipulationResult::ChildContextSuccess:
			bReturn = true;
			break;
	}
	if (bUnregisterAndDeactivateChildContext)
	{
		OwnerContext->UnregisterChildContext(InContext);

		FCameraEvaluationContextDeactivateParams DeactivateParams;
		InContext->Deactivate(DeactivateParams);
	}
	return bReturn;
}

void FCameraDirectorEvaluator::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(PrivateCameraDirector);

	OnAddReferencedObjects(Collector);
}

}  // namespace UE::Cameras

