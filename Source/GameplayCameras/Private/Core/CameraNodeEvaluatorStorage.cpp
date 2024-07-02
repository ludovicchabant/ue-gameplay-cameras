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
		if (NewCapacity > 0 && NewAlignment > 0)
		{
			Super::AllocatePage(NewCapacity, NewAlignment);
		}
	}

	FCameraNodeEvaluatorBuilder Builder(*this);
	FCameraNodeEvaluatorBuildParams BuildParams(Builder);

	FCameraNodeEvaluatorPtr RootEvaluator = BuildParams.BuildEvaluator(Params.RootCameraNode);

	return RootEvaluator;
}

void FCameraNodeEvaluatorStorage::DestroyEvaluatorTree(bool bFreeAllocations)
{
	Super::DestroyObjects(bFreeAllocations);
}

void FCameraNodeEvaluatorStorage::GetAllocationInfo(FCameraNodeEvaluatorAllocationInfo& OutAllocationInfo)
{
	uint32 TotalUsed;
	uint32 FirstAlignment;
	Super::GetAllocationInfo(TotalUsed, FirstAlignment);

	OutAllocationInfo.TotalSizeof = TotalUsed;
	OutAllocationInfo.MaxAlignof = FirstAlignment;
}

}  // namespace UE::Cameras

