// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraValueInterpolator.h"
#include "Services/CameraAction.h"

#include "AimAtCameraAction.generated.h"

/** Lock-on policy for the aim-at camera action. */
UENUM()
enum class EAimAtCameraActionLockOnPolicy : uint8
{
	/** End the action once the aiming is accomplished. */
	Disengage,
	/** Continuously aim at the target until the action is explicitly stopped. */
	KeepLock
};

/**
 * A camera action that aims at a given target, using a given interpolation.
 */
UCLASS()
class UAimAtCameraAction : public UCameraAction
{
	GENERATED_BODY()

public:

	/** The target to aim at. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Common, meta=(ExposeOnSpawn=true))
	FVector TargetLocation;

	/** The interpolation to use for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Common, meta=(ExposeOnSpawn=true))
	TObjectPtr<UCameraValueInterpolator> Interpolator;

	/** The tolerance within which we can consider the aiming to be locked on target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Common, meta=(ExposeOnSpawn=true))
	float LockOnAngleTolerance = 0.05f;

	/** The lock-on policy. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Common, meta=(ExposeOnSpawn=true))
	EAimAtCameraActionLockOnPolicy LockOnPolicy = EAimAtCameraActionLockOnPolicy::Disengage;

	/** Where to aim the target in screen-space. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Aiming, meta=(ExposeOnSpawn=true))
	FVector2D TargetFraming = { 0.5, 0.5 };

protected:

	// UCameraAction interface.
	virtual FCameraActionEvaluatorPtr OnBuildEvaluator(FCameraActionEvaluatorBuilder& Builder) const override;
};

