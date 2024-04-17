// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectPtr.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraObjectRtti.h"
#include "Templates/SharedPointer.h"

class APlayerController;
class UCameraAsset;
class UCameraDirector;

namespace UE::Cameras
{

struct FCameraEvaluationContextInitializeParams
{
	TObjectPtr<const UCameraAsset> CameraAsset;
	TObjectPtr<APlayerController> PlayerController;
};

struct FCameraEvaluationContextActivateParams
{
};

struct FCameraEvaluationContextDeactivateParams
{
};

/**
 * Base class for providing a context to running camera rigs.
 */
class FCameraEvaluationContext : public TSharedFromThis<FCameraEvaluationContext>
{
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI_BASE(GAMEPLAYCAMERAS_API, FCameraEvaluationContext)

public:

	/** Constructs an evaluation context. */
	GAMEPLAYCAMERAS_API FCameraEvaluationContext();

	GAMEPLAYCAMERAS_API void Initialize(const FCameraEvaluationContextInitializeParams& Params);

	/** Destroys this evaluation context. */
	GAMEPLAYCAMERAS_API virtual ~FCameraEvaluationContext();

	/**
	 * Gets the player controller (if any) in control of the cameras running inside
	 * of this evaluation context.
	 */
	APlayerController* GetPlayerController() const { return PlayerController; }

	/** Gets the camera asset that is hosted in this context. */
	const UCameraAsset* GetCameraAsset() const { return CameraAsset; }

	/** Gets the initial evaluation result for all camera rigs in this context. */
	const FCameraNodeEvaluationResult& GetInitialResult() const { return InitialResult; }

	/** Gets the initial evaluation result for all camera rigs in this context. */
	FCameraNodeEvaluationResult& GetInitialResult() { return InitialResult; }

	FCameraDirectorEvaluator* GetDirectorEvaluator() const { return DirectorEvaluator; }

	TArrayView<const TSharedPtr<FCameraEvaluationContext>> GetChildrenContexts() const { return ChildrenContexts; }

public:

	void Activate(const FCameraEvaluationContextActivateParams& Params);
	void Deactivate(const FCameraEvaluationContextDeactivateParams& Params);

public:

	// Internal API.
	void AddReferencedObjects(FReferenceCollector& Collector);

	bool RegisterChildContext(TSharedRef<FCameraEvaluationContext> ChildContext);
	bool UnregisterChildContext(TSharedRef<FCameraEvaluationContext> ChildContext);

protected:

	virtual void OnActivate(const FCameraEvaluationContextActivateParams& Params) {}
	virtual void OnDeactivate(const FCameraEvaluationContextDeactivateParams& Params) {}

	void AutoCreateDirectorEvaluator();

protected:

	/**
	 * The player controller (if any) in control of the cameras running inside
	 * of this evaluation context.
	 */
	TObjectPtr<APlayerController> PlayerController;

	/** The camera asset hosted in this context. */
	TObjectPtr<const UCameraAsset> CameraAsset;

	/** The initial result for all camera rigs in this context. */
	FCameraNodeEvaluationResult InitialResult;

private:

	FCameraDirectorEvaluatorStorage DirectorEvaluatorStorage;
	FCameraDirectorEvaluator* DirectorEvaluator = nullptr;

	TWeakPtr<FCameraEvaluationContext> WeakParent;

	using FChildrenContexts = TArray<TSharedPtr<FCameraEvaluationContext>>;
	FChildrenContexts ChildrenContexts;

	bool bInitialized = false;
	bool bActivated = false;
};

}  // namespace UE::Cameras

// Utility macros for declaring and defining camera evaluation contexts.
//
#define UE_DECLARE_CAMERA_EVALUATION_CONTEXT(ApiDeclSpec, ClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ApiDeclSpec, ClassName, ::UE::Cameras::FCameraEvaluationContext)

#define UE_DECLARE_CAMERA_EVALUATION_CONTEXT_EX(ApiDeclSpec, ClassName, BaseClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ApiDeclSpec, ClassName, BaseClassName)

#define UE_DEFINE_CAMERA_EVALUATION_CONTEXT(ClassName)\
	UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(ClassName)

