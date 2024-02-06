// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "CoreTypes.h"

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
struct FCameraVariableDefinition
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 VariableId = 0;

	UPROPERTY()
	ECameraVariableType VariableType;

	UPROPERTY()
	bool bIsPrivate = false;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	FString VariableName;
#endif
};

USTRUCT()
struct FCameraVariableTableAllocationInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FCameraVariableDefinition> VariableDefinitions;
};

