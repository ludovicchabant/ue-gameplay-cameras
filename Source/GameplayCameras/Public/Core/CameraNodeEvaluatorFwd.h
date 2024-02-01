// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

class FCameraNodeEvaluator;
class FCameraNodeEvaluatorStorage;
struct FCameraNodeEvaluatorBuilder;

using FCameraNodeEvaluatorPtr = FCameraNodeEvaluator*;

/** Allocation information for a node evaluator. */
struct FCameraNodeEvaluatorAllocationInfo
{
	int16 Sizeof = 0;
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

