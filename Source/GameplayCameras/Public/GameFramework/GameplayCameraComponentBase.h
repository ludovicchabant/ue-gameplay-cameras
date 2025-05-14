// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Core/CameraAssetReference.h"
#include "Core/CameraEvaluationContext.h"
#include "GameFramework/BlueprintCameraEvaluationDataRef.h"
#include "GameFramework/IGameplayCameraSystemHost.h"
#include "UObject/ObjectMacros.h"

#include "GameplayCameraComponentBase.generated.h"

class APlayerController;
class FPrimitiveDrawInterface;
class FSceneView;
class FViewport;
class UCameraAsset;
class UCanvas;
class UCineCameraComponent;
struct FCameraContextDataTableAllocationInfo;
struct FCameraVariableTableAllocationInfo;

namespace UE::Cameras
{

class FCameraSystemEvaluator;
class FGameplayCameraComponentEvaluationContext;

}  // namespace UE::Cameras

/**
 * Defines how to activate a gameplay camera component.
 */
UENUM()
enum class EGameplayCameraComponentActivationMode : uint8
{
	/** Push the camera director over any existing ones. */
	Push,
	/** Push the camera director and try to insert any active one as a child. */
	PushAndInsert,
	/** Inserts the camera director as a child of the active one, or push it if there is no active one. */
	InsertOrPush
};

/**
 * A component that can run a camera asset inside its own camera evaluation context.
 */
UCLASS(Blueprintable, MinimalAPI, Abstract,
		ClassGroup=Camera, 
		HideCategories=(Mobility, Rendering, LOD), 
		meta=(BlueprintSpawnableComponent))
class UGameplayCameraComponentBase 
	: public USceneComponent
	, public IGameplayCameraSystemHost
{
	GENERATED_BODY()

public:

	/** Create a new camera component. */
	GAMEPLAYCAMERAS_API UGameplayCameraComponentBase(const FObjectInitializer& ObjectInit);

	/** Get the camera evaluation context used by this component. */
	GAMEPLAYCAMERAS_API TSharedPtr<const UE::Cameras::FCameraEvaluationContext> GetEvaluationContext() const;

	/** Get the camera evaluation context used by this component. */
	GAMEPLAYCAMERAS_API TSharedPtr<UE::Cameras::FCameraEvaluationContext> GetEvaluationContext();

public:

	/** Gets the child camera component used as the "output" for the gameplay/procedural camera. */
	UFUNCTION(BlueprintGetter, Category=Camera)
	UCineCameraComponent* GetOutputCameraComponent() const { return OutputCameraComponent; }

	/** 
	 * Activates the camera for the given player.
	 *
	 * @param PlayerIndex        The player to activate the camera for.
	 * @param bSetAsViewTarget   Whether to set this component's actor as the view target for the player.
	 * @param ActivationMode     How to activate this camera into the player's camera system. Only valid 
	 *                           and used when the player camera manager is running the camera system.
	 *                           Must be 'Push' otherwise, when this component runs as a standalone camera
	 *                           system.
	 */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void ActivateCameraForPlayerIndex(
			int32 PlayerIndex, 
			bool bSetAsViewTarget = true,
			EGameplayCameraComponentActivationMode ActivationMode = EGameplayCameraComponentActivationMode::Push);

	/** 
	 * Activates the camera for the given player.
	 *
	 * @param PlayerController   The player to activate the camera for.
	 * @param bSetAsViewTarget   Whether to set this component's actor as the view target for the player.
	 * @param ActivationMode     How to activate this camera into the player's camera system. Only valid 
	 *                           and used when the player camera manager is running the camera system.
	 *                           Must be 'Push' otherwise, when this component runs as a standalone camera
	 *                           system.
	 */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void ActivateCameraForPlayerController(
			APlayerController* PlayerController,
			bool bSetAsViewTarget = true,
			EGameplayCameraComponentActivationMode ActivationMode = EGameplayCameraComponentActivationMode::Push);

	/** 
	 * Deactivates the camera.
	 *
	 * @param bImmediately       Whether to let this component's camera rigs gracefully blend out before
	 *                           deactivating. If true, any running camera rigs will be frozen or forcibly
	 *                           removed from the camera system.
	 */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void DeactivateCamera(bool bImmediately = false);

	/** Gets the shared camera evaluation data for this component's evaluation context. */
	UFUNCTION(BlueprintPure, Category=Camera, meta=(DisplayName="Get Shared Camera Data"))
	GAMEPLAYCAMERAS_API FBlueprintCameraEvaluationDataRef GetInitialResult() const;

	/** Gets the camera evaluation data for a given sub-set of camera rigs in this component's evaluation context. */
	UFUNCTION(BlueprintPure, Category=Camera, meta=(DisplayName="Get Conditional Camera Data"))
	GAMEPLAYCAMERAS_API FBlueprintCameraEvaluationDataRef GetConditionalResult(ECameraEvaluationDataCondition Condition) const;

public:

	// UActorComponent interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
#if WITH_EDITOR
	virtual bool GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut) override;
#endif 

	// USceneComponent interface.
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport) override;

	// UObject interface.
	virtual void BeginDestroy() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

	// IGameplayCameraSystemHost interface.
	virtual UObject* GetAsObject() override { return this; }

public:

	bool CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult);

#if WITH_EDITOR
	GAMEPLAYCAMERAS_API void OnDrawVisualizationHUD(const FViewport* Viewport, const FSceneView* SceneView, FCanvas* Canvas) const;
#endif  // WITH_EDITOR

protected:

	// UGameplayCameraComponentBase interface.
	virtual UCameraAsset* GetCameraAsset() PURE_VIRTUAL(UGameplayCameraComponentBase::GetCameraAsset, return nullptr;)
	virtual bool OnValidateCameraEvaluationContextActivation() { return true; }
	virtual void OnUpdateCameraEvaluationContext(bool bForceApplyParameterOverrides) {}

	void UpdateCameraEvaluationContext(bool bForceApplyParameterOverrides);
	bool HasCameraEvaluationContext() const { return EvaluationContext.IsValid(); }

	bool IsEditorWorld() const;
	void UpdateControlRotationIfNeeded();

#if WITH_EDITOR
	void ReinitializeCameraEvaluationContext(
			const FCameraVariableTableAllocationInfo& VariableTableAllocationInfo,
			const FCameraContextDataTableAllocationInfo& ContextDataTableAllocationInfo);
	void RecreateEditorWorldCameraEvaluationContext();
#endif  // WITH_EDITOR

private:

	bool CanRunCameraSystem() const;
	void EnsureCameraSystemHost();
	void TeardownCameraSystemHost();

	void ActivateCameraEvaluationContext(APlayerController* PlayerController, IGameplayCameraSystemHost* Host, EGameplayCameraComponentActivationMode ActivationMode);
	void TryCreateCameraEvaluationContext(APlayerController* PlayerController);
	void UpdateOutputCameraComponent();
	void DeactivateCameraEvaluationContext(bool bImmediately);
	void CheckPendingDeactivation();

#if WITH_EDITOR
	void AutoManageEditorPreviewEvaluator();
	void OnEditorPreviewCameraRigIndexChanged();
#endif  // WITH_EDITOR

public:

	/**
	 * If AutoActivate is set, auto-activates this component's camera for the given player.
	 * This is equivalent to calling ActivateCameraForPlayerIndex on BeginPlay.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Activation, meta=(EditCondition="bAutoActivate"))
	TEnumAsByte<EAutoReceiveInput::Type> AutoActivateForPlayer;

	/**
	 * Specifies whether this component should set the player controller's control rotation 
	 * to the computed point of view's orientation every frame. This is only used when a 
	 * player controller is associated with this component, and the view target is that
	 * component.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Camera)
	bool bSetControlRotationWhenViewTarget = false;

#if WITH_EDITORONLY_DATA

	/** Whether to run this camera in editor. */
	UPROPERTY(EditAnywhere, Category=Camera)
	bool bRunInEditor = true;

	/** The camera rig to run in the editor. */
	UPROPERTY(EditAnywhere, Category=Camera, meta=(EditCondition="bRunInEditor"))
	int32 EditorPreviewCameraRigIndex = 0;

#endif  // WITH_EDITORONLY_DATA

private:

	UPROPERTY(Transient)
	TObjectPtr<UCineCameraComponent> OutputCameraComponent;

private:

	using FGameplayCameraComponentEvaluationContext = UE::Cameras::FGameplayCameraComponentEvaluationContext;

	TSharedPtr<FGameplayCameraComponentEvaluationContext> EvaluationContext;

	bool bIsCameraCutNextFrame = false;

	bool bIsDeactivating = false;

#if WITH_EDITOR
	
	bool bIsEditorWorld = false;

	bool bIsFirstActivation = true;

	int32 CustomShowFlag = INDEX_NONE;

#endif  // WITH_EDITOR
};

namespace UE::Cameras
{

/**
 * Evaluation context for the gameplay camera component.
 */
class FGameplayCameraComponentEvaluationContext : public FCameraEvaluationContext
{
	UE_DECLARE_CAMERA_EVALUATION_CONTEXT(GAMEPLAYCAMERAS_API, FGameplayCameraComponentEvaluationContext)

#if WITH_EDITOR

public:

	void UpdateForEditorPreview();

#endif  // WITH_EDITOR
};

}  // namespace UE::Cameras

