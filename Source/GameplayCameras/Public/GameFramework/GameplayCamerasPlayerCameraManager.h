// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "Core/CameraEvaluationContext.h"
#include "Nodes/Blends/SimpleBlendCameraNode.h"

#include "GameplayCamerasPlayerCameraManager.generated.h"

class AGameplayCameraSystemActor;
class UGameplayCameraSystemHost;

namespace UE::Cameras
{

class FCameraSystemEvaluator;
class FViewTargetContextReferencerService;

}  // namespace UE::Cameras

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
class AGameplayCamerasPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	AGameplayCamerasPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	/** Gets the camera system host object. */
	UGameplayCameraSystemHost* GetCameraSystemHost() const { return CameraSystemHost; }

public:

	/** Replace the camera manager currently set on the provided controller with this camera manager. */
	UFUNCTION(BlueprintCallable, Category="Camera")
	void StealPlayerController(APlayerController* PlayerController);

	/** Restore an originally stolen camera manager (see StealPlayerController). */
	UFUNCTION(BlueprintCallable, Category="Camera")
	void ReleasePlayerController();

public:

	// APlayerCameraManager interface.
	virtual void InitializeFor(APlayerController* PlayerController) override;
	virtual void SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

	// AActor interface.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

protected:

	// APlayerCameraManager interface.
	virtual void DoUpdateCamera(float DeltaTime) override;

private:

	void AcquireCameraSystemHost(APlayerController* PlayerController);
	void ReleaseCameraSystemHost();

private:

	UPROPERTY(Transient)
	TObjectPtr<UGameplayCameraSystemHost> CameraSystemHost;

	UPROPERTY(Transient)
	TObjectPtr<APlayerCameraManager> OriginalCameraManager;

	UPROPERTY(Transient)
	TWeakObjectPtr<AGameplayCameraSystemActor> WeakOriginalAutoCameraSystemActor;

	TSharedPtr<UE::Cameras::FCameraSystemEvaluator> CameraSystemEvaluator;

	TSharedPtr<UE::Cameras::FViewTargetContextReferencerService> ViewTargetContextReferencerService;

	FMinimalViewInfo LastFrameDesiredView;
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

