// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraDirector.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraDirector)

FCameraDirectorEvaluator* UCameraDirector::BuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const
{
	FCameraDirectorEvaluator* NewEvaluator = OnBuildEvaluator(Builder);
	NewEvaluator->SetPrivateCameraDirector(this);
	return NewEvaluator;
}

