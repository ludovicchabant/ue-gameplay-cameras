// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraRigEvaluationInfo.h"
#include "Core/CameraObjectRtti.h"
#include "Core/CameraPose.h"
#include "Core/CameraRigJoints.h"
#include "Core/CameraVariableTable.h"
#include "Core/ObjectChildrenView.h"
#include "CoreTypes.h"
#include "Debug/RootCameraDebugBlock.h"
#include "GameplayCameras.h"
#include "UObject/ObjectPtr.h"

class FReferenceCollector;
class UCameraNode;
class UCameraRigAsset;

namespace UE::Cameras
{

class FCameraEvaluationContext;
class FCameraNodeEvaluator;
class FCameraSystemEvaluator;
struct FCameraOperation;
struct FCameraNodeEvaluationParams;
struct FCameraNodeEvaluatorBuilder;
struct FCameraRigEvaluationInfo;

#if UE_GAMEPLAY_CAMERAS_DEBUG
struct FCameraDebugBlockBuilder;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

/**
 * Flags describing the needs of a camera node evaluator.
 */
enum class ECameraNodeEvaluatorFlags
{
	None = 0,
	NeedsParameterUpdate = 1 << 0,
	NeedsEvaluationUpdate = 1 << 1,
	SupportsOperations = 1 << 2
};
ENUM_CLASS_FLAGS(ECameraNodeEvaluatorFlags)

/** View on a camera node evaluator's children. */
using FCameraNodeEvaluatorChildrenView = TObjectChildrenView<FCameraNodeEvaluator*>;

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
	FCameraSystemEvaluator* Evaluator = nullptr;
	/** The evaluation context (if any) responsible for this branch of the evaluation. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/**
	 * Information about the last active camera rig if the node tree being initialized
	 * is being pushed on top of a non-empty blend stack.
	 */
	FCameraRigEvaluationInfo LastActiveCameraRigInfo;
};

/**
 * Parameter structure for updating the pre-blended parameters of a camera node.
 */
struct FCameraBlendedParameterUpdateParams
{
	FCameraBlendedParameterUpdateParams(const FCameraNodeEvaluationParams& InEvaluationParams, const FCameraPose& InLastCameraPose)
		: EvaluationParams(InEvaluationParams)
		, LastCameraPose(InLastCameraPose)
	{}

	/** Information about the evaluation pass that will happen afterwards. */
	const FCameraNodeEvaluationParams& EvaluationParams;
	/** Last frame's camera pose. */
	const FCameraPose& LastCameraPose;
};

/**
 * Result of updating the pre-blended parameters of a camera node.
 */
struct FCameraBlendedParameterUpdateResult
{
	FCameraBlendedParameterUpdateResult(FCameraVariableTable& InVariableTable)
		: VariableTable(InVariableTable)
	{}

	/** Variable table in which parameters should be stored or obtained. */
	FCameraVariableTable& VariableTable;
};

/** The type of evaluation being run. */
enum class ECameraNodeEvaluationType
{
	/** Normal evaluation. */
	Standard,
	/** Evaluation for IK aiming. */
	IK
};

/**
 * Parameter structure for running a camera node evaluator.
 */
struct FCameraNodeEvaluationParams
{
	/** The evaluator running this evaluation. */
	FCameraSystemEvaluator* Evaluator = nullptr;
	/** The evaluation context (if any) responsible for this branch of the evaluation. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;
	/** The time interval for the evaluation. */
	float DeltaTime = 0.f;
	/** The type of evaluation being run. */
	ECameraNodeEvaluationType EvaluationType = ECameraNodeEvaluationType::Standard;
	/** Whether this is the first evaluation of this camera node hierarchy. */
	bool bIsFirstFrame = false;
};

/**
 * Input/output result structure for running a camera node evaluator.
 */
struct GAMEPLAYCAMERAS_API FCameraNodeEvaluationResult
{
	/** The camera pose. */
	FCameraPose CameraPose;

	/** The variable table. */
	FCameraVariableTable VariableTable;

	/** The list of joints in the current camera rig. */
	FCameraRigJoints CameraRigJoints;

	/** Whether the current frame is a camera cut. */
	bool bIsCameraCut = false;
	/** Whether this result is valid. */
	bool bIsValid = false;

public:

	/** Reset this result to its default (non-valid) state.  */
	void Reset(bool bResetVariableTable);

	/** Serializes this result to the given archive. */
	void Serialize(FArchive& Ar);
};

/**
 * Parameter structure for executing camera operations.
 */
struct FCameraOperationParams
{
	/** The evaluator running this operation. */
	FCameraSystemEvaluator* Evaluator = nullptr;
	/** The evaluation context (if any) responsible for this branch of the evaluation. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;
};

/**
 * Parameter structure for serializing the state of a camera node evaluator.
 */
struct GAMEPLAYCAMERAS_API FCameraNodeEvaluatorSerializeParams
{
};

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
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI_BASE(GAMEPLAYCAMERAS_API, FCameraNodeEvaluator)

public:

	GAMEPLAYCAMERAS_API FCameraNodeEvaluator();
	GAMEPLAYCAMERAS_API virtual ~FCameraNodeEvaluator() {}

	/** Called to build any children evaluators. */
	void Build(const FCameraNodeEvaluatorBuildParams& Params);

	/** Initialize this evaluator. */
	void Initialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult);

	/** Get the list of children under this evaluator. */
	FCameraNodeEvaluatorChildrenView GetChildren();

	/** Called to update and store the blended parameters for this node. */
	void UpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult);

	/** Run this evaluator. */
	void Run(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);

	/** Execute an IK operation. */
	void ExecuteOperation(const FCameraOperationParams& Params, FCameraOperation& Operation);

	/** Collect referenced UObjects. */
	void AddReferencedObjects(FReferenceCollector& Collector);

	/** Serializes the state of this evaluator. */
	void Serialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar);

	/** Gets the flags for this evaluator. */
	ECameraNodeEvaluatorFlags  GetNodeEvaluatorFlags() const { return PrivateFlags; }

	/** Get the camera node. */
	const UCameraNode* GetCameraNode() const { return PrivateCameraNode; }

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

	/** Sets the flags for this evaluator. */
	void SetNodeEvaluatorFlags(ECameraNodeEvaluatorFlags InFlags);

protected:

	/** Called to build any children evaluators. */
	GAMEPLAYCAMERAS_API virtual void OnBuild(const FCameraNodeEvaluatorBuildParams& Params) {}

	/** Initialize this evaluator. */
	GAMEPLAYCAMERAS_API virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) {}

	/** Get the list of children under this evaluator. */
	GAMEPLAYCAMERAS_API virtual FCameraNodeEvaluatorChildrenView OnGetChildren() { return FCameraNodeEvaluatorChildrenView(); }

	/** Called to update and store the blended parameters for this node. */
	GAMEPLAYCAMERAS_API virtual void OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult) {}

	/** Run this evaluator. */
	GAMEPLAYCAMERAS_API virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) {}

	/** Execute an IK operation. */
	GAMEPLAYCAMERAS_API virtual void OnExecuteOperation(const FCameraOperationParams& Params, FCameraOperation& Operation) {}

	/** Collect referenced UObjects. */
	GAMEPLAYCAMERAS_API virtual void OnAddReferencedObjects(FReferenceCollector& Collector) {}

	/** Serializes the state of this evaluator. */
	GAMEPLAYCAMERAS_API virtual void OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar) {}

#if UE_GAMEPLAY_CAMERAS_DEBUG
	/** Called to create debug blocks for this node evaluator. */
	GAMEPLAYCAMERAS_API virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder);
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	/** The camera node to run. */
	TObjectPtr<const UCameraNode> PrivateCameraNode;

	/** The flags for this evaluator. */
	ECameraNodeEvaluatorFlags PrivateFlags = ECameraNodeEvaluatorFlags::NeedsEvaluationUpdate;
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
	if (FCameraNodeEvaluator* NewEvaluator = BuildEvaluator(InNode))
	{
		return NewEvaluator->CastThisChecked<EvaluatorType>();
	}
	return nullptr;
}

}  // namespace UE::Cameras

// Utility macros for declaring and defining camera node evaluators.
//
#define UE_DECLARE_CAMERA_NODE_EVALUATOR(ApiDeclSpec, ClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ApiDeclSpec, ClassName, FCameraNodeEvaluator)

#define UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(ApiDeclSpec, ClassName, BaseClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ApiDeclSpec, ClassName, BaseClassName)

#define UE_DEFINE_CAMERA_NODE_EVALUATOR(ClassName)\
	UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(ClassName)

