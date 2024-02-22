// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectPtr.h"
#include "Core/CameraNodeEvaluator.h"

#include "CameraEvaluationContext.generated.h"

class UCameraAsset;
class APlayerController;

/**
 * Base class for providing a context to running camera rigs.
 */
UCLASS(MinimalAPI)
class UCameraEvaluationContext : public UObject
{
	GENERATED_BODY()

public:

	UCameraEvaluationContext(const FObjectInitializer& ObjectInit);

	/**
	 * Gets the player controller (if any) in control of the cameras running inside
	 * of this evaluation context.
	 */
	APlayerController* GetPlayerController() const { return PlayerController; }

	/** Gets the camera asset that is hosted in this context. */
	UCameraAsset* GetCameraAsset() const { return CameraAsset; }

	/** Gets the initial evaluation result for all camera rigs in this context. */
	const FCameraNodeEvaluationResult& GetInitialResult() const { return InitialResult; }

protected:

	/**
	 * The player controller (if any) in control of the cameras running inside
	 * of this evaluation context.
	 */
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	/** The camera asset hosted in this context. */
	UPROPERTY()
	TObjectPtr<UCameraAsset> CameraAsset;

	/** The initial result for all camera rigs in this context. */
	FCameraNodeEvaluationResult InitialResult;
};

