// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayCameras.h"
#include "UObject/ObjectMacros.h"
#include "Components/SceneComponent.h"

#include "GameplayCameraSystemComponent.generated.h"

class APlayerController;
class UBillboardComponent;
class UCameraRigAsset;
class UCanvas;
class UGameplayCameraSystemHost;
struct FMinimalViewInfo;

namespace UE::Cameras
{
	class FCameraSystemEvaluator;
}

/**
 * A component that hosts a camera system.
 */
UCLASS(BlueprintType, MinimalAPI, ClassGroup=Camera, HideCategories=(Mobility, Rendering, LOD))
class UGameplayCameraSystemComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	using FCameraSystemEvaluator = UE::Cameras::FCameraSystemEvaluator;

	UGameplayCameraSystemComponent(const FObjectInitializer& ObjectInit);

	/** Gets the camera system evaluator. */
	GAMEPLAYCAMERAS_API TSharedPtr<FCameraSystemEvaluator> GetCameraSystemEvaluator(bool bEnsureIfNull = true);

	/** Updates the camera system and returns the computed view. */
	GAMEPLAYCAMERAS_API void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView);

	/** Sets this component's actor as the view target for the given player. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	void ActivateCameraSystemForPlayerIndex(int32 PlayerIndex);

	/** Sets this component's actor as the view target for the given player. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	void ActivateCameraSystemForPlayerController(APlayerController* PlayerController);

	/** Returns whether this component's actor is set as the view target for the given player. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	bool IsCameraSystemActiveForPlayController(APlayerController* PlayerController) const;

	/** Removes this component's actor from being the view target. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	void DeactivateCameraSystem(AActor* NextViewTarget = nullptr);

public:

	// UActorComponent interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

public:

	// Internal API
	void OnBecomeViewTarget();
	void OnEndViewTarget();

private:

#if UE_GAMEPLAY_CAMERAS_DEBUG
	void DebugDraw(UCanvas* Canvas, APlayerController* PlayController);
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

#if WITH_EDITOR
	void CreateCameraSystemSpriteComponent();
#endif  // WITH_EDITOR

public:

	/**
	 * If AutoActivate is set, auto-activates the camera system for the given player.
	 * This sets this actor as the view target, and is equivalent to calling ActivateCameraSystem on BeginPlay.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Activation, meta=(EditCondition="bAutoActivate"))
	TEnumAsByte<EAutoReceiveInput::Type> AutoActivateForPlayer;

	/**
	 * If enabled, sets the evaluated camera orientation as the player controller rotation every frame.
	 * This is set on the player controller that this component was activated for.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Camera)
	bool bSetPlayerControllerRotation = false;

private:
	
	UPROPERTY()
	TObjectPtr<UGameplayCameraSystemHost> CameraSystemHost;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> WeakPlayerController;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	FDelegateHandle DebugDrawDelegateHandle;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

#if WITH_EDITORONLY_DATA

	/** Sprite scaling for the editor. */
	UPROPERTY(transient)
	float EditorSpriteTextureScale = 0.5f;

	/** Sprite component for the editor. */
	UPROPERTY()
	TObjectPtr<UBillboardComponent> EditorSpriteComponent;

#endif	// WITH_EDITORONLY_DATA
};

