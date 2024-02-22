// Copyright Epic Games, Inc. All Rights Reserved.

#include "Directors/SingleCameraDirector.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SingleCameraDirector)

class FSingleCameraDirectorEvaluator : public FCameraDirectorEvaluator
{
	UE_DECLARE_CAMERA_DIRECTOR_EVALUATOR(FSingleCameraDirectorEvaluator)
protected:
	virtual void OnRun(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult) override
	{
		const USingleCameraDirector* SingleDirector = GetCameraDirectorAs<USingleCameraDirector>();
		if (SingleDirector->CameraRig)
		{
			OutResult.ActiveCameraRigs.Add(SingleDirector->CameraRig);
		}
	}
};

UE_DEFINE_CAMERA_DIRECTOR_EVALUATOR(FSingleCameraDirectorEvaluator)

USingleCameraDirector::USingleCameraDirector(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

FCameraDirectorEvaluator* USingleCameraDirector::OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const
{
	return Builder.BuildEvaluator<FSingleCameraDirectorEvaluator>();
}

