// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraRigTransition.h"
#include "GameplayTagContainer.h"

#include "GameplayTagTransitionConditions.generated.h"

/**
 * A transition condition that matches the gameplay tags on the previous
 * camera rig and asset.
 */
UCLASS(MinimalAPI)
class UPreviousGameplayTagTransitionCondition 
	: public UCameraRigTransitionCondition
{
	GENERATED_BODY()

public:

	/** The gameplay tags to look for on the previous camera rig/asset. */
	UPROPERTY(EditAnywhere, Category=Transition)
	FGameplayTagQuery GameplayTagQuery;

protected:

	// UCameraRigTransitionCondition interface.
	virtual bool OnTransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const override;
};

/**
 * A transition condition that matches the gameplay tags on the next
 * camera rig and asset.
 */
UCLASS(MinimalAPI)
class UNextGameplayTagTransitionCondition 
	: public UCameraRigTransitionCondition
{
	GENERATED_BODY()

public:

	/** The gameplay tags to look for on the next camera rig/asset. */
	UPROPERTY(EditAnywhere, Category=Transition)
	FGameplayTagQuery GameplayTagQuery;

protected:

	// UCameraRigTransitionCondition interface.
	virtual bool OnTransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const override;
};

