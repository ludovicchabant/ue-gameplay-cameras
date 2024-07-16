// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BlendCameraNode.h"

#include "SimpleBlendCameraNode.generated.h"

/**
 * Base class for a blend camera node that uses a simple scalar factor.
 */
UCLASS(MinimalAPI, Abstract)
class USimpleBlendCameraNode : public UBlendCameraNode
{
	GENERATED_BODY()
};

/**
 * Base class for a blend camera node that uses a simple scalar factor over a fixed time.
 */
UCLASS(MinimalAPI, Abstract)
class USimpleFixedTimeBlendCameraNode : public USimpleBlendCameraNode
{
	GENERATED_BODY()

public:

	/** Duration of the blend. */
	UPROPERTY(EditAnywhere, Category=Blending)
	float BlendTime = 1.f;
};

namespace UE::Cameras
{

/**
 * Result structure for defining a simple scalar-factor-based blend.
 */
struct FSimpleBlendCameraNodeEvaluationResult
{
	float BlendFactor = 0.f;
};

class FSimpleBlendCameraNodeEvaluator : public FBlendCameraNodeEvaluator
{
	UE_DECLARE_BLEND_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FSimpleBlendCameraNodeEvaluator)

public:

	/** Gets the last evaluated blend factor. */
	float GetBlendFactor() const { return BlendFactor; }

protected:

	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnBlendParameters(const FCameraNodePreBlendParams& Params, FCameraNodePreBlendResult& OutResult) override;
	virtual void OnBlendResults(const FCameraNodeBlendParams& Params, FCameraNodeBlendResult& OutResult) override;
	virtual void OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar) override;

	virtual void OnComputeBlendFactor(const FCameraNodeEvaluationParams& Params, FSimpleBlendCameraNodeEvaluationResult& OutResult) {}

	void SetBlendFinished() { bIsBlendFinished = true; }

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	float BlendFactor = 0.f;
	bool bIsBlendFinished = false;
};

class FSimpleFixedTimeBlendCameraNodeEvaluator : public FSimpleBlendCameraNodeEvaluator
{
	UE_DECLARE_BLEND_CAMERA_NODE_EVALUATOR_EX(GAMEPLAYCAMERAS_API, FSimpleFixedTimeBlendCameraNodeEvaluator, FSimpleBlendCameraNodeEvaluator)

protected:

	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

	float GetTimeFactor() const;

private:

	float CurrentTime = 0.f;
};

}  // namespace UE::Cameras

