// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectPtr.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraObjectRtti.h"
#include "Templates/SharedPointer.h"

class UCameraAsset;
class APlayerController;

/**
 * Base class for providing a context to running camera rigs.
 */
class FCameraEvaluationContext : public TSharedFromThis<FCameraEvaluationContext>
{
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI_BASE(FCameraEvaluationContext)

public:

	/** Constructs an evaluation context. */
	FCameraEvaluationContext();

	/** Destroys this evaluation context. */
	virtual ~FCameraEvaluationContext();

	/**
	 * Gets the player controller (if any) in control of the cameras running inside
	 * of this evaluation context.
	 */
	APlayerController* GetPlayerController() const { return PlayerController; }

	/** Gets the camera asset that is hosted in this context. */
	UCameraAsset* GetCameraAsset() const { return CameraAsset; }

	/** Gets the initial evaluation result for all camera rigs in this context. */
	const FCameraNodeEvaluationResult& GetInitialResult() const { return InitialResult; }

public:

	void AddReferencedObjects(FReferenceCollector& Collector);

protected:

	/**
	 * The player controller (if any) in control of the cameras running inside
	 * of this evaluation context.
	 */
	TObjectPtr<APlayerController> PlayerController;

	/** The camera asset hosted in this context. */
	TObjectPtr<UCameraAsset> CameraAsset;

	/** The initial result for all camera rigs in this context. */
	FCameraNodeEvaluationResult InitialResult;
};

// Utility macros for declaring and defining camera evaluation contexts.
//
#define UE_DECLARE_CAMERA_EVALUATION_CONTEXT(ClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ClassName, FCameraEvaluationContext)

#define UE_DECLARE_CAMERA_EVALUATION_CONTEXT_EX(ClassName, BaseClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ClassName, BaseClassName)

#define UE_DEFINE_CAMERA_EVALUATION_CONTEXT(ClassName)\
	UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(ClassName)

