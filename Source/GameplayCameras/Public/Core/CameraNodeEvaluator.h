// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraObjectRtti.h"
#include "Core/CameraPose.h"
#include "Core/CameraVariableTable.h"
#include "Core/ObjectChildrenView.h"
#include "CoreTypes.h"
#include "Debug/RootCameraDebugBlock.h"
#include "GameplayCameras.h"
#include "UObject/ObjectPtr.h"

class FReferenceCollector;
class UCameraNode;

namespace UE::Cameras
{

class FCameraEvaluationContext;
class FCameraNodeEvaluator;
class FCameraSystemEvaluator;
struct FCameraNodeEvaluatorBuilder;

#if UE_GAMEPLAY_CAMERAS_DEBUG
struct FCameraDebugBlockBuilder;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

/**
 * Parameter structure for running a camera node evaluator.
 */
struct FCameraNodeEvaluationParams
{
	/** The evaluation running this evaluation. */
	TSharedPtr<FCameraSystemEvaluator> Evaluator;
	/** The evaluation context (if any) responsible for this branch of the evaluation. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;
	/** The time interval for the evaluation. */
	float DeltaTime = 0.f;
	/** Whether this is the first evaluation of this camera node hierarchy. */
	bool bIsFirstFrame = false;
};

/**
 * Input/output result structure for running a camera node evaluator.
 */
struct FCameraNodeEvaluationResult
{
	/** The camera pose. */
	FCameraPose CameraPose;

	/** The variable table. */
	FCameraVariableTable VariableTable;

	/** Whether the current frame is a camera cut. */
	bool bIsCameraCut = false;
	/** Whether this result is valid. */
	bool bIsValid = false;

	/** Reset this result to its default (non-valid) state. */
	void Reset();
};

/**
 * Structure for building the tree of camera node evaluators.
 */
struct FCameraNodeEvaluatorBuildParams
{
	FCameraNodeEvaluatorBuildParams(FCameraNodeEvaluatorBuilder& InBuilder)
		: Builder(InBuilder)
	{}

	/** Builds an evaluator for the given camera node. */
	FCameraNodeEvaluator* BuildEvaluator(const UCameraNode* InNode) const;

	/** Builds an evaluator for the given camera node, and down-cast it to the given type. */
	template<typename EvaluatorType>
	EvaluatorType* BuildEvaluatorAs(const UCameraNode* InNode) const;

private:

	/** Builder object for building children evaluators. */
	FCameraNodeEvaluatorBuilder& Builder;
};

/**
 * Structure for initializing a camera node evaluator.
 */
struct FCameraNodeEvaluatorInitializeParams
{
	/** The evaluation running this evaluation. */
	TSharedPtr<FCameraSystemEvaluator> Evaluator;
	/** The evaluation context (if any) responsible for this branch of the evaluation. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;
};

/** View on a camera node evaluator's children. */
using FCameraNodeEvaluatorChildrenView = TObjectChildrenView<FCameraNodeEvaluator*>;

#if UE_GAMEPLAY_CAMERAS_DEBUG

/**
 * Structure for creating the node evaluator's debug blocks.
 */
struct FCameraDebugBlockBuildParams
{
	// Empty for now, but defined for later API changes.
};

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

/**
 * Base class for objects responsible for running a camera node.
 */
class FCameraNodeEvaluator
{
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI_BASE(FCameraNodeEvaluator)

public:

	FCameraNodeEvaluator();
	virtual ~FCameraNodeEvaluator() {}

	/** Called to build any children evaluators. */
	void Build(const FCameraNodeEvaluatorBuildParams& Params);

	/** Initialize this evaluator. */
	void Initialize(const FCameraNodeEvaluatorInitializeParams& Params);

	/** Get the list of children under this evaluator. */
	FCameraNodeEvaluatorChildrenView GetChildren();

	/** Run this evaluator. */
	void Run(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);

	/** Collect referenced UObjects. */
	void AddReferencedObjects(FReferenceCollector& Collector);

	/** Get the camera node. */
	template<typename CameraNodeType>
	const CameraNodeType* GetCameraNodeAs() const
	{
		return Cast<CameraNodeType>(PrivateCameraNode);
	}

#if UE_GAMEPLAY_CAMERAS_DEBUG
	/** Called to create debug blocks for this node evaluator. */
	void BuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder);
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

public:

	// Internal API.
	void SetPrivateCameraNode(TObjectPtr<const UCameraNode> InCameraNode);

protected:

	/** Called to build any children evaluators. */
	virtual void OnBuild(const FCameraNodeEvaluatorBuildParams& Params) {}

	/** Initialize this evaluator. */
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) {}

	/** Get the list of children under this evaluator. */
	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() { return FCameraNodeEvaluatorChildrenView(); }

	/** Run this evaluator. */
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) {}

	/** Collect referenced UObjects. */
	virtual void OnAddReferencedObjects(FReferenceCollector& Collector) {}

#if UE_GAMEPLAY_CAMERAS_DEBUG
	/** Called to create debug blocks for this node evaluator. */
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder);
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	/** The camera node to run. */
	TObjectPtr<const UCameraNode> PrivateCameraNode;
};

/** Utility base class for camera node evaluators of a specific camera node type. */
template<typename CameraNodeType>
class TCameraNodeEvaluator : public FCameraNodeEvaluator
{
public:

	/** Gets the camera node. */
	const CameraNodeType* GetCameraNode() const
	{
		return GetCameraNodeAs<CameraNodeType>();
	}

	friend CameraNodeType;
};

template<typename EvaluatorType>
EvaluatorType* FCameraNodeEvaluatorBuildParams::BuildEvaluatorAs(const UCameraNode* InNode) const
{
	FCameraNodeEvaluator* NewEvaluator = BuildEvaluator(InNode);
	check(NewEvaluator);
	return NewEvaluator->CastThisChecked<EvaluatorType>();
}

}  // namespace UE::Cameras

// Utility macros for declaring and defining camera node evaluators.
//
#define UE_DECLARE_CAMERA_NODE_EVALUATOR(ClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ClassName, FCameraNodeEvaluator)

#define UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(ClassName, BaseClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ClassName, BaseClassName)

#define UE_DEFINE_CAMERA_NODE_EVALUATOR(ClassName)\
	UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(ClassName)

