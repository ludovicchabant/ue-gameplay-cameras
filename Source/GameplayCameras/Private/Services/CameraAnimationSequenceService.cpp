// Copyright Epic Games, Inc. All Rights Reserved.

#include "Services/CameraAnimationSequenceService.h"

//#include "CameraAnimationSequenceState.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNode.h"
#include "EntitySystem/MovieSceneEntitySystemLinker.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "UObject/ObjectMacros.h"

namespace UE::Cameras
{

UE_DEFINE_CAMERA_EVALUATION_SERVICE(FCameraAnimationSequenceService)

void FCameraAnimationSequenceService::OnInitialize(const FCameraEvaluationServiceInitializeParams& Params)
{
	SetEvaluationServiceFlags(ECameraEvaluationServiceFlags::NeedsPreUpdate);

	ensure(Evaluator == nullptr);
	Evaluator = Params.Evaluator;
}

void FCameraAnimationSequenceService::OnPreUpdate(const FCameraEvaluationServiceUpdateParams& Params, FCameraEvaluationServiceUpdateResult& OutResult)
{
	if (Linker)
	{
		for (TSharedRef<FCameraAnimationSequenceState> AnimationState : ActiveAnimationStates)
		{
			//AnimationState->Tick(Params.DeltaTime);
		}

		if (TSharedPtr<FMovieSceneEntitySystemRunner> Runner = Linker->GetRunner())
		{
			Runner->Flush();
		}
	}
}

void FCameraAnimationSequenceService::OnTeardown(const FCameraEvaluationServiceTeardownParams& Params)
{
	ensure(Evaluator != nullptr);
	Evaluator = nullptr;
}

void FCameraAnimationSequenceService::OnAddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(Linker);
}

UMovieSceneEntitySystemLinker* FCameraAnimationSequenceService::GetLinker()
{
	EnsureLinkerCreated();
	return Linker;
}

void FCameraAnimationSequenceService::AddAnimation(TSharedRef<FCameraAnimationSequenceState> InAnimationState)
{
	ActiveAnimationStates.Add(InAnimationState);
}

void FCameraAnimationSequenceService::RemoveAnimation(TSharedRef<FCameraAnimationSequenceState> InAnimationState)
{
	ActiveAnimationStates.Remove(InAnimationState);
}

void FCameraAnimationSequenceService::EnsureLinkerCreated()
{
	using namespace UE::MovieScene;

	if (!Linker)
	{
		UObject* Outer = (Evaluator ? Evaluator->GetOwner() : nullptr);
		Linker = UMovieSceneEntitySystemLinker::FindOrCreateLinker(Outer, EEntitySystemLinkerRole::CameraAnimations);
		ensure(Linker);
	}
}

}  // namespace UE::Cameras

