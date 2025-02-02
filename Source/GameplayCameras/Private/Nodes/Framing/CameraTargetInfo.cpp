// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Framing/CameraTargetInfo.h"

#include "Components/SkeletalMeshComponent.h"
#include "Core/CameraContextDataTable.h"
#include "GameFramework/Actor.h"

const FCameraTargetInfo& FCameraTargetInfoParameter::GetValue(const UE::Cameras::FCameraContextDataTable& ContextDataTable) const
{
	if (DataID.IsValid())
	{
		if (const FCameraTargetInfo* ActualValue = ContextDataTable.TryGetData<FCameraTargetInfo>(DataID))
		{
			return *ActualValue;
		}
	}
	return Value;
}

namespace UE::Cameras
{

FCameraTargetInfoReader::FCameraTargetInfoReader(const FCameraTargetInfo& InTargetInfo)
{
	Initialize(InTargetInfo);
}

void FCameraTargetInfoReader::Initialize(const FCameraTargetInfo& InTargetInfo)
{
	TargetInfo = InTargetInfo;
	if (TargetInfo.Actor && !TargetInfo.SocketName.IsNone())
	{
		CachedSkeletalMeshComponent = TargetInfo.Actor->FindComponentByClass<USkeletalMeshComponent>();
	}
}

bool FCameraTargetInfoReader::GetTargetTransform(FTransform3d& OutTransform) const
{
	if (!TargetInfo.SocketName.IsNone() && CachedSkeletalMeshComponent)
	{
		OutTransform = CachedSkeletalMeshComponent->GetSocketTransform(TargetInfo.SocketName);
		return true;
	}
	return false;
}

}  // namespace UE::Cameras

