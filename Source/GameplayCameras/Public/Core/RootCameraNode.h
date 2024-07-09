// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"

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
class IRootCameraNodeObserver;
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
 * Base class for the evaluator of a root camera node.
 */
class FRootCameraNodeEvaluator : public FCameraNodeEvaluator
{
public:

	/** Activates a camera rig. */
	void ActivateCameraRig(const FActivateCameraRigParams& Params);

	/** Registers an observer to this root node. */
	void RegisterObserver(IRootCameraNodeObserver* Observer);
	/** Unregisters an observer from this root node. */
	void UnregisterObserver(IRootCameraNodeObserver* Observer);

protected:

	// FCameraNodeEvaluator interface.
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) override;

protected:

	/** Activates a camera rig. */
	virtual void OnActivateCameraRig(const FActivateCameraRigParams& Params) {}

	bool HasObservers() const;
	void NotifyObservers(const FRootCameraNodeCameraRigEvent& InEvent) const;

private:

	/** The camera system that owns this root node. */
	FCameraSystemEvaluator* OwningEvaluator = nullptr;

	/** The list of observers. */
	TArray<IRootCameraNodeObserver*> Observers;
};

}  // namespace UE::Cameras

