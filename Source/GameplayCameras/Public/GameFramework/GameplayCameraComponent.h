// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraAssetReference.h"
#include "GameFramework/GameplayCameraComponentBase.h"

#include "GameplayCameraComponent.generated.h"

/**
 * A component that can run a camera asset inside its own camera evaluation context.
 */
UCLASS(Blueprintable, MinimalAPI, 
		ClassGroup=Camera, 
		meta=(BlueprintSpawnableComponent))
class UGameplayCameraComponent : public UGameplayCameraComponentBase
{
	GENERATED_BODY()

public:

	/** Create a new camera component. */
	GAMEPLAYCAMERAS_API UGameplayCameraComponent(const FObjectInitializer& ObjectInit);

public:

	// UActorComponent interface.
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	// UObject interface.
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	/** The camera asset to run. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Camera, meta=(SequencerHideProperty=true))
	FCameraAssetReference CameraReference;

protected:

	// UGameplayCameraComponentBase interface.
	virtual UCameraAsset* GetCameraAsset() override;
	virtual bool OnValidateCameraEvaluationContextActivation() override;
	virtual void OnUpdateCameraEvaluationContext(bool bForceApplyParameterOverrides) override;

private:

#if WITH_EDITOR
	void OnCameraAssetBuilt(const UCameraAsset* InCameraAsset);
#endif

private:

	UPROPERTY()
	TObjectPtr<UCameraAsset> Camera_DEPRECATED;
};

