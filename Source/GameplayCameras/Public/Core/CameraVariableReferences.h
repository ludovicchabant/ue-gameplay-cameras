// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraVariableAssets.h"

#include "CameraVariableReferences.generated.h"

namespace UE::Cameras
{
	class FCameraVariableTable;
}

#define UE_DEFINE_CAMERA_VARIABLE_REFERENCE(ValueName)\
	F##ValueName##CameraVariableReference() {}\
	F##ValueName##CameraVariableReference(VariableAssetType* InVariable) : Variable(InVariable) {}\
	bool IsValid() const { return Variable != nullptr; }\
	const F##ValueName##CameraVariableReference::ValueType* GetValue(const UE::Cameras::FCameraVariableTable& VariableTable) const;

// Camera variable references are simple wrappers around a UCameraVariableAsset sub-class pointer, 
// with a Details View customization to show an appropriate picker widget for it.

USTRUCT()
struct FBooleanCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = bool;
	using VariableAssetType = UBooleanCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UBooleanCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Boolean)
};

USTRUCT()
struct FInteger32CameraVariableReference
{
	GENERATED_BODY()

	using ValueType = int32;
	using VariableAssetType = UInteger32CameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UInteger32CameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Integer32)
};

USTRUCT()
struct FFloatCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = float;
	using VariableAssetType = UFloatCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UFloatCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Float)
};

USTRUCT()
struct FDoubleCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = double;
	using VariableAssetType = UDoubleCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UDoubleCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Double)
};

USTRUCT()
struct FVector2fCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = FVector2f;
	using VariableAssetType = UVector2fCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector2fCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector2f)
};

USTRUCT()
struct FVector2dCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = FVector2d;
	using VariableAssetType = UVector2dCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector2dCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector2d)
};

USTRUCT()
struct FVector3fCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = FVector3f;
	using VariableAssetType = UVector3fCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector3fCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector3f)
};

USTRUCT()
struct FVector3dCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = FVector3d;
	using VariableAssetType = UVector3dCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector3dCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector3d)
};

USTRUCT()
struct FVector4fCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = FVector4f;
	using VariableAssetType = UVector4fCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector4fCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector4f)
};

USTRUCT()
struct FVector4dCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = FVector4d;
	using VariableAssetType = UVector4dCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector4dCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector4d)
};

USTRUCT()
struct FRotator3fCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = FRotator3f;
	using VariableAssetType = URotator3fCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<URotator3fCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Rotator3f)
};

USTRUCT()
struct FRotator3dCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = FRotator3d;
	using VariableAssetType = URotator3dCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<URotator3dCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Rotator3d)
};

USTRUCT()
struct FTransform3fCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = FTransform3f;
	using VariableAssetType = UTransform3fCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UTransform3fCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Transform3f)
};

USTRUCT()
struct FTransform3dCameraVariableReference
{
	GENERATED_BODY()

	using ValueType = FTransform3d;
	using VariableAssetType = UTransform3dCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UTransform3dCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Transform3d)
};

