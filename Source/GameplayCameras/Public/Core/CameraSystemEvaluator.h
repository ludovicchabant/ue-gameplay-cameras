// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraDirector.h"
#include "Core/CameraEvaluationContextStack.h"
#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraPose.h"
#include "CoreTypes.h"
#include "UObject/GCObject.h"

#include "CameraSystemEvaluator.generated.h"

class FRootCameraNodeEvaluator;
class UCameraDirector;
class UCameraEvaluationContext;
class UCameraRigAsset;
class URootCameraNode;
struct FMinimalViewInfo;

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

/**
 * The main camera system evaluator class.
 */
UCLASS(MinimalAPI)
class UCameraSystemEvaluator : public UObject
{
	GENERATED_BODY()

public:

	UCameraSystemEvaluator(const FObjectInitializer& ObjectInit);

public:

	/** Push a new evaluation context on the stack. */
	void PushEvaluationContext(UCameraEvaluationContext* EvaluationContext);
	/** Remove an existing evaluation context from the stack. */
	void RemoveEvaluationContext(UCameraEvaluationContext* EvaluationContext);
	/** Pop the active (top) evaluation context from the stack. */
	void PopEvaluationContext();

public:

	/** Run an update of the camera system. */
	void Update(const FCameraSystemEvaluationUpdateParams& Params);

	/** Get the last evaluated camera. */
	void GetEvaluatedCameraView(FMinimalViewInfo& DesiredView);

public:

	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

private:

	/** The root camera node. */
	UPROPERTY()
	TObjectPtr<URootCameraNode> RootNode;

	/** The stack of active evaluation context. */
	FCameraEvaluationContextStack ContextStack;

	/** Storage buffer for the root evaluator. */
	FCameraNodeEvaluatorStorage RootEvaluatorStorage;

	/** The root evaluator. */
	FRootCameraNodeEvaluator* RootEvaluator;

	/** The current result of the root camera node. */
	FCameraNodeEvaluationResult RootNodeResult;

	/** The current overall result of the camera system. */
	FCameraSystemEvaluationUpdateResult Result;
};

