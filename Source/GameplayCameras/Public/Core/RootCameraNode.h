// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraRigEvaluationInfo.h"

#include "RootCameraNode.generated.h"

class UCameraRigAsset;

/**
 * Defines evaluation layers for camera rigs.
 */
UENUM(BlueprintType)
enum class ECameraRigLayer : uint8
{
	Base UMETA(DisplayName="Base Layer"),
	Main UMETA(DisplayName="Main Layer"),
	Global UMETA(DisplayName="Global Layer"),
	Visual UMETA(DisplayName="Visual Layer"),
	ScratchMain UMETA(DisplayName="Scratch Main Layer"),
	User0,
	User1,
	User2
};
ENUM_CLASS_FLAGS(ECameraRigLayer)

/**
 * The base class for a camera node that can act as the root of the
 * camera system evaluation.
 */
UCLASS(MinimalAPI, Abstract)
class URootCameraNode : public UCameraNode
{
	GENERATED_BODY()
};

namespace UE::Cameras
{

class FCameraEvaluationContext;
class FCameraSystemEvaluator;
struct FRootCameraNodeCameraRigEvent;

/**
 * Parameter structure for activating a new camera rig.
 */
struct FActivateCameraRigParams
{
	/** The evaluator currently running.*/
	FCameraSystemEvaluator* Evaluator = nullptr;

	/** The evaluation context in which the camera rig runs. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset that will be instantiated. */
	TObjectPtr<const UCameraRigAsset> CameraRig;

	/** The evaluation layer on which to instantiate the camera rig. */
	ECameraRigLayer Layer = ECameraRigLayer::Main;
};

/**
 * Parameter structure for evaluating a single camera rig.
 */
struct FSingleCameraRigEvaluationParams
{
	/** The evaluation parameters. */
	FCameraNodeEvaluationParams EvaluationParams;

	/** The camera rig to evaluate. */
	FCameraRigEvaluationInfo CameraRigInfo;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRootCameraNodeCameraRigEvent, const FRootCameraNodeCameraRigEvent&);

/**
 * Base class for the evaluator of a root camera node.
 */
class FRootCameraNodeEvaluator : public FCameraNodeEvaluator
{
public:

	/** Activates a camera rig. */
	void ActivateCameraRig(const FActivateCameraRigParams& Params);

	/**
	 * Evaluates a single camera rig.
	 * This is expected to run all layers as usual, except for the main layer which should
	 * only run the given camera rig instead.
	 */
	void RunSingleCameraRig(const FSingleCameraRigEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);

	/** Gets the delegate for camera rig events. */
	FOnRootCameraNodeCameraRigEvent& OnCameraRigEvent() { return OnCameraRigEventDelegate; }

protected:

	// FCameraNodeEvaluator interface.
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;

protected:

	/** Activates a camera rig. */
	virtual void OnActivateCameraRig(const FActivateCameraRigParams& Params) {}

	/** Evaluates a single camera rig. See comments on RunSingleCameraRig. */
	virtual void OnRunSingleCameraRig(const FSingleCameraRigEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) {}

protected:

	void BroadcastCameraRigEvent(const FRootCameraNodeCameraRigEvent& InEvent) const;

private:

	/** The camera system that owns this root node. */
	FCameraSystemEvaluator* OwningEvaluator = nullptr;

	/** The delegate to notify when an event happens. */
	FOnRootCameraNodeCameraRigEvent OnCameraRigEventDelegate;
};

}  // namespace UE::Cameras

