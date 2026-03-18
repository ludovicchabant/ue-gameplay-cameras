// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraEvaluationService.h"
#include "Containers/Array.h"
#include "Templates/SharedPointerFwd.h"

class UMovieSceneEntitySystemLinker;
struct FCameraAnimationSequenceState;

namespace UE::Cameras
{

class FCameraAnimationSequenceService : public FCameraEvaluationService
{
	UE_DECLARE_CAMERA_EVALUATION_SERVICE(GAMEPLAYCAMERAS_API, FCameraAnimationSequenceService)

public:

	GAMEPLAYCAMERAS_API UMovieSceneEntitySystemLinker* GetLinker();

	GAMEPLAYCAMERAS_API void AddAnimation(TSharedRef<FCameraAnimationSequenceState> InAnimationState);
	GAMEPLAYCAMERAS_API void RemoveAnimation(TSharedRef<FCameraAnimationSequenceState> InAnimationState);

protected:

	// FCameraEvaluationService interface.
	virtual void OnInitialize(const FCameraEvaluationServiceInitializeParams& Params) override;
	virtual void OnPreUpdate(const FCameraEvaluationServiceUpdateParams& Params, FCameraEvaluationServiceUpdateResult& OutResult) override;
	virtual void OnTeardown(const FCameraEvaluationServiceTeardownParams& Params) override;
	virtual void OnAddReferencedObjects(FReferenceCollector& Collector) override;

private:

	void EnsureLinkerCreated();

private:

	FCameraSystemEvaluator* Evaluator = nullptr;

	TObjectPtr<UMovieSceneEntitySystemLinker> Linker;

	TArray<TSharedRef<FCameraAnimationSequenceState>> ActiveAnimationStates;
};

}  // namespace UE::Cameras

