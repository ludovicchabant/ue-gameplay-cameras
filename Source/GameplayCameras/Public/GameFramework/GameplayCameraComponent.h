// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Core/CameraEvaluationContext.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptInterface.h"

#include "GameplayCameraComponent.generated.h"

class UCameraAsset;

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

	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void ActivateCamera(int32 PlayerIndex = 0);

	UFUNCTION(BlueprintCallable, Category=Camera)
	GAMEPLAYCAMERAS_API void DeactivateCamera();

public:

	// UActorComponent interface
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

private:

	void ActivateCamera(APlayerController* PlayerController);
	void DeactivateCamera(APlayerController* PlayerController);

#if WITH_EDITORONLY_DATA

	void UpdatePreviewMeshTransform();

#endif

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	TObjectPtr<UCameraAsset> Camera;

protected:

	using FGameplayCameraComponentEvaluationContext = UE::Cameras::FGameplayCameraComponentEvaluationContext;

	TSharedPtr<FGameplayCameraComponentEvaluationContext> EvaluationContext;
	
#if WITH_EDITORONLY_DATA

	UPROPERTY()
	TObjectPtr<UStaticMesh> PreviewMesh;

	UPROPERTY()
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
	UE_DECLARE_CAMERA_EVALUATION_CONTEXT(FGameplayCameraComponentEvaluationContext)

public:

	void Initialize(UGameplayCameraComponent* Owner, APlayerController* InPlayerController);
	void Update(UGameplayCameraComponent* Owner);
};

}  // namespace UE::Cameras

