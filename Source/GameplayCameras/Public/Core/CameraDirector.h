// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/Object.h"
#include "Core/CameraDirectorEvaluator.h"

#include "CameraDirector.generated.h"

namespace UE::Cameras { class FCameraBuildLog; }

#if WITH_EDITOR

/**
 * Parameter struct passed by an asset factory when a new camera asset is created.
 * This lets a camera director setup data before the editor opens.
 */
struct FCameraDirectorFactoryCreateParams
{
};

#endif

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

#if WITH_EDITOR
	/** Called by the asset factories to setup new data before the editor opens. */
	void FactoryCreateAsset(const FCameraDirectorFactoryCreateParams& InParams);
#endif

protected:

	/** Build the evaluator for this director. */
	virtual FCameraDirectorEvaluatorPtr OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const { return nullptr; }

	/** Builds and validates this camera director. */
	virtual void OnBuildCameraDirector(UE::Cameras::FCameraBuildLog& BuildLog) {}

#if WITH_EDITOR
	/** Called by the asset factories to setup new data before the editor opens. */
	virtual void OnFactoryCreateAsset(const FCameraDirectorFactoryCreateParams& InParams) {}
#endif
};

