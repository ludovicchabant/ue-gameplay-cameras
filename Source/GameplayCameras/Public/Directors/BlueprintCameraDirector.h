// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraDirector.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Templates/SubclassOf.h"

#include "BlueprintCameraDirector.generated.h"

class UCameraRigProxyAsset;
class UCameraRigProxyTable;

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
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
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
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
	TArray<TObjectPtr<UCameraRigProxyAsset>> ActiveCameraRigProxies;

	/** The list of camera rigs that should be active this frame. */
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
	TArray<TObjectPtr<UCameraRigAsset>> ActiveCameraRigs;
};

/**
 * Base class for a Blueprint camera director evaluator.
 */
UCLASS(MinimalAPI, Blueprintable, Abstract)
class UBlueprintCameraDirectorEvaluator : public UObject
{
	GENERATED_BODY()

public:
	
	/**
	 * Override this method in Blueprint to execute the custom logic that determines
	 * what camera rig(s) should be active every frame.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Evaluation")
	void RunCameraDirector(const FBlueprintCameraDirectorEvaluationParams& Params);

	/** Specifies a camera rig to be active this frame. */
	UFUNCTION(BlueprintCallable, Category="Evaluation")
	void ActivateCameraRig(UPARAM(meta=(UseCameraRigPicker=true)) UCameraRigAsset* CameraRig);

	/**
	 * Specifies a camera rig to be active this frame, via a proxy which is later resolved
	 * via the proxy table of the Blueprint camera director.
	 */
	UFUNCTION(BlueprintCallable, Category="Evaluation")
	void ActivateCameraRigViaProxy(UCameraRigProxyAsset* CameraRigProxy);

	/** Native wrapper for RunCameraDirector. */
	void NativeRunCameraDirector(
			const FBlueprintCameraDirectorEvaluationParams& Params,
			FBlueprintCameraDirectorEvaluationResult& OutResult);

protected:

	/** The current camera director evaluation result. */
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
	FBlueprintCameraDirectorEvaluationResult CurrentResult;
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

	/** 
	 * The table that maps camera rig proxies (used in the evaluator Blueprint graph)
	 * to actual camera rigs.
	 */
	UPROPERTY(EditAnywhere, Instanced, Category="Evaluation")
	TObjectPtr<UCameraRigProxyTable> CameraRigProxyTable;

protected:

	// UCameraDirector interface.
	virtual FCameraDirectorEvaluatorPtr OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const override;
	virtual void OnBuildCameraDirector(UE::Cameras::FCameraBuildLog& BuildLog) override;
#if WITH_EDITOR
	virtual void OnFactoryCreateAsset(const FCameraDirectorFactoryCreateParams& InParams) override;
#endif
};

