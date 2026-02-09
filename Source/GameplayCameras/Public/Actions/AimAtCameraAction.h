// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Actions/BaseAimAtCameraAction.h"

#include "AimAtCameraAction.generated.h"

/**
 * A camera action that aims at a given target, using a given interpolation.
 */
UCLASS()
class UAimAtCameraAction : public UBaseAimAtCameraAction
{
	GENERATED_BODY()

public:

	/** The target to aim at. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Common, meta=(ExposeOnSpawn=true))
	FVector TargetLocation;

protected:

	// UCameraAction interface.
	virtual FCameraActionEvaluatorPtr OnBuildEvaluator(FCameraActionEvaluatorBuilder& Builder) const override;
};

