// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraDirector.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraDirector)

FCameraDirectorEvaluatorPtr UCameraDirector::BuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	FCameraDirectorEvaluator* NewEvaluator = OnBuildEvaluator(Builder);
	NewEvaluator->SetPrivateCameraDirector(this);
	return NewEvaluator;
}

void UCameraDirector::BuildCameraDirector(UE::Cameras::FCameraBuildLog& BuildLog)
{
	OnBuildCameraDirector(BuildLog);
}

#if WITH_EDITOR

void UCameraDirector::FactoryCreateAsset(const FCameraDirectorFactoryCreateParams& InParams)
{
	OnFactoryCreateAsset(InParams);
}

#endif

