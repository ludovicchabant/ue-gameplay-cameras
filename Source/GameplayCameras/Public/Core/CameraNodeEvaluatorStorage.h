// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluatorFwd.h"
#include "CoreTypes.h"
#include "HAL/UnrealMemory.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ObjectPtr.h"

class FCameraNodeEvaluator;
class FCameraNodeEvaluatorStorage;
class FReferenceCollector;
class UCameraEvaluationContext;
class UCameraRigAsset;
class UCameraNode;
class UCameraSystemEvaluator;

/** Allocation information for an entire tree of node evaluators. */
struct FCameraNodeEvaluatorTreeAllocationInfo
{
	int16 TotalSizeof = 0;
	int16 MaxAlignof = 0;
};

/** Structure for building an entire tree of node evaluators. */
struct FCameraNodeEvaluatorTreeBuilderParams
{
	/** The root node of the tree. */
	TObjectPtr<const UCameraNode> RootCameraNode;
	/** The evaluator running this evaluation. */
	TObjectPtr<UCameraSystemEvaluator> Evaluator;
	/** The evaluation context (if any) responsible for this branch of the evaluation. */
	TObjectPtr<const UCameraEvaluationContext> EvaluationContext;
	/** An optional allocation information to optimize storage. */
	FCameraNodeEvaluatorTreeAllocationInfo* AllocationInfo = nullptr;
};

/**
 * A class responsible for storing a tree of camera node evaluators.
 */
class FCameraNodeEvaluatorStorage
{
public:

	FCameraNodeEvaluatorStorage();
	FCameraNodeEvaluatorStorage(FCameraNodeEvaluatorStorage&& Other);
	FCameraNodeEvaluatorStorage& operator=(FCameraNodeEvaluatorStorage&& Other);
	~FCameraNodeEvaluatorStorage();

	FCameraNodeEvaluatorStorage(const FCameraNodeEvaluatorStorage&) = delete;
	FCameraNodeEvaluatorStorage& operator=(const FCameraNodeEvaluatorStorage&) = delete;

public:

	/** Compute allocation information for the given tree of camera nodes. */
	static FCameraNodeEvaluatorTreeAllocationInfo ComputeTreeInfo(const UCameraRigAsset* CameraRig);

	/** Build the tree of evaluators for the given tree of camera nodes. */
	FCameraNodeEvaluatorPtr BuildEvaluatorTree(const FCameraNodeEvaluatorTreeBuilderParams& Params);
	/** Destroy any allocated evaluators. */
	void DestroyEvaluatorTree(bool bFreeAllocations = false);

private:

	template<typename EvaluatorType, typename ...ArgTypes>
	EvaluatorType* BuildEvaluator(ArgTypes&&... InArgs);

private:

	struct FAllocation
	{
		uint8* Memory = nullptr;
		uint16 Capacity = 0;
		uint16 Used = 0;
	};
	TArray<FAllocation> Allocations;

	struct FEvaluatorInfo
	{
		FCameraNodeEvaluator* Ptr = nullptr;
	};
	TArray<FEvaluatorInfo> EvaluatorInfos;

	friend struct FCameraNodeEvaluatorBuilder;
};

template<typename EvaluatorType, typename ...ArgTypes>
EvaluatorType* FCameraNodeEvaluatorStorage::BuildEvaluator(ArgTypes&&... InArgs)
{
	static const uint16 DefaultCapacity = 128;
	static const uint16 DefaultAlignment = 32;

	uint16 StateSizeof = sizeof(EvaluatorType);
	uint16 StateAlignof = alignof(EvaluatorType);

	// Search for any allocation bucket that has enough room for the evaluator
	// we want to build.
	uint8* TargetPtr = nullptr;
	FAllocation* TargetAllocation = nullptr;
	for (FAllocation& Allocation : Allocations)
	{
		uint8* PossiblePtr = Align(Allocation.Memory + Allocation.Used, StateAlignof);
		uint16 NewUsed = (PossiblePtr + StateSizeof) - Allocation.Memory;
		if (NewUsed <= Allocation.Capacity)
		{
			TargetPtr = PossiblePtr;
			TargetAllocation = &Allocation;
			TargetAllocation->Used = NewUsed;
			break;
		}
	}
	// If we didn't find anything, we need to make a new allocation bucket.
	if (TargetPtr == nullptr)
	{
		const uint16 NewCapacity = FMath::Max(DefaultCapacity, StateSizeof);
		const uint16 NewAlignment = FMath::Max(DefaultAlignment, StateAlignof);

		FAllocation& NewAllocation = Allocations.Emplace_GetRef();
		NewAllocation.Memory = reinterpret_cast<uint8*>(FMemory::Malloc(NewCapacity, NewAlignment));
		NewAllocation.Capacity = NewCapacity;
		NewAllocation.Used = StateSizeof;

		TargetPtr = NewAllocation.Memory;
		TargetAllocation = &NewAllocation;
	}
	check(TargetPtr && TargetAllocation);

	EvaluatorType* NewEvaluator = new(TargetPtr) EvaluatorType(Forward<ArgTypes>(InArgs)...);

	EvaluatorInfos.Add({ NewEvaluator });

	return NewEvaluator;
}

