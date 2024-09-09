// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/RootCameraNode.h"
#include "Core/BlendStackCameraRigEvent.h"

#include "DefaultRootCameraNode.generated.h"

class UBlendStackCameraNode;

/**
 * The default implementation of a root camera node.
 */
UCLASS(MinimalAPI, Hidden)
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

class FPersistentBlendStackCameraNodeEvaluator;
class FTransientBlendStackCameraNodeEvaluator;

/**
 * Evaluator for the default root camera node.
 */
class FDefaultRootCameraNodeEvaluator : public FRootCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FDefaultRootCameraNodeEvaluator)

protected:

	// FCameraNodeEvaluator interface.
	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnBuild(const FCameraNodeEvaluatorBuildParams& Params) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

	// FRootCameraNodeEvaluator interface.
	virtual void OnActivateCameraRig(const FActivateCameraRigParams& Params) override;
	virtual void OnDeactivateCameraRig(const FDeactivateCameraRigParams& Params) override;
	virtual void OnBuildSingleCameraRigHierarchy(const FSingleCameraRigHierarchyBuildParams& Params, FCameraNodeEvaluatorHierarchy& OutHierarchy) override;
	virtual void OnRunSingleCameraRig(const FSingleCameraRigEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	template<typename EvaluatorType>
	EvaluatorType* BuildBlendStackEvaluator(const FCameraNodeEvaluatorBuildParams& Params, UBlendStackCameraNode* BlendStackNode);

	void OnBlendStackEvent(const FBlendStackCameraRigEvent& InEvent);

private:

	FPersistentBlendStackCameraNodeEvaluator* BaseLayer;
	FTransientBlendStackCameraNodeEvaluator* MainLayer;
	FPersistentBlendStackCameraNodeEvaluator* GlobalLayer;
	FPersistentBlendStackCameraNodeEvaluator* VisualLayer;
};

}  // namespace UE::Cameras

