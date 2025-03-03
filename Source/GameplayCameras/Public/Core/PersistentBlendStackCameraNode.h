// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BlendStackCameraNode.h"

namespace UE::Cameras
{

/**
 * Parameter structure for inserting a camera rig into a persistent blend stack.
 */
struct FBlendStackCameraInsertParams
{
	/** The evaluation context within which a camera rig's node tree should run. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset to instantiate and push on the blend stack. */
	TObjectPtr<const UCameraRigAsset> CameraRig;

	/** A transition to use, instead of looking one up. */
	TObjectPtr<const UCameraRigTransition> TransitionOverride;

	/** Whether to force insert a new instance of the camera rig, even if there is already one in the stack. */
	bool bForceInsert = false;
};

/**
 * Parameter structure for removing a camera rig from a persistent blend stack.
 * The camera rig to remove is identified first by a given ID or, if no ID is given,
 * by finding a camera rig matching the provided EvaluationContext and CameraRig.
 */
struct FBlendStackCameraRemoveParams
{
	/** The ID of the blend stack entry to remove. */
	FBlendStackEntryID EntryID;

	/** The evaluation context within which the camera rig to remove is being run. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset used by the instanced to remove. */
	TObjectPtr<const UCameraRigAsset> CameraRig;
};

/**
 * Evaluator for a persistent blend stack, i.e. a blend stack in which camera rigs blend additively on top of 
 * each other, but without automatically popping out any fully blended-out entries. 
 * This is a stack suitable for a "camera modifier stack" of sorts.
 */
class FPersistentBlendStackCameraNodeEvaluator 
	: public FBlendStackCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(GAMEPLAYCAMERAS_API, FPersistentBlendStackCameraNodeEvaluator, FBlendStackCameraNodeEvaluator)

public:

	/** Insert a new camera rig onto the blend stack. */
	FBlendStackEntryID Insert(const FBlendStackCameraInsertParams& Params);

	/** Remove an existing camera rig from the blend stack. */
	void Remove(const FBlendStackCameraRemoveParams& Params);

protected:

	// FCameraNodeEvaluator interface.
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	// Update methods.
	void InternalUpdate(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);

	// Utility functions for finding an appropriate transition.
	const UCameraRigTransition* FindTransition(const FBlendStackCameraInsertParams& Params) const;
};

}  // namespace UE::Cameras

