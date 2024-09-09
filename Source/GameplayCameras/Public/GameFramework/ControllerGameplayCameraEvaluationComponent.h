// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Templates/SharedPointerFwd.h"
#include "UObject/ObjectPtr.h"

#include "ControllerGameplayCameraEvaluationComponent.generated.h"

class UCameraRigAsset;
class UGameplayCameraSystemHost;
enum class ECameraRigLayer : uint8;

namespace UE::Cameras
{
	class FCameraEvaluationContext;
}

/**
 * A component, attached to a player controller, that can run camera rigs activated from
 * a global place like the Blueprint functions inside UActivateCameraRigFunctions.
 */
UCLASS(Hidden, MinimalAPI)
class UControllerGameplayCameraEvaluationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UControllerGameplayCameraEvaluationComponent(const FObjectInitializer& ObjectInitializer);

	/** Activates a new camera rig. */
	void ActivateCameraRig(UCameraRigAsset* CameraRig, ECameraRigLayer EvaluationLayer);

public:

	static UControllerGameplayCameraEvaluationComponent* FindComponent(APlayerController* PlayerController);
	static UControllerGameplayCameraEvaluationComponent* FindOrAddComponent(APlayerController* PlayerController);

	static TSharedPtr<UE::Cameras::FCameraEvaluationContext> FindEvaluationContext(APlayerController* PlayerController);
	static TSharedRef<UE::Cameras::FCameraEvaluationContext> FindOrAddEvaluationContext(APlayerController* PlayerController);

public:

	// UActorComponent interface.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	void ActivateCameraRigs();
	void EnsureEvaluationContext();
	void EnsureCameraSystemHost();

private:

	struct FCameraRigInfo
	{
		TObjectPtr<UCameraRigAsset> CameraRig;
		ECameraRigLayer EvaluationLayer;
		bool bActivated = false;
	};

	TArray<FCameraRigInfo> CameraRigInfos;

	TSharedPtr<UE::Cameras::FCameraEvaluationContext> EvaluationContext;

	UPROPERTY()
	TObjectPtr<UGameplayCameraSystemHost> CameraSystemHost;
};

