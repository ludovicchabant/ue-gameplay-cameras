// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluatorStorage.h"

namespace UE::Cameras
{

/** Structure for building camera node evaluators. */
struct FCameraNodeEvaluatorBuilder
{
	FCameraNodeEvaluatorBuilder(FCameraNodeEvaluatorStorage& InStorage)
		: Storage(InStorage)
	{}

	/** Builds a camera node evaluator of a given type. */
	template<typename EvaluatorType, typename ...ArgTypes>
	EvaluatorType* BuildEvaluator(ArgTypes&&... InArgs);

private:

	FCameraNodeEvaluatorStorage& Storage;
};

template<typename EvaluatorType, typename ...ArgTypes>
EvaluatorType* FCameraNodeEvaluatorBuilder::BuildEvaluator(ArgTypes&&... InArgs)
{
	return Storage.BuildEvaluator<EvaluatorType>(Forward<ArgTypes>(InArgs)...);
}

}  // namespace UE::Cameras

