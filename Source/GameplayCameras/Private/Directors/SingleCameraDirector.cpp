// Copyright Epic Games, Inc. All Rights Reserved.

#include "Directors/SingleCameraDirector.h"

#include "Core/CameraAsset.h"
#include "Core/CameraBuildLog.h"
#include "Logging/TokenizedMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SingleCameraDirector)

#define LOCTEXT_NAMESPACE "SingleCameraDirector"

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

void USingleCameraDirector::OnBuildCameraDirector(UE::Cameras::FCameraBuildLog& BuildLog)
{
	if (!CameraRig)
	{
		BuildLog.AddMessage(EMessageSeverity::Error, this, LOCTEXT("MissingCameraRig", "No camera rig is set."));
	}
}

#if WITH_EDITOR

void USingleCameraDirector::OnFactoryCreateAsset(const FCameraDirectorFactoryCreateParams& InParams)
{
	// Automatically set ourselves to use the first camera rig available.
	if (UCameraAsset* OuterCameraAsset = GetTypedOuter<UCameraAsset>())
	{
		if (OuterCameraAsset->GetCameraRigs().Num() > 0)
		{
			CameraRig = OuterCameraAsset->GetCameraRigs()[0];
		}
	}
}

#endif

#undef LOCTEXT_NAMESPACE

