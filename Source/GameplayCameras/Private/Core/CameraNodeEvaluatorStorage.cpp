// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraNodeEvaluatorStorage.h"

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraRigAsset.h"
#include "UObject/Package.h"

namespace UE::Cameras
{

FCameraNodeEvaluatorStorage::FCameraNodeEvaluatorStorage()
{
}

FCameraNodeEvaluatorStorage::FCameraNodeEvaluatorStorage(FCameraNodeEvaluatorStorage&& Other)
{
	Allocations = MoveTemp(Other.Allocations);
	EvaluatorInfos = MoveTemp(Other.EvaluatorInfos);
}

FCameraNodeEvaluatorStorage& FCameraNodeEvaluatorStorage::operator=(FCameraNodeEvaluatorStorage&& Other)
{
	if (ensure(this != &Other))
	{
		Allocations = MoveTemp(Other.Allocations);
		EvaluatorInfos = MoveTemp(Other.EvaluatorInfos);
	}
	return *this;
}

FCameraNodeEvaluatorStorage::~FCameraNodeEvaluatorStorage()
{
	DestroyEvaluatorTree(true);
}

FCameraNodeEvaluatorPtr FCameraNodeEvaluatorStorage::BuildEvaluatorTree(const FCameraNodeEvaluatorTreeBuildParams& Params)
{
	if (Params.AllocationInfo)
	{
		const uint16 NewCapacity = Params.AllocationInfo->TotalSizeof;
		const uint16 NewAlignment = Params.AllocationInfo->MaxAlignof;

		FAllocation& NewAllocation = Allocations.Emplace_GetRef();
		NewAllocation.Memory = reinterpret_cast<uint8*>(FMemory::Malloc(NewCapacity, NewAlignment));
		NewAllocation.Capacity = NewCapacity;
		NewAllocation.Used = 0;
	}

	FCameraNodeEvaluatorBuilder Builder(*this);
	FCameraNodeEvaluatorBuildParams BuildParams(Builder);

	FCameraNodeEvaluatorPtr RootEvaluator = BuildParams.BuildEvaluator(Params.RootCameraNode);

	if (Params.bInitialize)
	{
		FCameraNodeEvaluatorInitializeParams InitParams;
		InitParams.Evaluator = Params.Evaluator;
		InitParams.EvaluationContext = Params.EvaluationContext;

		RootEvaluator->Initialize(InitParams);
	}

	return RootEvaluator;
}

void FCameraNodeEvaluatorStorage::DestroyEvaluatorTree(bool bFreeAllocations)
{
	for (FEvaluatorInfo& EvaluatorInfo : EvaluatorInfos)
	{
		FCameraNodeEvaluator* Evaluator(EvaluatorInfo.Ptr);
		Evaluator->~FCameraNodeEvaluator();
	}
	EvaluatorInfos.Reset();

	if (bFreeAllocations)
	{
		for (FAllocation& Allocation : Allocations)
		{
			FMemory::Free(Allocation.Memory);
		}
		Allocations.Reset();
	}
	else
	{
		for (FAllocation& Allocation : Allocations)
		{
			Allocation.Used = 0;
		}
	}
}

void FCameraNodeEvaluatorStorage::GetAllocationInfo(FCameraNodeEvaluatorAllocationInfo& OutAllocationInfo)
{
	OutAllocationInfo.TotalSizeof = 0;
	OutAllocationInfo.MaxAlignof = 0;

	if (!Allocations.IsEmpty())
	{
		OutAllocationInfo.MaxAlignof = Allocations[0].Alignment;

		for (FAllocation& Allocation : Allocations)
		{
			if (OutAllocationInfo.TotalSizeof > 0)
			{
				OutAllocationInfo.TotalSizeof = Align(OutAllocationInfo.TotalSizeof, Allocation.Alignment);
			}
			OutAllocationInfo.TotalSizeof += Allocation.Used;
		}
	}
}

}  // namespace UE::Cameras

