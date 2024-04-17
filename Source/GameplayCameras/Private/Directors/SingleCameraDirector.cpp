// Copyright Epic Games, Inc. All Rights Reserved.

#include "Directors/SingleCameraDirector.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SingleCameraDirector)

namespace UE::Cameras
{

class FSingleCameraDirectorEvaluator : public FCameraDirectorEvaluator
{
	UE_DECLARE_CAMERA_DIRECTOR_EVALUATOR(GAMEPLAYCAMERAS_API, FSingleCameraDirectorEvaluator)
protected:
	virtual void OnRun(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult) override
	{
		const USingleCameraDirector* SingleDirector = GetCameraDirectorAs<USingleCameraDirector>();
		if (SingleDirector->CameraRig)
		{
			OutResult.Add(Params.OwnerContext, SingleDirector->CameraRig);
		}
	}
};

UE_DEFINE_CAMERA_DIRECTOR_EVALUATOR(FSingleCameraDirectorEvaluator)

}  // namespace UE::Cameras

USingleCameraDirector::USingleCameraDirector(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

FCameraDirectorEvaluatorPtr USingleCameraDirector::OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FSingleCameraDirectorEvaluator>();
}

