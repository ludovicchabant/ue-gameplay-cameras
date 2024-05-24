// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Core/CameraEvaluationContext.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptInterface.h"

#include "GameplayCameraComponent.generated.h"

class UCameraAsset;
class UCameraEvaluationResultInterop;

namespace UE::Cameras
{

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

	UGameplayCameraComponent(const FObjectInitializer& ObjectInit);

public:

	/** 
	 * Activates the camera for the given player.
	 * This looks up the current player camera manager and/or view target in order to find
	 * the active camera system for the given player. If found, it adds its own camera asset
	 * as the active one.
	 */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void ActivateCamera(int32 PlayerIndex = 0);

	/** Deactivates the camera for the last player it was activated for. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void DeactivateCamera();

	/** Gets the initial evaluation result for this component's context. */
	UFUNCTION(BlueprintPure, Category=Camera)
	GAMEPLAYCAMERAS_API UCameraEvaluationResultInterop* GetInitialResult() const;

public:

	// UActorComponent interface
	virtual void OnRegister() override;
	virtual void Deactivate() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

private:

	void ActivateCamera(APlayerController* PlayerController);
	void DeactivateCamera(APlayerController* PlayerController);

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

	UPROPERTY(Transient)
	TObjectPtr<UCameraEvaluationResultInterop> InitialResultInterop;
	
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

