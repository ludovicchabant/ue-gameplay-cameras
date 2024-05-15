// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraDirector.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Templates/SubclassOf.h"

#include "BlueprintCameraDirector.generated.h"

/**
 * Parameter struct for the Blueprint camera director evaluator.
 */
USTRUCT(BlueprintType)
struct FBlueprintCameraDirectorEvaluationParams
{
	GENERATED_BODY()

	/** The elapsed time since the last evaluation. */
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
	float DeltaTime = 0.f;

	/** The owner (if any) of the evaluation context we are running inside of. */
	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	TObjectPtr<UObject> EvaluationContextOwner;
};

/**
 * The evaluation result for the Blueprint camera director evaluator.
 */
USTRUCT(BlueprintType)
struct FBlueprintCameraDirectorEvaluationResult
{
	GENERATED_BODY()

	/** The list of camera rigs that should be active this frame. */
	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	TArray<UCameraRigAsset*> ActiveCameraRigs;
};

/**
 * Base class for a Blueprint camera director evaluator.
 */
UCLASS(MinimalAPI, Blueprintable)
class UBlueprintCameraDirectorEvaluator : public UObject
{
	GENERATED_BODY()

public:
	
	/**
	 * Override this method in Blueprint to execute the custom logic that determines
	 * what camera rig(s) should be active every frame.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Evaluation")
	void RunCameraDirector(
			const FBlueprintCameraDirectorEvaluationParams& Params, 
			FBlueprintCameraDirectorEvaluationResult& OutResult);

	/** Native wrapper for RunCameraDirector. */
	void NativeRunCameraDirector(
			const FBlueprintCameraDirectorEvaluationParams& Params, 
			FBlueprintCameraDirectorEvaluationResult& OutResult);
};

/**
 * A camera director that will instantiate the given Blueprint and run it.
 */
UCLASS(MinimalAPI, EditInlineNew)
class UBlueprintCameraDirector : public UCameraDirector
{
	GENERATED_BODY()

public:

	/** The blueprint class that we should instantiate and run. */
	UPROPERTY(EditAnywhere, Category="Evaluation")
	TSubclassOf<UBlueprintCameraDirectorEvaluator> CameraDirectorEvaluatorClass;

protected:

	// UCameraDirector interface.
	virtual FCameraDirectorEvaluatorPtr OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const override;
};

