// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraDirector.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "GameFramework/BlueprintCameraEvaluationDataRef.h"
#include "Templates/SubclassOf.h"

#include "BlueprintCameraDirector.generated.h"

class UCameraRigAsset;
class UCameraRigProxyAsset;
enum class ECameraRigLayer : uint8;

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
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Camera Director|Activation")
	void ActivateCameraDirector(UObject* EvaluationContextOwner);

	/**
	 * Override this method in Blueprint to execute custom logic when this
	 * camera director gets deactivated.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Camera Director|Activation")
	void DeactivateCameraDirector(UObject* EvaluationContextOwner);
	
	/**
	 * Override this method in Blueprint to execute the custom logic that determines
	 * what camera rig(s) should be active every frame.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Evaluation")
	void RunCameraDirector(float DeltaTime, UObject* EvaluationContextOwner);

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Evaluation")
	FName AddChildEvaluationContext(UObject* ChildEvaluationContextOwner);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Evaluation")
	bool RemoveChildEvaluationContext(UObject* ChildEvaluationContextOwner, FName ChildSlotName);

	UFUNCTION(BlueprintCallable, Category="Evaluation")
	bool RunChildCameraDirector(float DeltaTime, FName ChildSlotName);

public:

	/** Activates the given camera rig prefab in the base layer. */
	UFUNCTION(BlueprintCallable, Category="Camera Director|Activation")
	void ActivatePersistentBaseCameraRig(UCameraRigAsset* CameraRigPrefab);

	/** Activates the given camera rig prefab in the global layer. */
	UFUNCTION(BlueprintCallable, Category="Camera Director|Activation")
	void ActivatePersistentGlobalCameraRig(UCameraRigAsset* CameraRigPrefab);

	/** Activates the given camera rig prefab in the visual layer. */
	UFUNCTION(BlueprintCallable, Category="Camera Director|Activation")
	void ActivatePersistentVisualCameraRig(UCameraRigAsset* CameraRigPrefab);

	/** Deactivates the given camera rig prefab in the base layer. */
	UFUNCTION(BlueprintCallable, Category="Camera Director|Activation")
	void DeactivatePersistentBaseCameraRig(UCameraRigAsset* CameraRigPrefab);

	/** Deactivates the given camera rig prefab in the global layer. */
	UFUNCTION(BlueprintCallable, Category="Camera Director|Activation")
	void DeactivatePersistentGlobalCameraRig(UCameraRigAsset* CameraRigPrefab);

	/** Deactivates the given camera rig prefab in the visual layer. */
	UFUNCTION(BlueprintCallable, Category="Camera Director|Activation")
	void DeactivatePersistentVisualCameraRig(UCameraRigAsset* CameraRigPrefab);

public:

	/** Specifies a camera rig to be active this frame. */
	UFUNCTION(BlueprintCallable, Category="Camera Director|Evaluation")
	void ActivateCameraRig(UCameraRigAsset* CameraRig, bool bForceNewInstance = false);

	/**
	 * Specifies a camera rig to be active this frame, via a proxy which is later resolved
	 * via the proxy table of the Blueprint camera director.
	 */
	UFUNCTION(BlueprintCallable, Category="Camera Director|Evaluation")
	void ActivateCameraRigViaProxy(UCameraRigProxyAsset* CameraRigProxy, bool bForceNewInstance = false);

public:

	/**
	 * A utility function that tries to find if an actor owns the evaluation context.
	 * Handles the situation where the evaluation context is an actor component (like a
	 * UGameplayCameraComponent) or an actor itself.
	 */
	UFUNCTION(BlueprintPure, Category="Camera Director|Evaluation", meta=(DeterminesOutputType="ActorClass"))
	AActor* FindEvaluationContextOwnerActor(TSubclassOf<AActor> ActorClass) const;

	/** Gets the shared evaluation context data. */
	UFUNCTION(BlueprintPure, Category="Camera Director|Evaluation", meta=(DisplayName="Get Shared Camera Data"))
	FBlueprintCameraEvaluationDataRef GetInitialContextResult() const;

	/** Gets the evaluation context data for a sub-set of camera rigs. */	
	UFUNCTION(BlueprintPure, Category="Camera Director|Evaluation", meta=(DisplayName="Get Conditional Camera Data"))
	FBlueprintCameraEvaluationDataRef GetConditionalContextResult(ECameraEvaluationDataCondition Condition) const;

public:

	// UObject interface.
	virtual UWorld* GetWorld() const override;
#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
#endif  // WITH_EDITOR

public:

	using FCameraEvaluationContext = UE::Cameras::FCameraEvaluationContext;
	using FCameraDirectorEvaluationResult = UE::Cameras::FCameraDirectorEvaluationResult;

	// Internal API.

	const FCameraDirectorEvaluationResult& GetEvaluationResult() const { return EvaluationResult; }

public:

	void NativeInitializeCameraDirector(const UE::Cameras::FCameraDirectorInitializeParams& Params);

	/** Native wrapper for ActivateCameraDirector. */
	void NativeActivateCameraDirector(const UE::Cameras::FCameraDirectorActivateParams& Params);

	/** Native wrapper for DeactivateCameraDirector. */
	void NativeDeactivateCameraDirector(const UE::Cameras::FCameraDirectorDeactivateParams& Params);

	/** Native wrapper for RunCameraDirector. */
	void NativeRunCameraDirector(const UE::Cameras::FCameraDirectorEvaluationParams& Params);

	bool NativeAddChildEvaluationContext(UObject* ChildEvaluationContextOwner);

	bool NativeRemoveChildEvaluationContext(UObject* ChildEvaluationContextOwner);

private:

	/** The current evaluation result. */
	FCameraDirectorEvaluationResult EvaluationResult;

	/** The current evaluation context. */
	TSharedPtr<FCameraEvaluationContext> EvaluationContext;

	/** Currently registered children contexts. */
	TArray<FName> ChildrenContextSlotNames;

	/** Cached world. */
	mutable TWeakObjectPtr<UWorld> WeakCachedWorld;
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
	virtual void OnBuildCameraDirector(UE::Cameras::FCameraBuildLog& BuildLog) override;
	virtual void OnGatherRigUsageInfo(FCameraDirectorRigUsageInfo& UsageInfo) override;
};

