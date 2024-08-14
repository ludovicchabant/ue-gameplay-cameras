// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Core/CameraSystemEvaluator.h"

#include "GameplayControlRotationComponent.generated.h"

class APlayerController;
class UCanvas;
class UInputAction;

namespace UE::Cameras
{
	class FCameraSystemEvaluator; 
	class FPlayerControlRotationEvaluationService;
}

/**
 * An example component that works with the GameplayCameraComponent to manage a player's
 * control rotation when the camera changes or moves in a way that was not initiated 
 * by the player themselves.
 */
UCLASS(BlueprintType, MinimalAPI, ClassGroup=Camera, HideCategories=(Mobility, Rendering, LOD), meta=(BlueprintSpawnableComponent))
class UGameplayControlRotationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	/** Create a new control rotation component. */
	UGameplayControlRotationComponent(const FObjectInitializer& ObjectInit);

public:

	// UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	APlayerController* GetPlayerController();

public:

	/** The axis input action(s) to read from. */
	UPROPERTY(EditAnywhere, Category="Input")
	TArray<TObjectPtr<UInputAction>> AxisActions;

	/**
	 * The angular speed, in degrees per second, past which a change in the player input
	 * will thaw a frozen control rotation.
	 */
	UPROPERTY(EditAnywhere, Category="Input")
	float AxisActionAngularSpeedThreshold = 20.f;

	/**
	 * The player input magnitude under which the frozen control rotation is thawed.
	 */
	UPROPERTY(EditAnywhere, Category="Input")
	float AxisActionMagnitudeThreshold = 0.1f;

private:

	TSharedPtr<UE::Cameras::FPlayerControlRotationEvaluationService> ControlRotationService;
	TWeakPtr<UE::Cameras::FCameraSystemEvaluator> WeakCameraSystem;
	int32 PlayerIndex = INDEX_NONE;
};

