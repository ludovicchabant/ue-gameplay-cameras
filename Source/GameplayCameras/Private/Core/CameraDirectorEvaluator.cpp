// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraDirectorEvaluator.h"

#include "Core/CameraDirector.h"

namespace UE::Cameras
{

UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(FCameraDirectorEvaluator)

FCameraDirectorEvaluator::FCameraDirectorEvaluator()
{
}

void FCameraDirectorEvaluator::Run(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult)
{
	OnRun(Params, OutResult);
}

void FCameraDirectorEvaluator::SetPrivateCameraDirector(TObjectPtr<const UCameraDirector> InCameraDirector)
{
	PrivateCameraDirector = InCameraDirector;
}

}  // namespace UE::Cameras

