// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraEvaluationContext.h"

#include "Core/CameraAsset.h"
#include "Core/CameraDirector.h"
#include "GameFramework/PlayerController.h"

namespace UE::Cameras
{

UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(FCameraEvaluationContext)

FCameraEvaluationContext::FCameraEvaluationContext()
{
}

FCameraEvaluationContext::FCameraEvaluationContext(const FCameraEvaluationContextInitializeParams& Params)
{
	Initialize(Params);
}

void FCameraEvaluationContext::Initialize(const FCameraEvaluationContextInitializeParams& Params)
{
	if (!ensureMsgf(!bInitialized, TEXT("This evaluation context has already been initialized!")))
	{
		return;
	}

	WeakOwner = Params.Owner;
	CameraAsset = Params.CameraAsset;
	WeakPlayerController = Params.PlayerController;

	bInitialized = true;
}

FCameraEvaluationContext::~FCameraEvaluationContext()
{
	// Camera director evaluator usually gets destroyed here since the storage object generally
	// holds the only shared pointer to it.
}

UWorld* FCameraEvaluationContext::GetWorld() const
{
	if (UObject* Owner = GetOwner())
	{
		return Owner->GetWorld();
	}
	return nullptr;
}

void FCameraEvaluationContext::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(CameraAsset);

	if (DirectorEvaluator)
	{
		DirectorEvaluator->AddReferencedObjects(Collector);
	}

	for (TSharedPtr<FCameraEvaluationContext> ChildContext : ChildrenContexts)
	{
		ChildContext->AddReferencedObjects(Collector);
	}
}

void FCameraEvaluationContext::AutoCreateDirectorEvaluator()
{
	if (DirectorEvaluator == nullptr)
	{
		if (!CameraAsset)
		{
			UE_LOG(LogCameraSystem, Error, TEXT("Activating an evaluation context without a camera!"));
			return;
		}
		if (!CameraAsset->GetCameraDirector())
		{
			UE_LOG(LogCameraSystem, Error, TEXT("Activating an evaluation context without a camera director!"));
			return;
		}

		const UCameraDirector* CameraDirector = CameraAsset->GetCameraDirector();
		FCameraDirectorEvaluatorBuilder DirectorBuilder(DirectorEvaluatorStorage);
		DirectorEvaluator = CameraDirector->BuildEvaluator(DirectorBuilder);

		FCameraDirectorInitializeParams InitParams;
		InitParams.OwnerContext = SharedThis(this);
		DirectorEvaluator->Initialize(InitParams);
	}
}

void FCameraEvaluationContext::Activate(const FCameraEvaluationContextActivateParams& Params)
{
	if (!ensureMsgf(bInitialized, TEXT("This evaluation context needs to be initialized!")))
	{
		return;
	}
	if (!ensureMsgf(!bActivated, TEXT("This evaluation context has already been activated!")))
	{
		return;
	}

	CameraSystemEvaluator = Params.Evaluator;

	OnActivate(Params);

	AutoCreateDirectorEvaluator();

	if (ensure(DirectorEvaluator))
	{
		FCameraDirectorActivateParams DirectorParams;
		DirectorParams.Evaluator = Params.Evaluator;
		DirectorParams.OwnerContext = SharedThis(this);
		DirectorEvaluator->Activate(DirectorParams);
	}

	bActivated = true;
}

void FCameraEvaluationContext::Deactivate(const FCameraEvaluationContextDeactivateParams& Params)
{
	if (!ensureMsgf(bActivated, TEXT("This evaluation context has not been activated!")))
	{
		return;
	}

	if (ensure(DirectorEvaluator))
	{
		FCameraDirectorDeactivateParams DirectorParams;
		DirectorParams.OwnerContext = SharedThis(this);
		DirectorEvaluator->Deactivate(DirectorParams);
	}

	// Don't destroy the camera director evaluator, it could still be useful. We only destroy it
	// along with this context.

	OnDeactivate(Params);

	CameraSystemEvaluator = nullptr;

	bActivated = false;
}

bool FCameraEvaluationContext::RegisterChildContext(TSharedRef<FCameraEvaluationContext> ChildContext)
{
	if (!ensureMsgf(ChildContext->WeakParent == nullptr, TEXT("The given evaluation context already has a parent!")))
	{
		return false;
	}

	ChildContext->WeakParent = SharedThis(this);
	ChildrenContexts.Add(ChildContext);
	return true;
}

bool FCameraEvaluationContext::UnregisterChildContext(TSharedRef<FCameraEvaluationContext> ChildContext)
{
	if (!ensureMsgf(ChildContext->WeakParent == SharedThis(this), TEXT("The given evaluation context isn't our child!")))
	{
		return false;
	}

	ChildContext->WeakParent = nullptr;
	const int32 NumRemoved = ChildrenContexts.Remove(ChildContext);
	ensureMsgf(NumRemoved == 1, TEXT("The given evaluation context wasn't in our list of children!"));
	return true;
}

}  // namespace UE::Cameras

