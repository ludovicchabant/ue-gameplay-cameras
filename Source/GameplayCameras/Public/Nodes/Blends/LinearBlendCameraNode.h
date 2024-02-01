// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Nodes/Blends/SimpleBlendCameraNode.h"

#include "LinearBlendCameraNode.generated.h"

/**
 * Linear blend node.
 */
UCLASS(MinimalAPI)
class ULinearBlendCameraNode : public USimpleFixedTimeBlendCameraNode
{
	GENERATED_BODY()

protected:

	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
};

