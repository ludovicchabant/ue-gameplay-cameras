// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Attach/CameraActorAttachmentInfo.h"

#include "Algo/Accumulate.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/CameraContextDataTable.h"
#include "GameFramework/Actor.h"

namespace UE::Cameras
{

FCameraActorAttachmentInfoReader::FCameraActorAttachmentInfoReader(const FCameraActorAttachmentInfo& InAttachmentInfo, FCameraContextDataID InDataID)
{
	Initialize(InAttachmentInfo, InDataID);
}

void FCameraActorAttachmentInfoReader::Initialize(const FCameraActorAttachmentInfo& InAttachmentInfo, FCameraContextDataID InDataID)
{
	DefaultAttachmentInfo = InAttachmentInfo;
	DataID = InDataID;

	CacheAttachmentInfo(DefaultAttachmentInfo);
}

void FCameraActorAttachmentInfoReader::CacheAttachmentInfo(const FCameraActorAttachmentInfo& InAttachmentInfo)
{
	if (CachedAttachmentInfo != InAttachmentInfo)
	{
		CachedAttachmentInfo = InAttachmentInfo;

		CachedSkeletalMeshComponent = nullptr;
		if (InAttachmentInfo.Actor && !InAttachmentInfo.SocketName.IsNone())
		{
			CachedSkeletalMeshComponent = InAttachmentInfo.Actor->FindComponentByClass<USkeletalMeshComponent>();
		}
	}
}

bool FCameraActorAttachmentInfoReader::GetAttachmentTransform(const FCameraContextDataTable& ContextDataTable, FTransform3d& OutTransform)
{
	if (DataID.IsValid())
	{
		if (const FCameraActorAttachmentInfo* NewAttachmentInfo = ContextDataTable.TryGetData<FCameraActorAttachmentInfo>(DataID))
		{
			CacheAttachmentInfo(*NewAttachmentInfo);
		}
		else
		{
			CacheAttachmentInfo(DefaultAttachmentInfo);
		}
	}

	if (!CachedAttachmentInfo.SocketName.IsNone() && CachedSkeletalMeshComponent)
	{
		OutTransform = CachedSkeletalMeshComponent->GetSocketTransform(CachedAttachmentInfo.SocketName);
		return true;
	}
	else if (CachedAttachmentInfo.Actor)
	{
		OutTransform = CachedAttachmentInfo.Actor->GetTransform();
		return true;
	}
	return false;
}

bool FCameraActorAttachmentInfoReader::GetAttachmentTransform(TArrayView<FCameraActorAttachmentInfoReader> Readers, const FCameraContextDataTable& ContextDataTable,FTransform3d& OutTransform)
{
	using FWeightedTransform = TTuple<FTransform3d, float>;
	
	if (Readers.IsEmpty())
	{
		return false;
	}

	TArray<FWeightedTransform> WeightedTransforms;

	for (FCameraActorAttachmentInfoReader& Reader : Readers)
	{
		FTransform3d CurTransform;
		if (Reader.GetAttachmentTransform(ContextDataTable, CurTransform))
		{
			WeightedTransforms.Emplace(CurTransform, Reader.CachedAttachmentInfo.Weight);
		}
	}

	float TotalWeight = Algo::Accumulate(
			WeightedTransforms, 
			0.f,
			[](float Cur, const FWeightedTransform& Item) { return Cur + Item.Get<1>(); });
	if (TotalWeight == 0.f)
	{
		return false;
	}

	OutTransform = FTransform3d::Identity;
	for (const FWeightedTransform& WeightedTransform : WeightedTransforms)
	{
		OutTransform.BlendWith(WeightedTransform.Get<0>(), WeightedTransform.Get<1>() / TotalWeight);
	}

	return true;
}

}  // namespace UE::Cameras

