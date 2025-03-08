// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/CameraTypes.h"
#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"

#include "ShakeCameraNode.generated.h"

/**
 * Base class for shake camera nodes.
 */
UCLASS(MinimalAPI, Abstract, meta=(CameraNodeCategories="Shakes"))
class UShakeCameraNode : public UCameraNode
{
	GENERATED_BODY()
};

namespace UE::Cameras
{

/**
 * Parameters for applying a shake to a camera result.
 */
struct FCameraNodeShakeParams
{
	FCameraNodeShakeParams(const FCameraNodeEvaluationParams& InChildParams)
		: ChildParams(InChildParams) 
	{}

	/** The parameters that the shake received during the evaluation. */
	const FCameraNodeEvaluationParams& ChildParams;
	/** The intensity to use for the camera shake. */
	float ShakeScale = 1.f;
	/** The play space to modify the result by */
	ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal;
	/** The custom space to use for the shake. Only used when PlaySpace is UserDefined. */
	FMatrix UserPlaySpaceMatrix;
};

/**
 * Result structure for applying a shake to a camera result.
 */
struct FCameraNodeShakeResult
{
	FCameraNodeShakeResult(FCameraNodeEvaluationResult& InShakenResult)
		: ShakenResult(InShakenResult)
	{}

	/** The result that should be shaken. */
	FCameraNodeEvaluationResult& ShakenResult;

	/** The time left in this shake, if applicable. Negative values indicate an infinite shake. */
	float ShakeTimeLeft = 0.f;
};

/**
 * Parameters for restarting a running camera shake.
 */
struct FCameraNodeShakeRestartParams
{
};

class FShakeCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FShakeCameraNodeEvaluator)

public:

	/** Applies the shake to the given result. */
	GAMEPLAYCAMERAS_API void ShakeResult(const FCameraNodeShakeParams& Params, FCameraNodeShakeResult& OutResult);

	/** Restart a running camera shake. */
	GAMEPLAYCAMERAS_API void RestartShake(const FCameraNodeShakeRestartParams& Params);

protected:

	/** Applies the shake to the given result. */
	virtual void OnShakeResult(const FCameraNodeShakeParams& Params, FCameraNodeShakeResult& OutResult) {}

	/** Called when the intensity of the shake changes, but before ShakeScale is set. */
	virtual void OnSetShakeScale(float InShakeScale) {}

	/** Restart a running camera shake. */
	virtual void OnRestartShake(const FCameraNodeShakeRestartParams& Params) {}
};

}  // namespace UE::Cameras

// Macros for declaring and defining new shake node evaluators. They are the same
// as the base ones for generic node evaluators, but the first one prevents you
// from having to specify FShakeCameraNodeEvaluator as the base class, which saves
// a little bit of typing.
//
#define UE_DECLARE_SHAKE_CAMERA_NODE_EVALUATOR(ApiDeclSpec, ClassName)\
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(ApiDeclSpec, ClassName, ::UE::Cameras::FShakeCameraNodeEvaluator)

#define UE_DECLARE_SHAKE_CAMERA_NODE_EVALUATOR_EX(ApiDeclSpec, ClassName, BaseClassName)\
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(ApiDeclSpec, ClassName, BaseClassName)

#define UE_DEFINE_SHAKE_CAMERA_NODE_EVALUATOR(ClassName)\
	UE_DEFINE_CAMERA_NODE_EVALUATOR(ClassName)

