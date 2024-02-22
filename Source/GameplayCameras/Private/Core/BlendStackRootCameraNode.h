// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"

#include "BlendStackRootCameraNode.generated.h"

class UBlendCameraNode;
class UCameraRigAsset;

/**
 * Root camera node for running a camera rig in a blend stack.
 * This camera node wraps both the camera rig's root node, and the
 * blend node used to blend it.
 */
UCLASS(MinimalAPI)
class UBlendStackRootCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeChildrenView OnGetChildren() override;
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

	/** The blend to use on the camera rig. */
	UPROPERTY()
	TObjectPtr<UBlendCameraNode> Blend;

	/** The root of the instantied camera node tree. */
	UPROPERTY()
	TObjectPtr<UCameraNode> RootNode;
};

namespace UE::Cameras
{

class FBlendCameraNodeEvaluator;

/**
 * Evaluator for the blend stack entry root node.
 */
class FBlendStackRootCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(FBlendStackRootCameraNodeEvaluator)

public:

	FBlendCameraNodeEvaluator* GetBlendEvaluator() const { return BlendEvaluator; }
	FCameraNodeEvaluator* GetRootEvaluator() const { return RootEvaluator; }

protected:

	// FCameraNodeEvaluator interface.
	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	FBlendCameraNodeEvaluator* BlendEvaluator = nullptr;
	FCameraNodeEvaluator* RootEvaluator = nullptr;
};

}  // namespace UE::Cameras

