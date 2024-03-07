// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/RootCameraNode.h"

#include "DefaultRootCameraNode.generated.h"

class UBlendStackCameraNode;

/**
 * The default implementation of a root camera node.
 */
UCLASS(MinimalAPI)
class UDefaultRootCameraNode : public URootCameraNode
{
	GENERATED_BODY()

public:

	UDefaultRootCameraNode(const FObjectInitializer& ObjectInit);

protected:

	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

	UPROPERTY(Instanced)
	TObjectPtr<UBlendStackCameraNode> BaseLayer;

	UPROPERTY(Instanced)
	TObjectPtr<UBlendStackCameraNode> MainLayer;

	UPROPERTY(Instanced)
	TObjectPtr<UBlendStackCameraNode> GlobalLayer;

	UPROPERTY(Instanced)
	TObjectPtr<UBlendStackCameraNode> VisualLayer;
};

namespace UE::Cameras
{

class FBlendStackCameraNodeEvaluator;

/**
 * Evaluator for the default root camera node.
 */
class FDefaultRootCameraNodeEvaluator : public FRootCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(FDefaultRootCameraNodeEvaluator)

protected:

	// FRootCameraNodeEvaluator interface.
	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnBuild(const FCameraNodeEvaluatorBuildParams& Params) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnActivateCameraRig(const FActivateCameraRigParams& Params) override;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	FBlendStackCameraNodeEvaluator* BaseLayer;
	FBlendStackCameraNodeEvaluator* MainLayer;
	FBlendStackCameraNodeEvaluator* GlobalLayer;
	FBlendStackCameraNodeEvaluator* VisualLayer;
};

}  // namespace UE::Cameras

