// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/Object.h"
#include "Core/CameraDirectorEvaluator.h"

#include "CameraDirector.generated.h"

/**
 * Base class for a camera director.
 */
UCLASS(Abstract, DefaultToInstanced, MinimalAPI)
class UCameraDirector : public UObject
{
	GENERATED_BODY()

public:

	/** Build the evaluator for this director. */
	FCameraDirectorEvaluator* BuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const;

protected:

	/** Build the evaluator for this director. */
	virtual FCameraDirectorEvaluator* OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const { return nullptr; }
};

