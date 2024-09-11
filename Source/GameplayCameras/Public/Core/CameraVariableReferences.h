// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraVariableAssets.h"

#include "CameraVariableReferences.generated.h"

#define UE_DEFINE_CAMERA_VARIABLE_REFERENCE(ValueName)\
	F##ValueName##CameraVariableReference() {}\
	F##ValueName##CameraVariableReference(VariableAssetType* InVariable) : Variable(InVariable) {}\
	bool IsValid() const { return (bool)Variable; }\
	U##ValueName##CameraVariable* Get() const { return Variable.Get(); }\
	operator U##ValueName##CameraVariable* () const { return Variable.Get(); }

USTRUCT()
struct FBooleanCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UBooleanCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UBooleanCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Boolean)
};

USTRUCT()
struct FInteger32CameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UInteger32CameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UInteger32CameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Integer32)
};

USTRUCT()
struct FFloatCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UFloatCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UFloatCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Float)
};

USTRUCT()
struct FDoubleCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UDoubleCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UDoubleCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Double)
};

USTRUCT()
struct FVector2fCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UVector2fCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector2fCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector2f)
};

USTRUCT()
struct FVector2dCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UVector2dCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector2dCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector2d)
};

USTRUCT()
struct FVector3fCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UVector3fCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector3fCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector3f)
};

USTRUCT()
struct FVector3dCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UVector3dCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector3dCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector3d)
};

USTRUCT()
struct FVector4fCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UVector4fCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector4fCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector4f)
};

USTRUCT()
struct FVector4dCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UVector4dCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UVector4dCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Vector4d)
};

USTRUCT()
struct FRotator3fCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = URotator3fCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<URotator3fCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Rotator3f)
};

USTRUCT()
struct FRotator3dCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = URotator3dCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<URotator3dCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Rotator3d)
};

USTRUCT()
struct FTransform3fCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UTransform3fCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UTransform3fCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Transform3f)
};

USTRUCT()
struct FTransform3dCameraVariableReference
{
	GENERATED_BODY()

	using VariableAssetType = UTransform3dCameraVariable;

	UPROPERTY(EditAnywhere, Category="Variable")
	TObjectPtr<UTransform3dCameraVariable> Variable;

	UE_DEFINE_CAMERA_VARIABLE_REFERENCE(Transform3d)
};

