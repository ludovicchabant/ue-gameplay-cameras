// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "UObject/ObjectMacros.h"

#include "GameplayCameraRigActor.generated.h"

class UGameplayCameraRigComponent;

/**
 * An actor that can run a camera asset.
 */
UCLASS(BlueprintType, MinimalAPI, ClassGroup=Camera, HideCategories=(Input, Rendering))
class AGameplayCameraRigActor : public AActor
{
	GENERATED_BODY()

public:

	AGameplayCameraRigActor(const FObjectInitializer& ObjectInit);

public:

	/** Gets the camera component. */
	UFUNCTION(BlueprintGetter, Category=Camera)
	UGameplayCameraRigComponent* GetCameraRigComponent() const { return CameraRigComponent; }

public:

	// AActor interface.
	virtual USceneComponent* GetDefaultAttachComponent() const override;
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:

	UPROPERTY(VisibleAnywhere, Category=Camera, BlueprintGetter="GetCameraRigComponent", meta=(ExposeFunctionCategories="Camera"))
	TObjectPtr<UGameplayCameraRigComponent> CameraRigComponent;
};

