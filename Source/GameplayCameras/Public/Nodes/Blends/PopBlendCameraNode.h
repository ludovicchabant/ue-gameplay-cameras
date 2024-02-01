// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BlendCameraNode.h"

#include "PopBlendCameraNode.generated.h"

/**
 * A blend node that creates a camera cut (i.e. it doesn't blend at all).
 */
UCLASS(MinimalAPI)
class UPopBlendCameraNode : public UBlendCameraNode
{
	GENERATED_BODY()

protected:

	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
};

