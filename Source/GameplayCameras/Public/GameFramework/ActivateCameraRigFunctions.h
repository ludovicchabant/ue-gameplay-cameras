// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/RootCameraNode.h"
#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SharedPointerFwd.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectPtr.h"

#include "ActivateCameraRigFunctions.generated.h"

class APlayerController;
class UCameraRigAsset;

namespace UE::Cameras
{
	class FCameraEvaluationContext;
	class FCameraSystemEvaluator;
}  // namespace UE::Cameras

/**
 * Blueprint functions for activating camera rigs in the base/global/visual layers.
 *
 * These camera rigs run with a global, shared evaluation context that doesn't provide any
 * meaningful initial result. They are activated on the camera system found to be running
 * on the given player controller.
 */
UCLASS(MinimalAPI)
class UActivateCameraRigFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Activates the given camera rig in the base layer. */
	UFUNCTION(BlueprintCallable, Category="Camera", meta=(WorldContext="WorldContextObject"))
	static void ActivateBaseCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig);

	/** Activates the given camera rig in the global layer. */
	UFUNCTION(BlueprintCallable, Category="Camera", meta = (WorldContext = "WorldContextObject"))
	static void ActivateGlobalCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig);

	/** Activates the given camera rig in the visual layer. */
	UFUNCTION(BlueprintCallable, Category="Camera", meta = (WorldContext = "WorldContextObject"))
	static void ActivateVisualCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig);

	/** Activates the given camera rig in the given layer. */
	UFUNCTION(BlueprintCallable, Category="Camera", meta = (WorldContext = "WorldContextObject"))
	static void ActivateCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig, ECameraRigLayer EvaluationLayer);
};

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

	// UActorComponent interface.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	void ActivateCameraRigs();
	void EnsureEvaluationContext();

	static UE::Cameras::FCameraSystemEvaluator* FindCameraSystemEvaluator(APlayerController* PlayerController);

private:

	struct FCameraRigInfo
	{
		TObjectPtr<UCameraRigAsset> CameraRig;
		ECameraRigLayer EvaluationLayer;
		bool bActivated = false;
	};

	TArray<FCameraRigInfo> CameraRigInfos;
	TSharedPtr<UE::Cameras::FCameraEvaluationContext> EvaluationContext;
};

