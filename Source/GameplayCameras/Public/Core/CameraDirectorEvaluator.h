// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraObjectRtti.h"
#include "CoreTypes.h"
#include "GameplayCameras.h"
#include "Templates/UniquePtr.h"
#include "UObject/ObjectPtr.h"

class UCameraDirector;
class UCameraRigAsset;

namespace UE::Cameras
{

class FCameraDirectorEvaluator;
class FCameraDirectorEvaluatorStorage;
class FCameraEvaluationContext;

struct FCameraDirectorInitializeParams
{
	TSharedPtr<FCameraEvaluationContext> OwnerContext;
	TObjectPtr<const UCameraDirector> CameraDirector;
};

/**
 * Parameter structure for running a camera director.
 */
struct FCameraDirectorEvaluationParams
{
	/** Time interval for the update. */
	float DeltaTime = 0.f;

	/** The context in which this director runs. */
	TSharedPtr<const FCameraEvaluationContext> OwnerContext;
};

struct FActiveCameraRigInfo
{
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;
	TObjectPtr<const UCameraRigAsset> CameraRig;
};

/**
 * Result structure for running a camera director.
 */
struct FCameraDirectorEvaluationResult
{
	using FActiveCameraRigInfos = TArray<FActiveCameraRigInfo, TInlineAllocator<2>>;
	/** The camera rig(s) that the director says should be active this frame. */
	FActiveCameraRigInfos ActiveCameraRigs;

	void Add(TSharedPtr<const FCameraEvaluationContext> InContext, TObjectPtr<const UCameraRigAsset> InCameraRig)
	{
		ActiveCameraRigs.Add(FActiveCameraRigInfo{ InContext, InCameraRig });
	}
};

/**
 * Structure for building director evaluators.
 */
struct FCameraDirectorEvaluatorBuilder
{
	FCameraDirectorEvaluatorBuilder(FCameraDirectorEvaluatorStorage& InStorage)
		: Storage(InStorage)
	{}

	/** Builds a director evaluator of the given type. */
	template<typename EvaluatorType, typename ...ArgTypes>
	EvaluatorType* BuildEvaluator(ArgTypes&&... InArgs);

private:

	FCameraDirectorEvaluatorStorage& Storage;
};

/**
 * Storage for a director evaluator.
 */
class FCameraDirectorEvaluatorStorage
{
public:

	/** Gets the stored evaluator, if any. */
	FCameraDirectorEvaluator* GetEvaluator() const { return Evaluator.Get(); }

	template<typename EvaluatorType, typename ...ArgTypes>
	EvaluatorType* BuildEvaluator(ArgTypes&&... InArgs);

	void DestroyEvaluator();

private:

	TSharedPtr<FCameraDirectorEvaluator> Evaluator;

	friend struct FCameraDirectorEvaluatorBuilder;
};

/**
 * Base class for camera director evaluators.
 */
class FCameraDirectorEvaluator
{
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI_BASE(GAMEPLAYCAMERAS_API, FCameraDirectorEvaluator)

public:

	GAMEPLAYCAMERAS_API FCameraDirectorEvaluator();
	GAMEPLAYCAMERAS_API virtual ~FCameraDirectorEvaluator() {}

	void Initialize(const FCameraDirectorInitializeParams& Params);
	
	/** Runs the camera director to determine what camera rig(s) should be active this frame. */
	void Run(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult);

	const UCameraDirector* GetCameraDirector() const { return PrivateCameraDirector; }

	/** Gets the camera director. */
	template<typename CameraDirectorType>
	const CameraDirectorType* GetCameraDirectorAs() const
	{
		return Cast<CameraDirectorType>(PrivateCameraDirector);
	}

	GAMEPLAYCAMERAS_API bool AddChildEvaluationContext(TSharedRef<FCameraEvaluationContext> InContext);
	GAMEPLAYCAMERAS_API bool RemoveChildEvaluationContext(TSharedRef<FCameraEvaluationContext> InContext);

	void AddReferencedObjects(FReferenceCollector& Collector);

public:

	// Internal API.
	void SetPrivateCameraDirector(const UCameraDirector* InCameraDirector);

protected:

	enum class EChildContextManipulationResult
	{
		Failure,
		Success,
		ChildContextSuccess
	};

	struct FChildContextManulationParams
	{
		TSharedPtr<FCameraEvaluationContext> ParentContext;
		TSharedPtr<FCameraEvaluationContext> ChildContext;
	};

	struct FChildContextManulationResult
	{
		EChildContextManipulationResult Result = EChildContextManipulationResult::Failure;
#if UE_GAMEPLAY_CAMERAS_DEBUG
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
	};

	virtual void OnInitialize(const FCameraDirectorInitializeParams& Params) {}

	/** Runs the camera director to determine what camera rig(s) should be active this frame. */
	virtual void OnRun(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult) {}

	virtual void OnAddChildEvaluationContext(const FChildContextManulationParams& Params, FChildContextManulationResult& Result) {}
	virtual void OnRemoveChildEvaluationContext(const FChildContextManulationParams& Params, FChildContextManulationResult& Result) {}

	virtual void OnAddReferencedObjects(FReferenceCollector& Collector) {}

private:

	TWeakPtr<FCameraEvaluationContext> WeakOwnerContext;

	/** The camera director this evaluator is running. */
	TObjectPtr<const UCameraDirector> PrivateCameraDirector;
};

template<typename EvaluatorType, typename ...ArgTypes>
EvaluatorType* FCameraDirectorEvaluatorBuilder::BuildEvaluator(ArgTypes&&... InArgs)
{
	return Storage.BuildEvaluator<EvaluatorType>(Forward<ArgTypes>(InArgs)...);
}

template<typename EvaluatorType, typename ...ArgTypes>
EvaluatorType* FCameraDirectorEvaluatorStorage::BuildEvaluator(ArgTypes&&... InArgs)
{
	// We should only build one evaluator.
	ensure(Evaluator == nullptr);
	Evaluator = MakeShared<EvaluatorType>(Forward<ArgTypes>(InArgs)...);
	return Evaluator->CastThisChecked<EvaluatorType>();
}

}  // namespace UE::Cameras

// Typedef to avoid having to deal with namespaces in UCameraNode subclasses.
using FCameraDirectorEvaluatorPtr = UE::Cameras::FCameraDirectorEvaluator*;

// Utility macros for declaring and defining camera director evaluators.
//
#define UE_DECLARE_CAMERA_DIRECTOR_EVALUATOR(ApiDeclSpec, ClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ApiDeclSpec, ClassName, ::UE::Cameras::FCameraDirectorEvaluator)

#define UE_DECLARE_CAMERA_DIRECTOR_EVALUATOR_EX(ApiDeclSpec, ClassName, BaseClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ApiDeclSpec, ClassName, BaseClassName)

#define UE_DEFINE_CAMERA_DIRECTOR_EVALUATOR(ClassName)\
	UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(ClassName)

