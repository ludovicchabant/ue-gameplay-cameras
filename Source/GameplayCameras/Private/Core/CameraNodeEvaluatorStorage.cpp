// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraNodeEvaluatorStorage.h"

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraRigAsset.h"
#include "UObject/Package.h"

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

FCameraNodeEvaluatorTreeAllocationInfo FCameraNodeEvaluatorStorage::ComputeTreeInfo(const UCameraRigAsset* CameraRig)
{
	int16 MaxSize = 0;
	int16 MaxAlignment = 0;

	// This isn't ideal, maybe we need a GetChildren API on the UCameraNode class too.
	UPackage* Package = CameraRig->GetPackage();
	ForEachObjectWithPackage(Package, [&MaxSize, &MaxAlignment](UObject* Object)
	{
		if (UCameraNode* CurNode = Cast<UCameraNode>(Object))
		{
			FCameraNodeAllocationInfo CurInfo = CurNode->GetAllocationInfo();

			MaxAlignment = FMath::Max(MaxAlignment, CurInfo.EvaluatorInfo.Alignof);
			MaxSize = Align(MaxSize, CurInfo.EvaluatorInfo.Alignof) + CurInfo.EvaluatorInfo.Sizeof;
		}
		return true; // Keep iterating.
	});

	return FCameraNodeEvaluatorTreeAllocationInfo{ MaxSize, MaxAlignment };
}

FCameraNodeEvaluatorPtr FCameraNodeEvaluatorStorage::BuildEvaluatorTree(const FCameraNodeEvaluatorTreeBuilderParams& Params)
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

	FCameraNodeEvaluatorInitializeParams InitParams;
	InitParams.Evaluator = Params.Evaluator;
	InitParams.EvaluationContext = Params.EvaluationContext;
	InitParams.Builder = &Builder;

	FCameraNodeEvaluatorPtr RootEvaluator = InitParams.BuildEvaluator(Params.RootCameraNode);

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

