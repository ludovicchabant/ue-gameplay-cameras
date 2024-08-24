// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Nodes/CameraNodeTypes.h"
#include "Engine/EngineTypes.h"

#include "CollisionPushCameraNode.generated.h"

class UCameraValueInterpolator;

UENUM()
enum class ECollisionSafePositionOffsetSpace
{
	ActiveContext,
	OwningContext,
	Pivot,
	CameraPose
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

	/** Radius of the sphere used for collision testing. */
	UPROPERTY(EditAnywhere, Category="Collision")
	FFloatCameraParameter CollisionSphereRadius;

	/** Collision channel to use for the line trace. */
	UPROPERTY(EditAnywhere, Category="Collision")
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	/** World-space offset from the target to the line trace's end. */
	UPROPERTY(EditAnywhere, Category="Occlusion")
	FVector3dCameraParameter SafePositionOffset;

	/** What space the safe position offset should be in. */
	UPROPERTY(EditAnywhere, Category=Damping)
	ECollisionSafePositionOffsetSpace SafePositionOffsetSpace = ECollisionSafePositionOffsetSpace::Pivot;

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

