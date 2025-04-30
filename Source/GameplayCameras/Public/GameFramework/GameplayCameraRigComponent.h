// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraRigAssetReference.h"
#include "GameFramework/GameplayCameraComponentBase.h"

#include "GameplayCameraRigComponent.generated.h"

/**
 * A component that can run a camera rig asset inside its own camera evaluation context.
 */
UCLASS(Blueprintable, MinimalAPI, 
		ClassGroup=Camera, 
		meta=(BlueprintSpawnableComponent))
class UGameplayCameraRigComponent : public UGameplayCameraComponentBase
{
	GENERATED_BODY()

public:

	/** Create a new camera component. */
	GAMEPLAYCAMERAS_API UGameplayCameraRigComponent(const FObjectInitializer& ObjectInit);

public:

	// UActorComponent interface.
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	// UObject interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	/** The camera rig asset to run. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Camera, meta=(SequencerHideProperty=true))
	FCameraRigAssetReference CameraRigReference;

protected:

	// UGameplayCameraComponentBase interface.
	virtual UCameraAsset* GetCameraAsset() override;
	virtual bool OnValidateCameraEvaluationContextActivation() override;
	virtual void OnUpdateCameraEvaluationContext(bool bForceApplyParameterOverrides) override;

private:

#if WITH_EDITOR
	void OnCameraRigAssetBuilt(const UCameraRigAsset* InCameraRigAsset);
#endif

private:

	UPROPERTY()
	TObjectPtr<UCameraAsset> GeneratedCameraAsset;

#if WITH_EDITOR
	bool bIsBuildingGeneratedCameraAsset = false;
#endif
};

