// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraRigJoints.h"
#include "Core/CameraVariableAssets.h"
#include "GameFramework/BlueprintCameraPose.h"
#include "GameFramework/BlueprintCameraVariableTable.h"
#include "Templates/SubclassOf.h"

#include "BlueprintCameraNode.generated.h"

struct FBlueprintCameraPose;

namespace UE::Cameras
{
	class FCameraVariableTable;
	struct FCameraNodeEvaluationParams;
	struct FCameraNodeEvaluationResult;
};

/**
 * The base class for Blueprint camera node evaluators.
 */
UCLASS(MinimalAPI, Blueprintable, Abstract)
class UBlueprintCameraNodeEvaluator : public UObject
{
	GENERATED_BODY()

public:

	/** The main execution callback for the camera node. Call SetCameraPose to affect the result. */
	UFUNCTION(BlueprintImplementableEvent, Category="Evaluation")
	void TickCameraNode(float DeltaTime);

public:

	using FCameraNodeEvaluationParams = UE::Cameras::FCameraNodeEvaluationParams;
	using FCameraNodeEvaluationResult = UE::Cameras::FCameraNodeEvaluationResult;

	/** Runs this camera node. */
	void NativeRunCameraNode(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);

public:

	/**
	 * A utility function that tries to find if an actor owns the evaluation context.
	 * Handles the situation where the evaluation context is an actor component (like a
	 * UGameplayCameraComponent) or an actor itself.
	 */
	UFUNCTION(BlueprintPure, Category="Evaluation", meta=(DeterminesOutputType="ActorClass"))
	AActor* FindEvaluationContextOwnerActor(TSubclassOf<AActor> ActorClass) const;

protected:

	/** Whether this is the first frame of this camera node's lifetime. */
	UPROPERTY(BlueprintReadOnly, Category="Evaluation")
	bool bIsFirstFrame = false;

	/** The owner object of this camera node's evaluation context. */
	UPROPERTY(BlueprintReadOnly, Category="Evaluation")
	TObjectPtr<UObject> EvaluationContextOwner;

	/** The input/output camera pose for this frame. */
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
	FBlueprintCameraPose CameraPose;

	/** The input/output camera variable table for this frame. */
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
	FBlueprintCameraVariableTable VariableTable;

private:

	TSharedPtr<const UE::Cameras::FCameraEvaluationContext> CurrentContext;

	FCameraNodeEvaluationResult* CurrentResult = nullptr;
};

/**
 * A camera node that runs arbitrary Blueprint logic.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Common,Transform"))
class UBlueprintCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual void OnBuild(FCameraRigBuildContext& BuildContext) override;
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

	/** The camera node evaluator class to instantiate and run. */
	UPROPERTY(EditAnywhere, Category=Common)
	TSubclassOf<UBlueprintCameraNodeEvaluator> CameraNodeEvaluatorClass;
};

