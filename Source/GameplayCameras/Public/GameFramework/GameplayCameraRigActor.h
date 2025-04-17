// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameplayCameraActorBase.h"

#include "GameplayCameraRigActor.generated.h"

class UGameplayCameraRigComponent;

/**
 * An actor that can run a camera asset.
 */
UCLASS(BlueprintType, MinimalAPI, ClassGroup=Camera, HideCategories=(Input, Rendering))
class AGameplayCameraRigActor : public AGameplayCameraActorBase
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

protected:

	// AGameplayCameraActorBase interface.
	virtual UGameplayCameraComponentBase* GetCameraComponentBase() const override;

private:

	UPROPERTY(VisibleAnywhere, Category=Camera, BlueprintGetter="GetCameraRigComponent", meta=(ExposeFunctionCategories="Camera"))
	TObjectPtr<UGameplayCameraRigComponent> CameraRigComponent;
};

