// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "Core/CameraEvaluationContext.h"
#include "GameFramework/IGameplayCameraSystemHost.h"
#include "Nodes/Blends/SimpleBlendCameraNode.h"

#include "GameplayCamerasPlayerCameraManager.generated.h"

struct FCameraRigInstanceID;

namespace UE::Cameras
{

class FCameraEvaluationContext;
class FCameraSystemEvaluator;
class FViewTargetContextReferencerService;

}  // namespace UE::Cameras

/**
 * Defines how the GameplayCamerasPlayerCameraManager should handle the player's view rotation.
 */
UENUM()
enum class EGameplayCamerasViewRotationMode
{
	/**
	 * Don't do anything with the view rotation. This is suitable if the player camera input
	 * and control rotation are handled by custom logic, and the Gameplay Cameras only use
	 * the resulting control rotation (i.e. none of the camera rigs use player input nodes).
	 */
	None,
	/**
	 * Runs a "light" update of the whole camera system and comes up with a good approximation of
	 * the resulting camera orientation. Computationally more expensive than just computing a
	 * yaw and pitch angle, but handles blending camera rigs with different orientations while
	 * preserving aim.
	 */
	PreviewUpdate
};

/**
 * A player camera manager that runs the GameplayCameras camera system.
 *
 * Setting the view target does the following:
 * - Push a new evaluation context for the provided view target actor.
 *    - If that actor contains a GameplayCameraComponent, use its evaluation context directly.
 *    - If that actor contains a CameraComponent, make an evaluation context that wraps it
 *      and runs by simply copying that camera's properties (see FCameraActorCameraEvaluationContext).
 *    - For other actors, do as above, but convert the output of the actor's CalcCamera function.
 * - The old view target's evaluation context is immediately removed from the evaluation stack.
 *   For other way to handle evaluation contexts, call methods directly on the camera system
 *   evaluator instead of going through the base APlayerCameraManager class.
 *
 * There is only ever one active view target, the "pending" view target isn't used. This is
 * because we may be blending between more than two camera rigs that may belong to more than
 * two actors.
 */
UCLASS(notplaceable, MinimalAPI)
class AGameplayCamerasPlayerCameraManager 
	: public APlayerCameraManager
	, public IGameplayCameraSystemHost
{
	GENERATED_BODY()

public:

	AGameplayCamerasPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

public:

	/** Replace the camera manager currently set on the provided controller with this camera manager. */
	UFUNCTION(BlueprintCallable, Category="Camera")
	void StealPlayerController(APlayerController* PlayerController);

	/** Restore an originally stolen camera manager (see StealPlayerController). */
	UFUNCTION(BlueprintCallable, Category="Camera")
	void ReleasePlayerController();

public:

	UFUNCTION(BlueprintCallable, Category="Camera")
	FCameraRigInstanceID StartGlobalCameraModifierRig(const UCameraRigAsset* CameraRig, int32 OrderKey = 0);

	UFUNCTION(BlueprintCallable, Category="Camera")
	FCameraRigInstanceID StartVisualCameraModifierRig(const UCameraRigAsset* CameraRig, int32 OrderKey = 0);

	UFUNCTION(BlueprintCallable, Category="Camera")
	void StopCameraModifierRig(FCameraRigInstanceID InstanceID, bool bImmediately = false);

public:

	// APlayerCameraManager interface.
	virtual void InitializeFor(APlayerController* PlayerController) override;
	virtual void SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

	// AActor interface.
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

	// UObject interface.
	virtual void BeginDestroy() override;
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

	// IGameplayCameraSystemHost interface.
	virtual UObject* GetAsObject() override { return this; }

protected:

	// APlayerCameraManager interface.
	virtual void DoUpdateCamera(float DeltaTime) override;

private:

	void RunViewRotationPreviewUpdate(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot);

	void OnContextStackChanged();
	void CleanUpViewTargetContexts();

public:

	/** The view rotation handling mode to use. */
	UPROPERTY(EditAnywhere, Category="Camera", meta=(EditCondition="bOverrideViewRotationMode"))
	EGameplayCamerasViewRotationMode ViewRotationMode = EGameplayCamerasViewRotationMode::None;

	/** Whether the default view rotation mode setting should be overriden. */
	UPROPERTY(EditAnywhere, Category="Camera")
	bool bOverrideViewRotationMode = false;

private:

	UPROPERTY(Transient)
	TObjectPtr<APlayerCameraManager> OriginalCameraManager;

	TArray<TSharedRef<UE::Cameras::FCameraEvaluationContext>> ViewTargetContexts;

	FMinimalViewInfo LastFrameDesiredView;

	bool bIsSettingNewViewTarget = false;
};

/**
 * A blend node that implements the blend algorithms of the FViewTargetTransitionParams.
 */
UCLASS(MinimalAPI, Hidden)
class UViewTargetTransitionParamsBlendCameraNode : public USimpleBlendCameraNode
{
	GENERATED_BODY()

public:

	/** The transition params to use. */
	UPROPERTY()
	FViewTargetTransitionParams TransitionParams;

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
};

