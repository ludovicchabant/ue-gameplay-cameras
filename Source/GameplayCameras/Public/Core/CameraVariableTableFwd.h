// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "CoreTypes.h"
#include "Templates/TypeHash.h"

#include "CameraVariableTableFwd.generated.h"

class UCameraVariableAsset;

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

/**
 * The type of a camera variable. 
 *
 * Only a fixed set of types are supported for camera variables because of
 * simplicity, and because these types need to be blendable.
 */
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

/**
 * The ID of a camera variable, used to refer to it in a camera variable table.
 */
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

	friend FArchive& operator<< (FArchive& Ar, FCameraVariableID& In)
	{
		Ar << In.Value;
		return Ar;
	}

private:

	FCameraVariableID(uint32 InValue) : Value(InValue) {}

	static const uint32 INVALID = uint32(-1);

	UPROPERTY()
	uint32 Value;
};

/**
 * A structure that describes a camera variable.
 */
USTRUCT()
struct FCameraVariableDefinition
{
	GENERATED_BODY()

	/** The ID of the variable. */
	UPROPERTY()
	FCameraVariableID VariableID;

	/** The type of the variable. */
	UPROPERTY()
	ECameraVariableType VariableType = ECameraVariableType::Boolean;

	/**
	 * Whether the variable is private. 
	 *
	 * Private variables are not propagated from one table to another when
	 * interpolating or overriding a table.
	 */
	UPROPERTY()
	bool bIsPrivate = false;

	/**
	 * Whether the variable is an input variable.
	 *
	 * Input variables are blended during the pre-blend parameter update phase.
	 */
	UPROPERTY()
	bool bIsInput = false;

#if WITH_EDITORONLY_DATA
	/** The name of the variable, for debugging purposes. */
	UPROPERTY()
	FString VariableName;
#endif

	/** Returns whether this definition has a valid variable ID. */
	bool IsValid() const
	{
		return VariableID.IsValid();
	}

	/** Implicit conversion to a camera variable ID. */
	operator FCameraVariableID() const
	{
		return VariableID;
	}

	/** Creates a variant of this camera variable definition. */
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

	GAMEPLAYCAMERAS_API friend bool operator==(const FCameraVariableDefinition& A, const FCameraVariableDefinition& B);
};

template<>
struct TStructOpsTypeTraits<FCameraVariableDefinition> : public TStructOpsTypeTraitsBase2<FCameraVariableDefinition>
{
	enum
	{
		WithCopy = true,
		WithIdenticalViaEquality = true
	};
};

/**
 * A structure that describes the required camera variable table setup of a camera rig.
 */
USTRUCT()
struct FCameraVariableTableAllocationInfo
{
	GENERATED_BODY()

	/** The list of variables that should be allocated in a table. */
	UPROPERTY()
	TArray<FCameraVariableDefinition> VariableDefinitions;

	/** The list of variables that should be auto-reset to their default value every frame. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraVariableAsset>> AutoResetVariables;

	GAMEPLAYCAMERAS_API friend bool operator==(const FCameraVariableTableAllocationInfo& A, const FCameraVariableTableAllocationInfo& B);
};

template<>
struct TStructOpsTypeTraits<FCameraVariableTableAllocationInfo> : public TStructOpsTypeTraitsBase2<FCameraVariableTableAllocationInfo>
{
	enum
	{
		WithCopy = true,
		WithIdenticalViaEquality = true
	};
};

