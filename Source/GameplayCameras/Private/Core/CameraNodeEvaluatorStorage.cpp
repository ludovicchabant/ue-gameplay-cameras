// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraNodeEvaluatorStorage.h"

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraRigAsset.h"
#include "UObject/Package.h"

namespace UE::Cameras
{

FCameraNodeEvaluatorPtr FCameraNodeEvaluatorStorage::BuildEvaluatorTree(const FCameraNodeEvaluatorTreeBuildParams& Params)
{
	if (Params.AllocationInfo)
	{
		const uint16 NewCapacity = Params.AllocationInfo->TotalSizeof;
		const uint16 NewAlignment = Params.AllocationInfo->MaxAlignof;
		Super::AllocatePage(NewCapacity, NewAlignment);
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
	Super::DestroyObjects(bFreeAllocations);
}

void FCameraNodeEvaluatorStorage::GetAllocationInfo(FCameraNodeEvaluatorAllocationInfo& OutAllocationInfo)
{
	uint16 TotalUsed;
	uint16 FirstAlignment;
	Super::GetAllocationInfo(TotalUsed, FirstAlignment);

	OutAllocationInfo.TotalSizeof = TotalUsed;
	OutAllocationInfo.MaxAlignof = FirstAlignment;
}

}  // namespace UE::Cameras

