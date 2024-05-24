// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "UObject/ObjectMacros.h"

#include "GameplayCameraActor.generated.h"

class APlayerController;
class UGameplayCameraComponent;

/**
 * An actor that can run a camera asset.
 */
UCLASS(BlueprintType, MinimalAPI, ClassGroup=Camera, HideCategories=(Input, Rendering))
class AGameplayCameraActor : public AActor
{
	GENERATED_BODY()

public:

	AGameplayCameraActor(const FObjectInitializer& ObjectInit);

public:

	/** Gets the camera component. */
	UFUNCTION(BlueprintGetter, Category=Camera)
	UGameplayCameraComponent* GetCameraComponent() const { return CameraComponent; }

public:

	// AActor interface.
	virtual USceneComponent* GetDefaultAttachComponent() const override;

private:

	UPROPERTY(VisibleAnywhere, Category=Camera, BlueprintGetter="GetCameraComponent", meta=(ExposeFunctionCategories="Camera"))
	TObjectPtr<UGameplayCameraComponent> CameraComponent;
};

