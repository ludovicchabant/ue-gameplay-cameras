// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraParameters.h"
#include "Core/ShakeCameraNode.h"
#include "UObject/ObjectPtr.h"

#include "CameraAnimationSequenceShakeCameraNode.generated.h"

class UCameraAnimationSequence;

/**
 * A shake node that plays a Camera Animation Sequence.
 */
UCLASS()
class UCameraAnimationSequenceShakeCameraNode : public UShakeCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const;

public:

	/** The camera animation to play. */
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UCameraAnimationSequence> Animation;

	/** How fast to play the animation. */
	UPROPERTY(EditAnywhere, Category=CameraShake, meta=(ClampMin="0.001"))
	FFloatCameraParameter PlayRate = 1.f;

	/** How "intensely" to play the animation. */
	UPROPERTY(EditAnywhere, Category=CameraShake, meta=(ClampMin="0.0"))
	FFloatCameraParameter Scale = 1.f;

	/** Linear blend-in time. */
	UPROPERTY(EditAnywhere, Category=CameraShake, meta=(ClampMin="0.0", Units=s))
	FFloatCameraParameter BlendInTime = 0.2f;

	/** Linear blend-out time. */
	UPROPERTY(EditAnywhere, Category=CameraShake, meta=(ClampMin="0.0", Units=s))
	FFloatCameraParameter BlendOutTime = 0.4f;
};

