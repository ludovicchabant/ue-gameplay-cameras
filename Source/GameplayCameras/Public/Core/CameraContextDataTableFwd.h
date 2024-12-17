// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayCameras.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UnrealNames.h"

#include "CameraContextDataTableFwd.generated.h"

namespace UE::Cameras
{ 
	class FCameraContextDataTable;
#if UE_GAMEPLAY_CAMERAS_DEBUG
	class FContextDataTableDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
}

/**
 * Supported types for a camera node's context data.
 * 
 * NOTE: simple types (bool, integer, float, etc.) and vector types (vector, rotator, transform)
 * are not supported as context data because they are supported as blendable parameters.
 */
UENUM()
enum class ECameraContextDataType
{
	Name,
	String,
	Enum UMETA(Hidden),
	Struct UMETA(Hidden),
	Object UMETA(Hidden),
	Class UMETA(Hidden),

	Count UMETA(Hidden)
};

/**
 * The ID of a context data, used to refer to it in a camera context data table.
 */
USTRUCT(BlueprintType)
struct FCameraContextDataID
{
	GENERATED_BODY()

public:

	FCameraContextDataID() : DataName(NAME_None) {}

	bool IsValid() const { return DataName != NAME_None; }

	explicit operator bool() const { return IsValid(); }

	static FCameraContextDataID FromName(FName InName)
	{
		return FCameraContextDataID{ InName };
	}

public:

	friend bool operator==(FCameraContextDataID A, FCameraContextDataID B)
	{
		return A.DataName == B.DataName;
	}

	friend bool operator!=(FCameraContextDataID A, FCameraContextDataID B)
	{
		return !(A == B);
	}

	friend bool operator<(FCameraContextDataID A, FCameraContextDataID B)
	{
		return A.DataName.Compare(B.DataName) < 0;
	}

	friend uint32 GetTypeHash(FCameraContextDataID In)
	{
		return GetTypeHash(In.DataName);
	}

private:

	FCameraContextDataID(FName InName) : DataName(InName) {}

	UPROPERTY()
	FName DataName = NAME_None;

	friend class UE::Cameras::FCameraContextDataTable;
#if UE_GAMEPLAY_CAMERAS_DEBUG
	friend class UE::Cameras::FContextDataTableDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

