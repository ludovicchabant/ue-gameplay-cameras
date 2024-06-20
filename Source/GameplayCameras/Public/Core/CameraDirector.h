// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/Object.h"
#include "Core/CameraDirectorEvaluator.h"

#include "CameraDirector.generated.h"

namespace UE::Cameras { class FCameraBuildLog; }

/**
 * Base class for a camera director.
 */
UCLASS(Abstract, DefaultToInstanced)
class GAMEPLAYCAMERAS_API UCameraDirector : public UObject
{
	GENERATED_BODY()

public:

	using FCameraDirectorEvaluatorBuilder = UE::Cameras::FCameraDirectorEvaluatorBuilder;

	/** Build the evaluator for this director. */
	FCameraDirectorEvaluatorPtr BuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const;

	/** Builds and validates this camera director. */
	void BuildCameraDirector(UE::Cameras::FCameraBuildLog& BuildLog);

protected:

	/** Build the evaluator for this director. */
	virtual FCameraDirectorEvaluatorPtr OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const { return nullptr; }

	/** Builds and validates this camera director. */
	virtual void OnBuildCameraDirector(UE::Cameras::FCameraBuildLog& BuildLog) {}
};

