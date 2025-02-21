// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Framing/CameraActorTargetInfo.h"

#include "Components/SkeletalMeshComponent.h"
#include "Core/CameraContextDataTable.h"
#include "GameFramework/Actor.h"

namespace UE::Cameras
{

FCameraActorTargetInfoReader::FCameraActorTargetInfoReader(const FCameraActorTargetInfo& InTargetInfo, FCameraContextDataID InDataID)
{
	Initialize(InTargetInfo, InDataID);
}

void FCameraActorTargetInfoReader::Initialize(const FCameraActorTargetInfo& InTargetInfo, FCameraContextDataID InDataID)
{
	DefaultTargetInfo = InTargetInfo;
	DataID = InDataID;

	CacheTargetInfo(DefaultTargetInfo);
}

void FCameraActorTargetInfoReader::CacheTargetInfo(const FCameraActorTargetInfo& InTargetInfo)
{
	if (CachedTargetInfo != InTargetInfo)
	{
		CachedTargetInfo = InTargetInfo;

		CachedSkeletalMeshComponent = nullptr;
		if (InTargetInfo.Actor && (!InTargetInfo.SocketName.IsNone() || !InTargetInfo.BoneName.IsNone()))
		{
			CachedSkeletalMeshComponent = InTargetInfo.Actor->FindComponentByClass<USkeletalMeshComponent>();
		}

		CachedBoneName = NAME_None;
		CachedParentBoneName = NAME_None;
		if (CachedSkeletalMeshComponent)
		{
			CachedBoneName = InTargetInfo.BoneName;
			if (!InTargetInfo.SocketName.IsNone())
			{
				CachedBoneName = CachedSkeletalMeshComponent->GetSocketBoneName(InTargetInfo.SocketName);
			}
			if (!CachedBoneName.IsNone())
			{
				CachedParentBoneName = CachedSkeletalMeshComponent->GetParentBone(CachedBoneName);
			}
		}
	}
}

bool FCameraActorTargetInfoReader::GetTargetInfo(const FCameraContextDataTable& ContextDataTable, FTransform3d& OutTransform, FBoxSphereBounds3d& OutBounds)
{
	if (DataID.IsValid())
	{
		if (const FCameraActorTargetInfo* NewTargetInfo = ContextDataTable.TryGetData<FCameraActorTargetInfo>(DataID))
		{
			CacheTargetInfo(*NewTargetInfo);
		}
		else
		{
			CacheTargetInfo(DefaultTargetInfo);
		}
	}

	if (CachedSkeletalMeshComponent && !CachedBoneName.IsNone())
	{
		OutTransform = CachedSkeletalMeshComponent->GetBoneTransform(CachedBoneName);
		ComputeTargetBounds(OutTransform.GetLocation(), OutBounds);
		return true;
	}
	else if (CachedTargetInfo.Actor)
	{
		OutTransform = CachedTargetInfo.Actor->GetTransform();
		ComputeTargetBounds(OutTransform.GetLocation(), OutBounds);
		return true;
	}
	return false;
}

void FCameraActorTargetInfoReader::ComputeTargetBounds(const FVector3d& TargetLocation, FBoxSphereBounds3d& OutBounds)
{
	switch (CachedTargetInfo.TargetShape)
	{
		case ECameraTargetShape::Point:
			OutBounds = FBoxSphereBounds3d(EForceInit::ForceInit);
			break;
		case ECameraTargetShape::AutomaticBounds:
			if (CachedSkeletalMeshComponent && !CachedParentBoneName.IsNone())
			{
				const FVector3d ParentBoneLocation = CachedSkeletalMeshComponent->GetBoneLocation(CachedParentBoneName);
				const FVector3d ParentToBone((TargetLocation - ParentBoneLocation).GetAbs());
				const float ParentToBoneLength(ParentToBone.Length());
				OutBounds = FBoxSphereBounds3d(FVector3d::ZeroVector, ParentToBone, ParentToBoneLength);
			}
			else
			{
				OutBounds = FBoxSphereBounds3d(EForceInit::ForceInit);
				if (USceneComponent* RootComponent = CachedTargetInfo.Actor->GetRootComponent())
				{
					OutBounds = RootComponent->Bounds;
				}
			}
			break;
		case ECameraTargetShape::ManualBounds:
			{
				float TargetSize = FMath::Max(0.f, CachedTargetInfo.TargetSize);
				OutBounds = FBoxSphereBounds3d(FVector3d::ZeroVector, FVector3d(TargetSize), TargetSize);
			}
			break;
	}
}

}  // namespace UE::Cameras

