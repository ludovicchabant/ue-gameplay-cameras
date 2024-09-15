// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Core/CameraEvaluationContext.h"
#include "GameFramework/BlueprintCameraPose.h"
#include "GameFramework/BlueprintCameraVariableTable.h"
#include "UObject/ObjectMacros.h"

#include "GameplayCameraComponent.generated.h"

class APlayerController;
class UCameraAsset;
class UGameplayCameraSystemHost;

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
	GAMEPLAYCAMERAS_API UGameplayCameraComponent(const FObjectInitializer& ObjectInit);

	/** Get the camera evaluation context used by this component. */
	GAMEPLAYCAMERAS_API TSharedPtr<UE::Cameras::FCameraEvaluationContext> GetEvaluationContext();

	/** Get the player controller this component is currently activated for (if any). */
	GAMEPLAYCAMERAS_API APlayerController* GetPlayerController() const;

public:

	/** Activates the camera for the given player. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void ActivateCameraForPlayerIndex(int32 PlayerIndex);

	/** Activates the camera for the given player. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void ActivateCameraForPlayerController(APlayerController* PlayerController);

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
#if WITH_EDITOR
	virtual bool GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut) override;
#endif 

	// USceneComponent interface.
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport) override;

private:

	void ActivateCameraEvaluationContext(int32 PlayerIndex);
	void ActivateCameraEvaluationContext(APlayerController* PlayerController);
	void DeactivateCameraEvaluationContext();

#if WITH_EDITORONLY_DATA

	void UpdatePreviewMeshTransform();

#endif

public:

	/** The camera asset to run. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Camera)
	TObjectPtr<UCameraAsset> Camera;

	/**
	 * If AutoActivate is set, auto-activates this component's camera for the given player.
	 * This is equivalent to calling ActivateCamera on BeginPlay.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Activation, meta=(EditCondition="bAutoActivate"))
	TEnumAsByte<EAutoReceiveInput::Type> AutoActivateForPlayer;

protected:

	using FGameplayCameraComponentEvaluationContext = UE::Cameras::FGameplayCameraComponentEvaluationContext;

	TSharedPtr<FGameplayCameraComponentEvaluationContext> EvaluationContext;

	bool bIsCameraCutNextFrame = false;

	UPROPERTY()
	TObjectPtr<UGameplayCameraSystemHost> CameraSystemHost;

#if WITH_EDITORONLY_DATA

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> PreviewMesh;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> PreviewMeshComponent;

#endif	// WITH_EDITORONLY_DATA
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

