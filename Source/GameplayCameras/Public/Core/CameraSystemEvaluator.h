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

class UCameraDirector;
class UCameraRigAsset;
class UCanvas;
class URootCameraNode;
struct FMinimalViewInfo;

namespace UE::Cameras
{

class FAutoResetCameraVariableService;
class FCameraEvaluationContext;
class FCameraEvaluationService;
class FCameraRigCombinationRegistry;
class FRootCameraNodeEvaluator;
enum class ECameraEvaluationServiceFlags;
struct FRootCameraNodeCameraRigEvent;

#if UE_GAMEPLAY_CAMERAS_DEBUG
class FRootCameraDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

/**
 * Parameter structure for initializing a new camera system evaluator.
 */
struct GAMEPLAYCAMERAS_API FCameraSystemEvaluatorCreateParams
{
	/** The owner of the camera system, if any. */
	TObjectPtr<UObject> Owner;

	/** An optional factory for creating the root node. */
	using FRootNodeFactory = TFunction<URootCameraNode*()>;
	FRootNodeFactory RootNodeFactory;
};

/**
 * Parameter structure for updating the camera system.
 */
struct GAMEPLAYCAMERAS_API FCameraSystemEvaluationParams
{
	/** Time interval for the update. */
	float DeltaTime = 0.f;
};

/**
 * Result structure for updating the camera system.
 */
struct FCameraSystemEvaluationResult
{
	/** The result camera pose. */
	FCameraPose CameraPose;

	/** The result camera variable table. */
	FCameraVariableTable VariableTable;

	/** Whether this evaluation was a camera cut. */
	bool bIsCameraCut = false;

	/** Whether this result is valid. */
	bool bIsValid = false;

public:

	/** Reset this result to its default (non-valid) state.  */
	void Reset();

	/** Set this result to be equivalent to the given evaluation result. */
	void Reset(const FCameraNodeEvaluationResult& NodeResult);
};

#if UE_GAMEPLAY_CAMERAS_DEBUG
struct FCameraSystemDebugUpdateParams
{
	UCanvas* CanvasObject = nullptr;
};
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

/**
 * The main camera system evaluator class.
 */
class FCameraSystemEvaluator : public TSharedFromThis<FCameraSystemEvaluator>
{
public:

	/** Builds a new camera system. Initialize must be called before the system is used. */
	GAMEPLAYCAMERAS_API FCameraSystemEvaluator();

	/** Initializes the camera system. */
	GAMEPLAYCAMERAS_API void Initialize(TObjectPtr<UObject> InOwner = nullptr);
	/** Initializes the camera system. */
	GAMEPLAYCAMERAS_API void Initialize(const FCameraSystemEvaluatorCreateParams& Params);

	GAMEPLAYCAMERAS_API ~FCameraSystemEvaluator();

public:

	/** Gets the owner of this camera system, if any, and if still valid. */
	UObject* GetOwner() const { return WeakOwner.Get(); }

public:

	/** Push a new evaluation context on the stack. */
	GAMEPLAYCAMERAS_API void PushEvaluationContext(TSharedRef<FCameraEvaluationContext> EvaluationContext);
	/** Remove an existing evaluation context from the stack. */
	GAMEPLAYCAMERAS_API void RemoveEvaluationContext(TSharedRef<FCameraEvaluationContext> EvaluationContext);
	/** Pop the active (top) evaluation context from the stack. */
	GAMEPLAYCAMERAS_API void PopEvaluationContext();

	/** Gets the context stack. */
	FCameraEvaluationContextStack& GetEvaluationContextStack() { return ContextStack; }
	/** Gets the context stack. */
	const FCameraEvaluationContextStack& GetEvaluationContextStack() const { return ContextStack; }

public:

	/** Registers an evaluation service on this camera system. */
	GAMEPLAYCAMERAS_API void RegisterEvaluationService(TSharedRef<FCameraEvaluationService> EvaluationService);
	/** Unregisters an evaluation service from this camera system. */
	GAMEPLAYCAMERAS_API void UnregisterEvaluationService(TSharedRef<FCameraEvaluationService> EvaluationService);
	/** Get currently registered evaluation services. */
	GAMEPLAYCAMERAS_API void GetEvaluationServices(TArray<TSharedPtr<FCameraEvaluationService>>& OutEvaluationServices) const;

	/** Finds an evaluation service of the given type. */
	GAMEPLAYCAMERAS_API TSharedPtr<FCameraEvaluationService> FindEvaluationService(const FCameraObjectTypeID& TypeID) const;
	
	/** Finds an evaluation service of the given type. */
	template<typename EvaluationServiceType>
	TSharedPtr<EvaluationServiceType> FindEvaluationService() const
	{
		TSharedPtr<FCameraEvaluationService> EvaluationService = FindEvaluationService(EvaluationServiceType::StaticTypeID());
		if (EvaluationService)
		{
			return StaticCastSharedPtr<EvaluationServiceType>(EvaluationService);
		}
		return nullptr;
	}

public:

	/** Run an update of the camera system. */
	GAMEPLAYCAMERAS_API void Update(const FCameraSystemEvaluationParams& Params);

	/** Returns the root node evaluator. */
	FRootCameraNodeEvaluator* GetRootNodeEvaluator() const { return RootEvaluator; }

	/** Gets the evaluated result. */
	const FCameraSystemEvaluationResult& GetEvaluatedResult() const { return Result; }

	/** Get the last evaluated camera. */
	GAMEPLAYCAMERAS_API void GetEvaluatedCameraView(FMinimalViewInfo& DesiredView);

	/** Collect reference objects for the garbage collector. */
	GAMEPLAYCAMERAS_API void AddReferencedObjects(FReferenceCollector& Collector);

#if UE_GAMEPLAY_CAMERAS_DEBUG
	GAMEPLAYCAMERAS_API void DebugUpdate(const FCameraSystemDebugUpdateParams& Params);
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	void PreUpdateServices(float DeltaTime, ECameraEvaluationServiceFlags ExtraFlags);
	void PostUpdateServices(float DeltaTime, ECameraEvaluationServiceFlags ExtraFlags);

	void NotifyRootCameraNodeEvent(const FRootCameraNodeCameraRigEvent& InEvent);

private:

	/** The owner (if any) of this camera system evaluator. */
	TWeakObjectPtr<> WeakOwner;

	/** The root camera node. */
	TObjectPtr<URootCameraNode> RootNode;

	/** The stack of active evaluation context. */
	FCameraEvaluationContextStack ContextStack;

	/** The list of evaluation services. */
	TArray<TSharedPtr<FCameraEvaluationService>> EvaluationServices;

	/** Quick access to the variable auto-reset service. */
	TSharedPtr<FAutoResetCameraVariableService> VariableAutoResetService;

	/** Registry for programmatically building combinations of camera rigs. */
	TSharedPtr<FCameraRigCombinationRegistry> CameraRigCombinationRegistry;

	/** Storage buffer for the root evaluator. */
	FCameraNodeEvaluatorStorage RootEvaluatorStorage;

	/** The root evaluator. */
	FRootCameraNodeEvaluator* RootEvaluator = nullptr;

	/** The current result of the root camera node. */
	FCameraNodeEvaluationResult RootNodeResult;

	/** The current overall result of the camera system. */
	FCameraSystemEvaluationResult Result;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	/** Storage for debug drawing blocks. */
	FCameraDebugBlockStorage DebugBlockStorage;

	/** The root debug drawing block. */
	FRootCameraDebugBlock* RootDebugBlock = nullptr;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

	friend class FRootCameraNodeEvaluator;
};

}  // namespace UE::Cameras

