// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Nodes/Blends/SimpleBlendCameraNode.h"

#include "SmoothBlendCameraNode.generated.h"

/**
 * The smoothstep type.
 */
UENUM()
enum class ESmoothCameraBlendType
{
	SmoothStep,
	SmootherStep
};

/**
 * A blend camera rig that implements the smoothstep and smoothersteps algorithms.
 */
UCLASS(MinimalAPI)
class USmoothBlendCameraNode : public USimpleFixedTimeBlendCameraNode
{
	GENERATED_BODY()

protected:

	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
	
public:

	/** The type of algorithm to use. */
	UPROPERTY(EditAnywhere, Category=Blending)
	ESmoothCameraBlendType BlendType;
};

