// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"

#include "BlendCameraNode.generated.h"

/**
 * Base class for blend camera nodes.
 */
UCLASS(MinimalAPI, Hidden, meta=(ObjectTreeGraphCategory="Camera Blends"))
class UBlendCameraNode : public UCameraNode
{
	GENERATED_BODY()
};

namespace UE::Cameras
{

/**
 * Parameter struct for blending camera node tree results.
 */
struct FCameraNodeBlendParams
{
	FCameraNodeBlendParams(
			const FCameraNodeEvaluationParams& InChildParams,
			const FCameraNodeEvaluationResult& InChildResult)
		: ChildParams(InChildParams)
		, ChildResult(InChildResult)
	{}

	/** The parameters that the blend received during the evaluation. */
	const FCameraNodeEvaluationParams& ChildParams;
	/** The result that the blend should apply over another result. */
	const FCameraNodeEvaluationResult& ChildResult;
};

/**
 * Result struct for blending camera node tree results.
 */
struct FCameraNodeBlendResult
{
	FCameraNodeBlendResult(FCameraNodeEvaluationResult& InBlendedResult)
		: BlendedResult(InBlendedResult)
	{}

	/** The result upon which another result should be blended. */
	FCameraNodeEvaluationResult& BlendedResult;

	/** Whether the blend has reached 100%. */
	bool bIsBlendFull = false;

	/** Whether the blend is finished. */
	bool bIsBlendFinished = false;
};

/**
 * Base evaluator class for blend camera nodes.
 */
class FBlendCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(FBlendCameraNodeEvaluator)

public:

	/** Blend the result of a camera node tree over another result. */
	void BlendResults(const FCameraNodeBlendParams& Params, FCameraNodeBlendResult& OutResult);

protected:

	/** Blend the result of a camera node tree over another result. */
	virtual void OnBlendResults(const FCameraNodeBlendParams& Params, FCameraNodeBlendResult& OutResult) {}
};

}  // namespace UE::Cameras

// Macros for declaring and defining new blend node evaluators. They are the same
// as the base ones for generic node evaluators, but the first one prevents you
// from having to specify FBlendCameraNodeEvaluator as the base class, which saves
// a little bit of typing.
//
#define UE_DECLARE_BLEND_CAMERA_NODE_EVALUATOR(ClassName)\
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(ClassName, ::UE::Cameras::FBlendCameraNodeEvaluator)

#define UE_DECLARE_BLEND_CAMERA_NODE_EVALUATOR_EX(ClassName, BaseClassName)\
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(ClassName, BaseClassName)

#define UE_DEFINE_BLEND_CAMERA_NODE_EVALUATOR(ClassName)\
	UE_DEFINE_CAMERA_NODE_EVALUATOR(ClassName)

