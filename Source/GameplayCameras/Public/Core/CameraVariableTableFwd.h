// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "CoreTypes.h"
#include "Templates/TypeHash.h"

#include "CameraVariableTableFwd.generated.h"

#define UE_CAMERA_VARIABLE_FOR_ALL_TYPES()\
	UE_CAMERA_VARIABLE_FOR_TYPE(bool, Boolean)\
	UE_CAMERA_VARIABLE_FOR_TYPE(int32, Integer32)\
	UE_CAMERA_VARIABLE_FOR_TYPE(float, Float)\
	UE_CAMERA_VARIABLE_FOR_TYPE(double, Double)\
	UE_CAMERA_VARIABLE_FOR_TYPE(FVector2f, Vector2f)\
	UE_CAMERA_VARIABLE_FOR_TYPE(FVector2d, Vector2d)\
	UE_CAMERA_VARIABLE_FOR_TYPE(FVector3f, Vector3f)\
	UE_CAMERA_VARIABLE_FOR_TYPE(FVector3d, Vector3d)\
	UE_CAMERA_VARIABLE_FOR_TYPE(FVector4f, Vector4f)\
	UE_CAMERA_VARIABLE_FOR_TYPE(FVector4d, Vector4d)\
	UE_CAMERA_VARIABLE_FOR_TYPE(FRotator3f, Rotator3f)\
	UE_CAMERA_VARIABLE_FOR_TYPE(FRotator3d, Rotator3d)\
	UE_CAMERA_VARIABLE_FOR_TYPE(FTransform3f, Transform3f)\
	UE_CAMERA_VARIABLE_FOR_TYPE(FTransform3d, Transform3d)

UENUM()
enum class ECameraVariableType
{
	Boolean,
	Integer32,
	Float,
	Double,
	Vector2f,
	Vector2d,
	Vector3f,
	Vector3d,
	Vector4f,
	Vector4d,
	Rotator3f,
	Rotator3d,
	Transform3f,
	Transform3d
};

USTRUCT()
struct FCameraVariableID
{
	GENERATED_BODY()

public:

	FCameraVariableID() : Value(INVALID) {}

	uint32 GetValue() const { return Value; }

	bool IsValid() const { return Value != INVALID; }

	explicit operator bool() const { return IsValid(); }

	static FCameraVariableID FromHashValue(uint32 InValue)
	{
		return FCameraVariableID(InValue);
	}

public:

	friend bool operator<(FCameraVariableID A, FCameraVariableID B)
	{
		return A.Value < B.Value;
	}

	friend bool operator==(FCameraVariableID A, FCameraVariableID B)
	{
		return A.Value == B.Value;
	}

	friend bool operator!=(FCameraVariableID A, FCameraVariableID B)
	{
		return A.Value != B.Value;
	}

	friend uint32 GetTypeHash(FCameraVariableID In)
	{
		return In.Value;
	}

private:

	FCameraVariableID(uint32 InValue) : Value(InValue) {}

	static const uint32 INVALID = uint32(-1);

	UPROPERTY()
	uint32 Value;
};

USTRUCT()
struct FCameraVariableDefinition
{
	GENERATED_BODY()

	UPROPERTY()
	FCameraVariableID VariableID;

	UPROPERTY()
	ECameraVariableType VariableType = ECameraVariableType::Boolean;

	UPROPERTY()
	bool bIsPrivate = false;

	UPROPERTY()
	bool bIsInput = false;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	FString VariableName;
#endif

	bool IsValid() const
	{
		return VariableID.IsValid();
	}

	FCameraVariableDefinition CreateVariant(const FString& VariantID) const
	{
		FCameraVariableDefinition VariantDefinition(*this);
		VariantDefinition.VariableID = FCameraVariableID::FromHashValue(
				HashCombineFast(VariableID.GetValue(), GetTypeHash(VariantID)));
#if WITH_EDITORONLY_DATA
		if (!VariableName.IsEmpty())
		{
			VariantDefinition.VariableName += FString::Format(TEXT("_{0}Variant"), { VariantID });
		}
#endif
		return VariantDefinition;
	}
};

USTRUCT()
struct FCameraVariableTableAllocationInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FCameraVariableDefinition> VariableDefinitions;
};

