// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraDirector.h"
#include "Core/CameraEvaluationContextStack.h"
#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraPose.h"
#include "CoreTypes.h"
#include "Debug/CameraDebugBlockStorage.h"
#include "GameplayCameras.h"
#include "Templates/SharedPointer.h"
#include "UObject/GCObject.h"

class FCanvas;
class UCameraDirector;
class UCameraRigAsset;
class URootCameraNode;
struct FMinimalViewInfo;

namespace UE::Cameras
{

class FCameraEvaluationContext;
class FRootCameraNodeEvaluator;

#if UE_GAMEPLAY_CAMERAS_DEBUG
class FRootCameraDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

/**
 * Parameter structure for initializing a new camera system evaluator.
 */
struct FCameraSystemEvaluatorCreateParams
{
	TObjectPtr<UObject> Owner;

	using FRootNodeFactory = TFunction<URootCameraNode*()>;
	FRootNodeFactory RootNodeFactory;
};

/**
 * Parameter structure for updating the camera system.
 */
struct FCameraSystemEvaluationUpdateParams
{
	/** Time interface for the update. */
	float DeltaTime = 0.f;
};

/**
 * Result structure for updating the camera system.
 */
struct FCameraSystemEvaluationUpdateResult
{
	/** The result camera pose. */
	FCameraPose CameraPose;

	/** Whether this evaluation was a camera cut. */
	bool bIsCameraCut = false;

	/** Whether this result is valid. */
	bool bIsValid = false;
};

#if UE_GAMEPLAY_CAMERAS_DEBUG
struct FCameraSystemDebugUpdateParams
{
	FCanvas* Canvas = nullptr;
};
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

/**
 * The main camera system evaluator class.
 */
class FCameraSystemEvaluator : public TSharedFromThis<FCameraSystemEvaluator>
{
public:

	/** Builds a new camera system. Initialize must be called before the system is used. */
	FCameraSystemEvaluator();

	/** Initializes the camera system. */
	void Initialize(TObjectPtr<UObject> InOwner = nullptr);
	/** Initializes the camera system. */
	void Initialize(const FCameraSystemEvaluatorCreateParams& Params);

public:

	/** Push a new evaluation context on the stack. */
	void PushEvaluationContext(TSharedRef<FCameraEvaluationContext> EvaluationContext);
	/** Remove an existing evaluation context from the stack. */
	void RemoveEvaluationContext(TSharedRef<FCameraEvaluationContext> EvaluationContext);
	/** Pop the active (top) evaluation context from the stack. */
	void PopEvaluationContext();

	/** Gets the context stack. */
	FCameraEvaluationContextStack& GetEvaluationContextStack() { return ContextStack; }
	/** Gets the context stack. */
	const FCameraEvaluationContextStack& GetEvaluationContextStack() const { return ContextStack; }

public:

	/** Run an update of the camera system. */
	void Update(const FCameraSystemEvaluationUpdateParams& Params);

	/** Returns the root node evaluator. */
	FRootCameraNodeEvaluator* GetRootNodeEvaluator() const { return RootEvaluator; }

	/** Gets the evaluated result. */
	const FCameraSystemEvaluationUpdateResult& GetEvaluatedResult() const { return Result; }

	/** Get the last evaluated camera. */
	void GetEvaluatedCameraView(FMinimalViewInfo& DesiredView);

#if UE_GAMEPLAY_CAMERAS_DEBUG
	void DebugUpdate(const FCameraSystemDebugUpdateParams& Params);
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

public:

	void AddReferencedObjects(FReferenceCollector& Collector);

private:

	/** The owner (if any) of this camera system evaluator. */
	TWeakObjectPtr<> WeakOwner;

	/** The root camera node. */
	TObjectPtr<URootCameraNode> RootNode;

	/** The stack of active evaluation context. */
	FCameraEvaluationContextStack ContextStack;

	/** Storage buffer for the root evaluator. */
	FCameraNodeEvaluatorStorage RootEvaluatorStorage;

	/** The root evaluator. */
	FRootCameraNodeEvaluator* RootEvaluator = nullptr;

	/** The current result of the root camera node. */
	FCameraNodeEvaluationResult RootNodeResult;

	/** The current overall result of the camera system. */
	FCameraSystemEvaluationUpdateResult Result;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	/** Storage for debug drawing blocks. */
	FCameraDebugBlockStorage DebugBlockStorage;

	/** The root debug drawing block. */
	FRootCameraDebugBlock* RootDebugBlock = nullptr;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

}  // namespace UE::Cameras

