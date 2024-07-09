// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"

class UCameraRigAsset;

namespace UE::Cameras
{

class FCameraEvaluationContext;
class FCameraNodeEvaluator;
struct FCameraNodeEvaluationResult;

/**
 * A structure describing an active camera rig being evaluated, generally
 * inside a blend stack.
 */
struct FCameraRigEvaluationInfo
{
	/** The camera rig being evaluated. */
	const UCameraRigAsset* CameraRig = nullptr;
	/** The context inside which the evaluation occurs. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;
	/** The last evaluated result for this camera rig. */
	const FCameraNodeEvaluationResult* LastResult = nullptr;
	/** The root node evaluator of the camera rig. */
	const FCameraNodeEvaluator* RootEvaluator;
	/** Whether the camera rig is frozen. */
	bool bIsFrozen = false;

	FCameraRigEvaluationInfo()
	{}

	FCameraRigEvaluationInfo(
			const UCameraRigAsset* InCameraRig,
			TSharedPtr<const FCameraEvaluationContext> InEvaluationContext,
			const FCameraNodeEvaluationResult& InLastResult,
			const FCameraNodeEvaluator* InRootEvaluator)
		: CameraRig(InCameraRig)
		, EvaluationContext(InEvaluationContext)
		, LastResult(&InLastResult)
		, RootEvaluator(InRootEvaluator)
	{}

	/** 
	 * Whether this evaluation info is valid.
	 *
	 * A valid evaluation info may not have a valid CameraRig, EvaluationContext, or RootEvaluator
	 * if the related camera rig was frozen. However, it would always have a valid LastResult.
	 */
	bool IsValid() const
	{
		return LastResult != nullptr;
	}
};

}  // namespace UE::Cameras

