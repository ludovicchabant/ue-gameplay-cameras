// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"

#include "RootCameraNode.generated.h"

class UCameraEvaluationContext;
class UCameraRigAsset;
class UCameraSystemEvaluator;

/**
 * Defines evaluation layers for camera rigs.
 */
UENUM()
enum class ECameraRigLayer
{
	Base,
	Main,
	Global,
	Visual,
	User0,
	User1,
	User2
};
ENUM_CLASS_FLAGS(ECameraRigLayer)

/**
 * Parameter structure for activating a new camera rig.
 */
struct FActivateCameraRigParams
{
	/** The evaluator currently running.*/
	TObjectPtr<UCameraSystemEvaluator> Evaluator;

	/** The evaluation context in which the camera rig runs. */
	TObjectPtr<const UCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset that will be instantiated. */
	TObjectPtr<const UCameraRigAsset> CameraRig;

	/** The evaluation layer on which to instantiate the camera rig. */
	ECameraRigLayer Layer = ECameraRigLayer::Main;
};

/**
 * The base class for a camera node that can act as the root of the
 * camera system evaluation.
 */
UCLASS(MinimalAPI, Abstract)
class URootCameraNode : public UCameraNode
{
	GENERATED_BODY()
};

/**
 * Base class for the evaluator of a root camera node.
 */
class FRootCameraNodeEvaluator : public FCameraNodeEvaluator
{
public:

	/** Activates a camera rig. */
	void ActivateCameraRig(const FActivateCameraRigParams& Params);

private:

	/** Activates a camera rig. */
	virtual void OnActivateCameraRig(const FActivateCameraRigParams& Params) {}
};

