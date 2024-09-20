// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraDirector.h"
#include "Core/CameraDirectorEvaluator.h"
#include "GameFramework/BlueprintCameraPose.h"
#include "GameFramework/BlueprintCameraVariableTable.h"
#include "Templates/SubclassOf.h"

#include "BlueprintCameraDirector.generated.h"

class UCameraRigAsset;
class UCameraRigProxyAsset;
class UCameraRigProxyTable;
enum class ECameraRigLayer : uint8;

namespace UE::Cameras
{

class FAutoResetCameraVariableService;

/** Information about a persitent camera rig to be activated or deactivated. */
struct FBlueprintPersistentCameraRigInfo
{
	UCameraRigAsset* CameraRig;
	ECameraRigLayer Layer;
};

/**
 * The evaluation result for the Blueprint camera director evaluator.
 */
struct FBlueprintCameraDirectorEvaluationResult
{
	/** The list of camera rigs that should be active this frame. */
	TArray<UCameraRigProxyAsset*> ActiveCameraRigProxies;

	/** The list of camera rigs that should be active this frame. */
	TArray<UCameraRigAsset*> ActiveCameraRigs;

	/** The list of persistent camera rigs to activate. */
	TArray<FBlueprintPersistentCameraRigInfo> ActivePersistentCameraRigs;

	/** The list of persistent camera rigs to deactivate. */
	TArray<FBlueprintPersistentCameraRigInfo> InactivePersistentCameraRigs;

	/** Reset this result for a new evaluation. */
	void Reset();
};

}  // namespace UE::Cameras

/**
 * Parameter struct for activating the Blueprint camera director evaluator.
 */
USTRUCT(BlueprintType)
struct FBlueprintCameraDirectorActivateParams
{
	GENERATED_BODY()

	/** The owner (if any) of the evaluation context we are running inside of. */
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
	TObjectPtr<UObject> EvaluationContextOwner;
};

/**
 * Parameter struct for deactivating the Blueprint camera director evaluator.
 */
USTRUCT(BlueprintType)
struct FBlueprintCameraDirectorDeactivateParams
{
	GENERATED_BODY()

	/** The owner (if any) of the evaluation context we were running inside of. */
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
	TObjectPtr<UObject> EvaluationContextOwner;
};

/**
 * Parameter struct for running the Blueprint camera director evaluator.
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
 * Base class for a Blueprint camera director evaluator.
 */
UCLASS(MinimalAPI, Blueprintable, Abstract)
class UBlueprintCameraDirectorEvaluator : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * Override this method in Blueprint to execute custom logic when this
	 * camera director gets activated.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Activation")
	void ActivateCameraDirector(const FBlueprintCameraDirectorActivateParams& Params);

	/**
	 * Override this method in Blueprint to execute custom logic when this
	 * camera director gets deactivated.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Activation")
	void DeactivateCameraDirector(const FBlueprintCameraDirectorDeactivateParams& Params);
	
	/**
	 * Override this method in Blueprint to execute the custom logic that determines
	 * what camera rig(s) should be active every frame.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Evaluation")
	void RunCameraDirector(const FBlueprintCameraDirectorEvaluationParams& Params);

public:

	/** Activates the given camera rig prefab in the base layer. */
	UFUNCTION(BlueprintCallable, Category="Activation")
	void ActivatePersistentBaseCameraRig(UCameraRigAsset* CameraRigPrefab);

	/** Activates the given camera rig prefab in the global layer. */
	UFUNCTION(BlueprintCallable, Category="Activation")
	void ActivatePersistentGlobalCameraRig(UCameraRigAsset* CameraRigPrefab);

	/** Activates the given camera rig prefab in the visual layer. */
	UFUNCTION(BlueprintCallable, Category="Activation")
	void ActivatePersistentVisualCameraRig(UCameraRigAsset* CameraRigPrefab);

	/** Deactivates the given camera rig prefab in the base layer. */
	UFUNCTION(BlueprintCallable, Category="Activation")
	void DeactivatePersistentBaseCameraRig(UCameraRigAsset* CameraRigPrefab);

	/** Deactivates the given camera rig prefab in the global layer. */
	UFUNCTION(BlueprintCallable, Category="Activation")
	void DeactivatePersistentGlobalCameraRig(UCameraRigAsset* CameraRigPrefab);

	/** Deactivates the given camera rig prefab in the visual layer. */
	UFUNCTION(BlueprintCallable, Category="Activation")
	void DeactivatePersistentVisualCameraRig(UCameraRigAsset* CameraRigPrefab);

public:

	/** Specifies a camera rig to be active this frame. */
	UFUNCTION(BlueprintCallable, Category="Evaluation")
	void ActivateCameraRig(
			UPARAM(meta=(UseBlueprintCameraDirectorRigPicker=true))
			UCameraRigAsset* CameraRig);

	/**
	 * Specifies a camera rig to be active this frame, via a proxy which is later resolved
	 * via the proxy table of the Blueprint camera director.
	 */
	UFUNCTION(BlueprintCallable, Category="Evaluation")
	void ActivateCameraRigViaProxy(UCameraRigProxyAsset* CameraRigProxy);

	/**
	 * Specifies an external camera rig prefab asset to be active this frame.
	 */
	UFUNCTION(BlueprintCallable, Category="Evaluation")
	void ActivateCameraRigPrefab(UCameraRigAsset* CameraRig);

	/** Gets a camera rig from the referencing camera asset. */
	UFUNCTION(BlueprintPure, Category="Evaluation", meta=(HideSelfPin=true))
	UCameraRigAsset* GetCameraRig(
			UPARAM(meta=(UseBlueprintCameraDirectorRigPicker=true))
			UCameraRigAsset* CameraRig) const;

public:

	/**
	 * A utility function that tries to find if an actor owns the evaluation context.
	 * Handles the situation where the evaluation context is an actor component (like a
	 * UGameplayCameraComponent) or an actor itself.
	 */
	UFUNCTION(BlueprintPure, Category="Evaluation", meta=(DeterminesOutputType="ActorClass"))
	AActor* FindEvaluationContextOwnerActor(TSubclassOf<AActor> ActorClass) const;

	/**
	 * Gets the initial evaluation context camera pose.
	 */
	UFUNCTION(BlueprintPure, Category="Evaluation")
	FBlueprintCameraPose GetInitialContextCameraPose() const;

	/**
	 * Sets the initial evaluation context camera pose.
	 * WARNING: this will change the initial pose of ALL running camera rigs!
	 */
	UFUNCTION(BlueprintCallable, Category="Evaluation")
	void SetInitialContextCameraPose(const FBlueprintCameraPose& InCameraPose);

	/**
	 * Gets the initial evaluation context camera variable table.
	 * WARNING: setting variables here will affect ALL running camera rigs!
	 */
	UFUNCTION(BlueprintPure, Category="Evaluation")
	FBlueprintCameraVariableTable GetInitialContextVariableTable() const;

public:

	using FCameraEvaluationContext = UE::Cameras::FCameraEvaluationContext;
	using FBlueprintCameraDirectorEvaluationResult = UE::Cameras::FBlueprintCameraDirectorEvaluationResult;

	/** Native wrapper for ActivateCameraDirector. */
	void NativeActivateCameraDirector(const UE::Cameras::FCameraDirectorActivateParams& Params);

	/** Native wrapper for DeactivateCameraDirector. */
	void NativeDeactivateCameraDirector(const UE::Cameras::FCameraDirectorDeactivateParams& Params);

	/** Native wrapper for RunCameraDirector. */
	void NativeRunCameraDirector(const UE::Cameras::FCameraDirectorEvaluationParams& Params);

	/** Get the last result for this camera director. */
	const FBlueprintCameraDirectorEvaluationResult& GetEvaluationResult() const { return EvaluationResult; }

private:

	/** The current camera director evaluation result. */
	FBlueprintCameraDirectorEvaluationResult EvaluationResult;

	/** The current evaluation context. */
	TSharedPtr<FCameraEvaluationContext> EvaluationContext;

	/** The variable auto-reset service, for using when returning the variable table. */
	TSharedPtr<UE::Cameras::FAutoResetCameraVariableService> VariableAutoResetService;
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

