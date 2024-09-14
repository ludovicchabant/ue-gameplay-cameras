// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Core/CameraVariableReferences.h"
#include "Nodes/CameraNodeTypes.h"
#include "Engine/EngineTypes.h"

#include "CollisionPushCameraNode.generated.h"

class UCameraValueInterpolator;

/**
 * Specifies how to compute the default safe position for the collision camera node
 * to push towards.
 */
UENUM()
enum class ECollisionSafePosition : uint8
{
	/**
	 * The initial result location of the active evaluation context on the main 
	 * layer's blend stack.
	 */
	ActiveContext,
	/**
	 * The initial result location of the evaluation context of the collision camera node.
	 */
	OwningContext,
	/**
	 * The current pivot. If no pivot is found, fallback to ActiveContext.
	 */
	Pivot,
	/**
	 * The location of the player's controlled pawn.
	 */
	Pawn
};

/**
 * Describes the coordinate system in which to offset the collision camera node's
 * safe position.
 */
UENUM()
enum class ECollisionSafePositionOffsetSpace : uint8
{
	/** The space of the active evaluation context on the main layer's blend stack. */
	ActiveContext,
	/** The space of the evaluation context of the collision camera node. */
	OwningContext,
	/** The space of the current pivot. If no pivot is found, fallback to ActiveContext. */
	Pivot,
	/** The local space of the current camera pose. */
	CameraPose,
	/** The space of the player's controlled pawn. */
	Pawn
};

/**
 * A node that pushes the camera towards a "safe position" when it is colliding with 
 * the environment. By default, the "safe position" is the pivot of the camera (if any) 
 * or the position of the player pawn.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Collision"))
class UCollisionPushCameraNode : public UCameraNode
{
	GENERATED_BODY()

public:

	/** How to compute the safe position. */
	UPROPERTY(EditAnywhere, Category="Safe Position")
	ECollisionSafePosition SafePosition = ECollisionSafePosition::Pivot;

	/**
	 * An optional camera variable to query for a safe position. If null, or if the variable
	 * isn't set, fallback to the value defined by SafePosition.
	 */
	UPROPERTY(EditAnywhere, Category="Safe Position")
	FVector3dCameraVariableReference CustomSafePosition;

	/** World-space offset from the target to the line trace's end. */
	UPROPERTY(EditAnywhere, Category="Safe Position")
	FVector3dCameraParameter SafePositionOffset;

	/** What space the safe position offset should be in. */
	UPROPERTY(EditAnywhere, Category="Safe Position")
	ECollisionSafePositionOffsetSpace SafePositionOffsetSpace = ECollisionSafePositionOffsetSpace::Pivot;

	/**
	 * An optional boolean camera variable that specifies whether collision should be enabled.
	 * When enabled/disabled, the collision push amount will interpolate as per the PushInterpolator
	 * and PullInterpolator.
	 */
	UPROPERTY(EditAnywhere, Category="Collision")
	FBooleanCameraVariableReference EnableCollision;

	/** Radius of the sphere used for collision testing. */
	UPROPERTY(EditAnywhere, Category="Collision")
	FFloatCameraParameter CollisionSphereRadius;

	/** Collision channel to use for the line trace. */
	UPROPERTY(EditAnywhere, Category="Collision")
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECollisionChannel::ECC_Camera;

	/** The interpolation to use when pushing the camera towards the safe position. */
	UPROPERTY(EditAnywhere, Category="Collision")
	TObjectPtr<UCameraValueInterpolator> PushInterpolator;

	/** The interpolation to use when pulling the camera back to its ideal position. */
	UPROPERTY(EditAnywhere, Category="Collision")
	TObjectPtr<UCameraValueInterpolator> PullInterpolator;

	/**
	 * Whether to run the collision asynchrnously. 
	 * This is better for performance, but results in collision handling being one frame late.
	 */
	UPROPERTY(EditAnywhere, Category="Collision")
	bool bRunAsyncCollision = false;

public:

	UCollisionPushCameraNode(const FObjectInitializer& ObjectInit);

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
};

