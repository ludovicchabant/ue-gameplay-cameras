// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Core/CameraContextDataTableFwd.h"

#include "CameraActorAttachmentInfo.generated.h"

class AActor;

/**
 * Attachment information for a camera rig.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYCAMERAS_API FCameraActorAttachmentInfo
{
	GENERATED_BODY()

	/** The actor to attach to. */
	UPROPERTY(EditAnywhere, Category="Attachment")
	TObjectPtr<AActor> Actor;

	/** An optional socket to attach to on the actor. */
	UPROPERTY(EditAnywhere, Interp, Category="Attachment")
	FName SocketName;

	/** The weight of this attachment. Unused if only one attachment is used. */
	UPROPERTY(EditAnywhere, Interp, Category="Attachment")
	float Weight = 1.f;

	bool operator== (const FCameraActorAttachmentInfo& Other) const = default;
};

namespace UE::Cameras
{

/** A special reader class for attachment information. */
struct FCameraActorAttachmentInfoReader
{
	FCameraActorAttachmentInfoReader() {}
	FCameraActorAttachmentInfoReader(const FCameraActorAttachmentInfo& InAttachmentInfo, FCameraContextDataID InDataID);

	void Initialize(const FCameraActorAttachmentInfo& InAttachmentInfo, FCameraContextDataID InDataID);

	bool GetAttachmentTransform(const FCameraContextDataTable& ContextDataTable, FTransform3d& OutTransform);

	static bool GetAttachmentTransform(TArrayView<FCameraActorAttachmentInfoReader> Readers, const FCameraContextDataTable& ContextDataTable, FTransform3d& OutTransform);

private:

	void CacheAttachmentInfo(const FCameraActorAttachmentInfo& InAttachmentInfo);

private:

	FCameraActorAttachmentInfo DefaultAttachmentInfo;
	FCameraContextDataID DataID;

	FCameraActorAttachmentInfo CachedAttachmentInfo;
	const USkeletalMeshComponent* CachedSkeletalMeshComponent = nullptr;
};

}  // namespace UE::Cameras

