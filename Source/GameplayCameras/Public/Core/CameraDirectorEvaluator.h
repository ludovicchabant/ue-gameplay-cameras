// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraObjectRtti.h"
#include "CoreTypes.h"
#include "Templates/UniquePtr.h"
#include "UObject/ObjectPtr.h"

class FCameraDirectorEvaluator;
class FCameraDirectorEvaluatorStorage;
class UCameraDirector;
class UCameraEvaluationContext;
class UCameraRigAsset;

/**
 * Parameter structure for running a camera director.
 */
struct FCameraDirectorEvaluationParams
{
	/** Time interval for the update. */
	float DeltaTime = 0.f;

	/** The context in which this director runs. */
	TObjectPtr<const UCameraEvaluationContext> OwnerContext;
};

/**
 * Result structure for running a camera director.
 */
struct FCameraDirectorEvaluationResult
{
	/** The camera rig(s) that the director says should be active this frame. */
	TArray<TObjectPtr<const UCameraRigAsset>, TInlineAllocator<2>> ActiveCameraRigs;
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

private:

	template<typename EvaluatorType, typename ...ArgTypes>
	EvaluatorType* BuildEvaluator(ArgTypes&&... InArgs);

	TSharedPtr<FCameraDirectorEvaluator> Evaluator;

	friend struct FCameraDirectorEvaluatorBuilder;
};

/**
 * Base class for camera director evaluators.
 */
class FCameraDirectorEvaluator
{
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI_BASE(FCameraDirectorEvaluator)

public:

	FCameraDirectorEvaluator();
	virtual ~FCameraDirectorEvaluator() {}
	
	/** Runs the camera director to determine what camera rig(s) should be active this frame. */
	void Run(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult);

	/** Gets the camera director. */
	template<typename CameraDirectorType>
	const CameraDirectorType* GetCameraDirectorAs() const
	{
		return Cast<CameraDirectorType>(PrivateCameraDirector);
	}

public:

	// Internal API.
	void SetPrivateCameraDirector(TObjectPtr<const UCameraDirector> InCameraDirector);

protected:

	/** Runs the camera director to determine what camera rig(s) should be active this frame. */
	virtual void OnRun(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult) {}

private:

	/** The camera director this evaluator is running. */
	TObjectPtr<const UCameraDirector> PrivateCameraDirector;
};

// Utility macros for declaring and defining camera director evaluators.
//
#define UE_DECLARE_CAMERA_DIRECTOR_EVALUATOR(ClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ClassName, FCameraDirectorEvaluator)

#define UE_DECLARE_CAMERA_DIRECTOR_EVALUATOR_EX(ClassName, BaseClassName)\
	UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ClassName, BaseClassName)

#define UE_DEFINE_CAMERA_DIRECTOR_EVALUATOR(ClassName)\
	UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(ClassName)

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

