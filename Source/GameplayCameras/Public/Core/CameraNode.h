// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraPose.h"
#include "Core/CameraNodeChildrenView.h"
#include "CoreTypes.h"
#include "UObject/Object.h"

#include "CameraNode.generated.h"

/**
 * Parameter structure for running a camera node.
 */
struct FCameraNodeRunParams
{
	/** The time interval for the evaluation. */
	float DeltaTime = 0.f;
	/** Whether this is the first evaluation of this camera node hierarchy. */
	bool bIsFirstFrame = false;
};

/**
 * Input/output result structure for running a camera node.
 */
struct FCameraNodeRunResult
{
	/** The camera pose. */
	FCameraPose CameraPose;
	/** Whether the current frame is a camera cut. */
	bool bIsCameraCut = false;
	/** Whether this result is valid. */
	bool bIsValid = false;

	/** Reset this result to its default (non-valid) state. */
	void Reset();
};

/**
 * The base class for a camera node.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, MinimalAPI)
class UCameraNode : public UObject
{
	GENERATED_BODY()

public:

	/** Get the list of children under this node. */
	FCameraNodeChildrenView GetChildren();

	/** Run this node. */
	void Run(const FCameraNodeRunParams& Params, FCameraNodeRunResult& OutResult);

public:

	// Internal API

	/** Whether this node was instantiated from a source node. */
	bool GetIsInstantiated() const { return bIsInstantiated; }
	/** Flag this node as being instantiated from a source node. */
	void SetIsInstantiated(bool bInValue = true) { bIsInstantiated = bInValue; }

protected:

	/** Get the list of children under this node. */
	virtual FCameraNodeChildrenView OnGetChildren() { return FCameraNodeChildrenView(); }

	/** Run this node. */
	virtual void OnRun(const FCameraNodeRunParams& Params, FCameraNodeRunResult& OutResult) {}

#if WITH_EDITOR

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

public:

	/** Specifies whether this node is enabled. */
	UPROPERTY(EditAnywhere, Category=Common)
	bool bIsEnabled = true;

private:

	bool bIsInstantiated = false;
};

