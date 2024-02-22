// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

#include "CameraNodeEvaluatorFwd.generated.h"

namespace UE::Cameras
{

class FCameraNodeEvaluator;
class FCameraNodeEvaluatorStorage;
struct FCameraNodeEvaluatorBuilder;

}  // namespace UE::Cameras

// Typedef to avoid having to deal with namespaces in UCameraNode subclasses.
using FCameraNodeEvaluatorPtr = UE::Cameras::FCameraNodeEvaluator*;

/** Allocation information for a node evaluator. */
USTRUCT()
struct FCameraNodeEvaluatorAllocationInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int16 Sizeof = 0;

	UPROPERTY()
	int16 Alignof = 0;
};

/** Allocation information for a node evaluator, auto-setup for a given type. */
template<typename EvaluatorType>
struct TCameraNodeEvaluatorAllocationInfo : FCameraNodeEvaluatorAllocationInfo
{
	TCameraNodeEvaluatorAllocationInfo() 
	{
		Sizeof = sizeof(EvaluatorType);
		Alignof = alignof(EvaluatorType);
	}
};

