// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Core/CameraEvaluationContext.h"
#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/BlueprintCameraPose.h"
#include "GameFramework/BlueprintCameraVariableTable.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptInterface.h"

#include "GameplayCameraComponent.generated.h"

class UCameraAsset;

namespace UE::Cameras
{

class FCameraSystemEvaluator;
class FGameplayCameraComponentEvaluationContext;

}  // namespace UE::Cameras

/**
 * A component that can run a camera asset inside its own camera evaluation context.
 */
UCLASS(Blueprintable, MinimalAPI, ClassGroup=Camera, HideCategories=(Mobility, Rendering, LOD), meta=(BlueprintSpawnableComponent))
class UGameplayCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	/** Create a new camera component. */
	UGameplayCameraComponent(const FObjectInitializer& ObjectInit);

	/** Get the camera evaluation context used by this component. */
	TSharedPtr<UE::Cameras::FCameraEvaluationContext> GetEvaluationContext();

	/** Get the player controller index that this component has been activated for. */
	int32 GetPlayerIndex() const { return ActivatedForPlayerIndex; }

public:

	/** 
	 * Activates the camera for the given player.
	 * This looks up the given player's camera manager and/or view target in order to find
	 * the active camera system. If found, this component adds its camera asset as the active one.
	 * If this component was already active for another player, it will be first deactivated.
	 */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void ActivateCamera(int32 PlayerIndex = 0);

	/** Deactivates the camera for the last player it was activated for. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void DeactivateCamera();

	/** Gets the initial camera pose for this component's camera evaluation context. */
	UFUNCTION(BlueprintPure, Category=Camera)
	GAMEPLAYCAMERAS_API FBlueprintCameraPose GetInitialPose() const;

	/** Sets the initial camera pose for this component's camera evaluation context. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void SetInitialPose(const FBlueprintCameraPose& CameraPose);

	/** Gets the initial camera variable table for this component's camera evaluation context. */
	UFUNCTION(BlueprintPure, Category=Camera)
	GAMEPLAYCAMERAS_API FBlueprintCameraVariableTable GetInitialVariableTable() const;

public:

	// UActorComponent interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

private:

	void ActivateCameraEvaluationContext(int32 PlayerIndex);
	void DeactivateCameraEvaluationContext();

	void ActivateCameraEvaluationContext(APlayerController* PlayerController);
	void DeactivateCameraEvaluationContext(APlayerController* PlayerController);

	void DelayedActivateCameraEvaluationContext(APlayerController* PlayerController, AActor* OldViewTarget, AActor* NewViewTarget);
	void AbortDelayedActivateCameraEvaluationContext();

	void DoActivateCameraEvaluationContext(APlayerController* PlayerController, TSharedPtr<UE::Cameras::FCameraSystemEvaluator> CameraSystemEvaluator);

#if WITH_EDITORONLY_DATA

	void UpdatePreviewMeshTransform();

#endif

public:

	/** The camera asset to run. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	TObjectPtr<UCameraAsset> Camera;

	/**
	 * If set, auto-activates this component's camera for the given player.
	 * This is equivalent to calling ActivateCamera on BeginPlay.
	 */
	UPROPERTY(EditAnywhere, Category=Camera)
	TEnumAsByte<EAutoReceiveInput::Type> AutoActivateForPlayer;

protected:

	using FGameplayCameraComponentEvaluationContext = UE::Cameras::FGameplayCameraComponentEvaluationContext;

	TSharedPtr<FGameplayCameraComponentEvaluationContext> EvaluationContext;

#if WITH_EDITORONLY_DATA

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> PreviewMesh;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> PreviewMeshComponent;

#endif	// WITH_EDITORONLY_DATA
	
private:

	int32 ActivatedForPlayerIndex = INDEX_NONE;
};

namespace UE::Cameras
{

/**
 * Evaluation context for the gameplay camera component.
 */
class FGameplayCameraComponentEvaluationContext : public FCameraEvaluationContext
{
	UE_DECLARE_CAMERA_EVALUATION_CONTEXT(GAMEPLAYCAMERAS_API, FGameplayCameraComponentEvaluationContext)

public:

	void Update(UGameplayCameraComponent* Owner);
};

}  // namespace UE::Cameras

