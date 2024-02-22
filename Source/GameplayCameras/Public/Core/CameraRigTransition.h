// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "CoreTypes.h"
#include "UObject/ObjectPtr.h"

#include "CameraRigTransition.generated.h"

class UBlendCameraNode;
class UCameraAsset;
class UCameraRigAsset;

/**
 * Parameter structure for camera transitions.
 */
struct FCameraRigTransitionConditionMatchParams
{
	/** The previous camera rig. */
	const UCameraRigAsset* FromCameraRig = nullptr;
	/** The previous camera asset. */
	const UCameraAsset* FromCameraAsset = nullptr;

	/** The next camera rig. */
	const UCameraRigAsset* ToCameraRig = nullptr;
	/** The next camera asset. */
	const UCameraAsset* ToCameraAsset = nullptr;
};

/**
 * Base class for a camera transition condition.
 */
UCLASS(Abstract, DefaultToInstanced, MinimalAPI)
class UCameraRigTransitionCondition : public UObject
{
	GENERATED_BODY()

public:

	/** Evaluates whether this transition should be used. */
	bool TransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const;

protected:

	/** Evaluates whether this transition should be used. */
	virtual bool OnTransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const { return false; }
};

/**
 * A camera transition.
 */
USTRUCT()
struct FCameraRigTransition
{
	GENERATED_BODY()

	/** The list of conditions that must pass for this transition to be used. */
	UPROPERTY(EditAnywhere, Instanced, Category=Common)
	TArray<TObjectPtr<UCameraRigTransitionCondition>> Conditions;

	/** The blend to use to blend a given camera rig in or out. */
	UPROPERTY(EditAnywhere, Instanced, Category=Common)
	TObjectPtr<UBlendCameraNode> Blend;
};

