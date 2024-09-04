// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayCameras.h"
#include "UObject/ObjectMacros.h"
#include "Components/SceneComponent.h"

#include "GameplayCameraSystemComponent.generated.h"

class APlayerController;
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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	// USceneComponent interface
#if WITH_EDITOR
	virtual bool GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut) override;
#endif  // WITH_EDITOR

public:

	// Internal API
	void OnBecomeViewTarget();
	void OnEndViewTarget();

private:

#if UE_GAMEPLAY_CAMERAS_DEBUG
	void DebugDraw(UCanvas* Canvas, APlayerController* PlayController);
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

public:

	/**
	 * If set, auto-activates the camera system for the given player.
	 * This sets this actor as the view target, and is equivalent to calling ActivateCameraSystem on BeginPlay.
	 */
	UPROPERTY(EditAnywhere, Category=Camera)
	TEnumAsByte<EAutoReceiveInput::Type> AutoActivateForPlayer;

private:
	
	UPROPERTY()
	TObjectPtr<UGameplayCameraSystemHost> CameraSystemHost;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> WeakPlayerController;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	FDelegateHandle DebugDrawDelegateHandle;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

#if WITH_EDITORONLY_DATA

	UPROPERTY()
	TObjectPtr<UStaticMesh> PreviewMesh;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> PreviewMeshComponent;

#endif	// WITH_EDITORONLY_DATA
};

