// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/CameraSystemEvaluator.h"
#include "GameplayCameras.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptInterface.h"
#include "Components/SceneComponent.h"

#include "GameplayCameraSystemComponent.generated.h"

class APlayerController;
class UCameraRigAsset;
class UCanvas;
struct FMinimalViewInfo;

namespace UE::Cameras
{

class FCameraSystemEvaluator;

}  // namespace UE::Cameras

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
	TSharedPtr<FCameraSystemEvaluator> GetCameraSystemEvaluator() { return Evaluator; }

	/** Updates the camera system and returns the computed view. */
	GAMEPLAYCAMERAS_API void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView);

	/** Sets this component's actor as the view target for the given player. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	void ActivateCameraSystem(int32 PlayerIndex = 0);

	/** Removes this component's actor from being the view target. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	void DeactivateCameraSystem(AActor* NextViewTarget = nullptr);

public:

	// UActorComponent interface
	virtual void OnRegister() override;
	virtual void Deactivate() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	// USceneComponent interface
#if WITH_EDITOR
	virtual bool GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut) override;
#endif  // WITH_EDITOR
	
	// UObject interface
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

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

	/**
	 * If enabled, sets the evaluated camera orientation as the player controller rotation every frame.
	 * This is set on the player controller that this component was activated for.
	 */
	UPROPERTY(EditAnywhere, Category=Camera)
	bool bSetPlayerControllerRotation = true;

private:
	
	TSharedPtr<FCameraSystemEvaluator> Evaluator;

	int32 ActivatedForPlayerIndex = INDEX_NONE;

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

