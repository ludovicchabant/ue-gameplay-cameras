// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Core/CameraContextDataTableFwd.h"

#include "CameraTargetInfo.generated.h"

class AActor;

namespace UE::Cameras
{
	class FCameraContextDataTable;
}

USTRUCT(BlueprintType)
struct GAMEPLAYCAMERAS_API FCameraTargetInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Target")
	TObjectPtr<AActor> Actor;

	UPROPERTY(EditAnywhere, Category="Target")
	FName SocketName;
};

USTRUCT()
struct GAMEPLAYCAMERAS_API FCameraTargetInfoParameter
{
	GENERATED_BODY()

	using DataType = FCameraTargetInfo;

	UPROPERTY(EditAnywhere, Category=Common, meta=(ShowOnlyInnerProperties))
	FCameraTargetInfo Value;

	UPROPERTY()
	FCameraContextDataID DataID;

	FCameraTargetInfoParameter() {}
	FCameraTargetInfoParameter(const FCameraTargetInfo& InValue)
		: Value(InValue)
	{}

	bool HasOverride() const { return DataID.IsValid(); }
	const FCameraTargetInfo& GetValue(const UE::Cameras::FCameraContextDataTable& ContextDataTable) const;
};

namespace UE::Cameras
{

struct FCameraTargetInfoReader
{
	FCameraTargetInfoReader() {}
	FCameraTargetInfoReader(const FCameraTargetInfo& InTargetInfo);

	void Initialize(const FCameraTargetInfo& InTargetInfo);

	bool GetTargetTransform(FTransform3d& OutTransform) const;

private:

	FCameraTargetInfo TargetInfo;
	const USkeletalMeshComponent* CachedSkeletalMeshComponent = nullptr;
};

}  // namespace UE::Cameras

