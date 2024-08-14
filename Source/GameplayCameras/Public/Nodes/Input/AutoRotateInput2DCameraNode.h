// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraParameters.h"
#include "Math/MathFwd.h"
#include "Nodes/Input/Input2DCameraNode.h"

#include "AutoRotateInput2DCameraNode.generated.h"

class UCameraValueInterpolator;

/** Describes the type of auto-rotate. */
UENUM()
enum class ECameraAutoRotateDirection
{
	/** Re-align towards the evaluation context's facing. */
	Facing,
	/** Re-align towards the evaluation context's movement direction. */
	Movement
};

/**
 * An input node that modifies a yaw/pitch input in order to re-align its
 * values to a given default direction.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Input"))
class UAutoRotateInput2DCameraNode : public UInput2DCameraNode
{
	GENERATED_BODY()

public:

	/** The direction to re-align towards. */
	UPROPERTY(EditAnywhere, Category="Auto-Rotate")
	ECameraAutoRotateDirection Direction;

	/** The time, in seconds, to wait before re-aligning. */
	UPROPERTY(EditAnywhere, Category="Auto-Rotate")
	FFloatCameraParameter WaitTime = 1.f;

	/** The minimum player-induced/manual rotation, in degrees, to deactivate auto-rotation. */
	UPROPERTY(EditAnywhere, Category="Auto-Rotate")
	FFloatCameraParameter DeactivationThreshold = 0.01f;

	/** The interpolation for re-alignment. */
	UPROPERTY(EditAnywhere, Category="Auto-Rotate")
	TObjectPtr<UCameraValueInterpolator> Interpolator;

	/** Whether to suggest freezing the input control rotation. */
	UPROPERTY(EditAnywhere, Category="Auto-Rotate")
	FBooleanCameraParameter FreezeControlRotation = false;

	/** Whether to enable auto-rotation. */
	UPROPERTY(EditAnywhere, Category="Auto-Rotate Toggle")
	FBooleanCameraParameter EnableAutoRotate = true;

	/** Whether to auto-rotate yaw. */
	UPROPERTY(EditAnywhere, Category="Auto-Rotate Toggle")
	FBooleanCameraParameter AutoRotateYaw = true;

	/** Whether to auto-rotate pitch. */
	UPROPERTY(EditAnywhere, Category="Auto-Rotate Toggle")
	FBooleanCameraParameter AutoRotatePitch = true;

	/** The underlying input node. */
	UPROPERTY()
	TObjectPtr<UInput2DCameraNode> InputNode;

public:

	UAutoRotateInput2DCameraNode(const FObjectInitializer& ObjInit);
	
protected:

	// UCameraNode interface.
	virtual FCameraNodeChildrenView OnGetChildren() override;
	virtual void OnBuild(FCameraRigBuildContext& BuildContext) override;
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
};

