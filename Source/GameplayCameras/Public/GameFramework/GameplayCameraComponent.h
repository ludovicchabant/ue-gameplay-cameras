// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Core/CameraAssetReference.h"
#include "Core/CameraEvaluationContext.h"
#include "GameFramework/BlueprintCameraNodeEvaluationResult.h"
#include "GameFramework/BlueprintCameraPose.h"
#include "GameFramework/BlueprintCameraVariableTable.h"
#include "UObject/ObjectMacros.h"

#include "GameplayCameraComponent.generated.h"

class APlayerController;
class FPrimitiveDrawInterface;
class FSceneView;
class FViewport;
class UCameraAsset;
class UCanvas;
class UCineCameraComponent;
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
	GAMEPLAYCAMERAS_API TSharedPtr<const UE::Cameras::FCameraEvaluationContext> GetEvaluationContext() const;

	/** Get the camera evaluation context used by this component. */
	GAMEPLAYCAMERAS_API TSharedPtr<UE::Cameras::FCameraEvaluationContext> GetEvaluationContext();

	/** Get the player controller this component is currently activated for (if any). */
	GAMEPLAYCAMERAS_API APlayerController* GetPlayerController() const;

public:

	/** Gets the child camera component used as the "output" for the gameplay/procedural camera. */
	UFUNCTION(BlueprintGetter, Category=Camera)
	UCineCameraComponent* GetOutputCameraComponent() const { return OutputCameraComponent; }

	/** Activates the camera for the given player. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void ActivateCameraForPlayerIndex(int32 PlayerIndex);

	/** Activates the camera for the given player. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void ActivateCameraForPlayerController(APlayerController* PlayerController);

	/** Deactivates the camera for the last player it was activated for. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void DeactivateCamera();

	/** Gets the shared camera evaluation data for this component's evaluation context. */
	UFUNCTION(BlueprintPure, Category=Camera, meta=(DisplayName="Get Shared Camera Data"))
	GAMEPLAYCAMERAS_API FBlueprintCameraNodeEvaluationResult GetInitialResult() const;
	
	/** Gets the camera evaluation data for a given sub-set of camera rigs in this component's evaluation context. */
	UFUNCTION(BlueprintPure, Category=Camera, meta=(DisplayName="Get Conditional Camera Data"))
	GAMEPLAYCAMERAS_API FBlueprintCameraNodeEvaluationResult GetConditionalResult(ECameraEvaluationDataCondition Condition) const;

	/** Gets the initial camera pose for this component's camera evaluation context. */
	UFUNCTION(BlueprintPure, Category=Camera, meta=(DeprecatedFunction, DeprecationMessage="Please use GetSharedCameraData"))
	GAMEPLAYCAMERAS_API FBlueprintCameraPose GetInitialPose() const;

	/** Sets the initial camera pose for this component's camera evaluation context. */
	UFUNCTION(BlueprintCallable, Category=Camera, meta=(DeprecatedFunction, DeprecationMessage="Please use GetSharedCameraData"))
	GAMEPLAYCAMERAS_API bool SetInitialPose(const FBlueprintCameraPose& CameraPose);

	/** Gets the initial camera variable table for this component's camera evaluation context. */
	UFUNCTION(BlueprintPure, Category=Camera, meta=(DeprecatedFunction, DeprecationMessage="Please use GetSharedCameraData"))
	GAMEPLAYCAMERAS_API FBlueprintCameraVariableTable GetInitialVariableTable() const;

public:

	// UActorComponent interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnUnregister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
#if WITH_EDITOR
	virtual bool GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut) override;
#endif 

	// USceneComponent interface.
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport) override;

	// UObject interface.
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

public:

#if WITH_EDITOR
	GAMEPLAYCAMERAS_API void OnDrawVisualizationHUD(const FViewport* Viewport, const FSceneView* SceneView, FCanvas* Canvas) const;
#endif

private:

	void ActivateCameraEvaluationContext(int32 PlayerIndex);
	void ActivateCameraEvaluationContext(APlayerController* PlayerController);
	void EnsureCameraEvaluationContextCreated(APlayerController* PlayerController);
	void UpdateCameraEvaluationContext(bool bApplyParameterOverrides);
	void UpdateOutputCameraComponent();
	void DeactivateCameraEvaluationContext();

#if WITH_EDITOR
	void AutoManageEditorPreviewEvaluator();
	void OnCameraAssetReferenceChanged();
	void OnEditorPreviewCameraRigIndexChanged();
	void OnCameraAssetBuilt(const UCameraAsset* InCameraAsset);

	void UpdateEditorPreviewEvaluator(float DeltaTime);
#endif  // WITH_EDITOR

public:

	/** The camera asset to run. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Camera)
	FCameraAssetReference CameraReference;

	/**
	 * If AutoActivate is set, auto-activates this component's camera for the given player.
	 * This is equivalent to calling ActivateCamera on BeginPlay.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Activation, meta=(EditCondition="bAutoActivate"))
	TEnumAsByte<EAutoReceiveInput::Type> AutoActivateForPlayer;

#if WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, Category=Camera)
	bool bRunInEditor = true;

	UPROPERTY(EditAnywhere, Category=Camera, meta=(EditCondition="bRunInEditor"))
	int32 EditorPreviewCameraRigIndex = 0;

#endif  // WITH_EDITORONLY_DATA

protected:

	UPROPERTY(Transient)
	TObjectPtr<UCineCameraComponent> OutputCameraComponent;

	UPROPERTY(Transient)
	TObjectPtr<UGameplayCameraSystemHost> CameraSystemHost;

protected:

	using FGameplayCameraComponentEvaluationContext = UE::Cameras::FGameplayCameraComponentEvaluationContext;

	TSharedPtr<FGameplayCameraComponentEvaluationContext> EvaluationContext;

	bool bIsCameraCutNextFrame = false;

#if WITH_EDITOR
	
	TSharedPtr<UE::Cameras::FCameraSystemEvaluator> EditorPreviewEvaluator;

	bool bIsEditorWorld = false;

	int32 CustomShowFlag = INDEX_NONE;

#endif  // WITH_EDITOR

private:

	UPROPERTY()
	TObjectPtr<UCameraAsset> Camera_DEPRECATED;
};

namespace UE::Cameras
{

/**
 * Evaluation context for the gameplay camera component.
 */
class FGameplayCameraComponentEvaluationContext : public FCameraEvaluationContext
{
	UE_DECLARE_CAMERA_EVALUATION_CONTEXT(GAMEPLAYCAMERAS_API, FGameplayCameraComponentEvaluationContext)
};

}  // namespace UE::Cameras

