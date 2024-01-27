// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"

#include "BlendStackRootCameraNode.generated.h"

class UBlendCameraNode;
class UCameraMode;

/**
 * Root camera node for running a camera mode in a blend stack.
 * This camera node wraps both the camera mode's root node, and the
 * blend node used to blend it.
 */
UCLASS(MinimalAPI)
class UBlendStackRootCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	virtual FCameraNodeChildrenView OnGetChildren() override;
	virtual void OnRun(const FCameraNodeRunParams& Params, FCameraNodeRunResult& OutResult) override;

public:

	/** The blend to use on the camera mode. */
	UPROPERTY()
	TObjectPtr<UBlendCameraNode> Blend;

	/** The root of the instantied camera node tree. */
	UPROPERTY()
	TObjectPtr<UCameraNode> RootNode;
};

