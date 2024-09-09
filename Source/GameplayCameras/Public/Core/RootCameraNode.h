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
	Visual UMETA(DisplayName="Visual Layer")
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
	/** The evaluation context in which the camera rig runs. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset that will be instantiated. */
	TObjectPtr<const UCameraRigAsset> CameraRig;

	/** The evaluation layer on which to instantiate the camera rig. */
	ECameraRigLayer Layer = ECameraRigLayer::Main;
};

/**
 * Parameter structure for deaactivating a running camera rig.
 */
struct FDeactivateCameraRigParams
{
	/** The evaluation context in which the camera rig runs. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset that was instantiated. */
	TObjectPtr<const UCameraRigAsset> CameraRig;

	/** The evaluation layer on which the camera rig is running. */
	ECameraRigLayer Layer = ECameraRigLayer::Main;
};

/**
 * Parameter structure for building a single camera rig hierarchy.
 */
struct FSingleCameraRigHierarchyBuildParams
{
	/** The camera rig to build the hierachy for. */
	FCameraRigEvaluationInfo CameraRigInfo;

	/** The name of the range to tag for the camera rig's nodes. */
	FName CameraRigRangeName = TEXT("ActiveCameraRig");
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

	/** Deactivates a camera rig. */
	void DeactivateCameraRig(const FDeactivateCameraRigParams& Params);

	/**
	 * Builds the hierarchy of the system for a given single camera rig.
	 * This is expected to return the nodes of all the layers, except for the main layer which
	 * should only have the nodes of the given camera rig (i.e. it shouldn't have nodes of
	 * other currently active camera rigs).
	 */
	void BuildSingleCameraRigHierarchy(const FSingleCameraRigHierarchyBuildParams& Params, FCameraNodeEvaluatorHierarchy& OutHierarchy);

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
	
	/** Deactivates a camera rig. */
	virtual void OnDeactivateCameraRig(const FDeactivateCameraRigParams& Params) {}

	/* Builds the hierarchy of the system for a given single camera rig. */
	virtual void OnBuildSingleCameraRigHierarchy(const FSingleCameraRigHierarchyBuildParams& Params, FCameraNodeEvaluatorHierarchy& OutHierarchy) {}

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

