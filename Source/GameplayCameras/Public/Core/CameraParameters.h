// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraVariableAssets.h"
#include "CoreTypes.h"
#include "Math/MathFwd.h"

#include "CameraParameters.generated.h"

#define UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS(ParameterClass)\
	ParameterClass(typename TCallTraits<ParameterClass::ValueType>::ParamType InValue)\
		: Value(InValue)\
	{}

#define UE_DEFINE_CAMERA_PARAMETER_ALL_CONSTRUCTORS(ParameterClass)\
	ParameterClass() {}\
	UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS(ParameterClass)

/** Boolean camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FBooleanCameraParameter
{
	GENERATED_BODY()

	using ValueType = bool;
	using VariableAssetType = UBooleanCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	bool Value = false;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UBooleanCameraVariable> Variable;

	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_ALL_CONSTRUCTORS(FBooleanCameraParameter)
};

/** Integer camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FInteger32CameraParameter
{
	GENERATED_BODY()

	using ValueType = int32;
	using VariableAssetType = UInteger32CameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	int32 Value = 0;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UInteger32CameraVariable> Variable;

	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_ALL_CONSTRUCTORS(FInteger32CameraParameter)
};

/** Float camera parameter. */
USTRUCT()
struct FFloatCameraParameter
{
	GENERATED_BODY()

	using ValueType = float;
	using VariableAssetType = UFloatCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	float Value = 0.f;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UFloatCameraVariable> Variable;

	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_ALL_CONSTRUCTORS(FFloatCameraParameter)
};

/** Double camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FDoubleCameraParameter
{
	GENERATED_BODY()

	using ValueType = double;
	using VariableAssetType = UDoubleCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	double Value = 0.0;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UDoubleCameraVariable> Variable;

	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_ALL_CONSTRUCTORS(FDoubleCameraParameter)
};

/** Vector2f camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FVector2fCameraParameter
{
	GENERATED_BODY()

	using ValueType = FVector2f;
	using VariableAssetType = UVector2fCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	FVector2f Value;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UVector2fCameraVariable> Variable;

	FVector2fCameraParameter();
	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS(FVector2fCameraParameter)
};

/** Vector2d camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FVector2dCameraParameter
{
	GENERATED_BODY()

	using ValueType = FVector2D;
	using VariableAssetType = UVector2dCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	FVector2D Value;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UVector2dCameraVariable> Variable;

	FVector2dCameraParameter();
	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS(FVector2dCameraParameter)
};

/** Vector3f camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FVector3fCameraParameter
{
	GENERATED_BODY()

	using ValueType = FVector3f;
	using VariableAssetType = UVector3fCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	FVector3f Value;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UVector3fCameraVariable> Variable;

	FVector3fCameraParameter();
	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS(FVector3fCameraParameter)
};

/** Vector3d camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FVector3dCameraParameter
{
	GENERATED_BODY()

	using ValueType = FVector3d;
	using VariableAssetType = UVector3dCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	FVector3d Value;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UVector3dCameraVariable> Variable;

	FVector3dCameraParameter();
	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS(FVector3dCameraParameter)
};

/** Vector4f camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FVector4fCameraParameter
{
	GENERATED_BODY()

	using ValueType = FVector4f;
	using VariableAssetType = UVector4fCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	FVector4f Value;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UVector4fCameraVariable> Variable;

	FVector4fCameraParameter();
	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS(FVector4fCameraParameter)
};

/** Vector4d camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FVector4dCameraParameter
{
	GENERATED_BODY()

	using ValueType = FVector4d;
	using VariableAssetType = UVector4dCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	FVector4d Value;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UVector4dCameraVariable> Variable;

	FVector4dCameraParameter();
	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS(FVector4dCameraParameter)
};

/** Rotator3f camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FRotator3fCameraParameter
{
	GENERATED_BODY()

	using ValueType = FRotator3f;
	using VariableAssetType = URotator3fCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	FRotator3f Value;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<URotator3fCameraVariable> Variable;

	FRotator3fCameraParameter();
	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS(FRotator3fCameraParameter)
};

/** Rotator3d camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FRotator3dCameraParameter
{
	GENERATED_BODY()

	using ValueType = FRotator3d;
	using VariableAssetType = URotator3dCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	FRotator3d Value;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<URotator3dCameraVariable> Variable;

	FRotator3dCameraParameter();
	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS(FRotator3dCameraParameter)
};

/** Transform3f camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FTransform3fCameraParameter
{
	GENERATED_BODY()

	using ValueType = FTransform3f;
	using VariableAssetType = UTransform3fCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	FTransform3f Value;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UTransform3fCameraVariable> Variable;

	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_ALL_CONSTRUCTORS(FTransform3fCameraParameter)
};

/** Transform3d camera parameter. */
USTRUCT()
struct GAMEPLAYCAMERAS_API FTransform3dCameraParameter
{
	GENERATED_BODY()

	using ValueType = FTransform3d;
	using VariableAssetType = UTransform3dCameraVariable;

	UPROPERTY(EditAnywhere, Category=Common)
	FTransform3d Value;

	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UTransform3dCameraVariable> Variable;

	bool SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot);

	UE_DEFINE_CAMERA_PARAMETER_ALL_CONSTRUCTORS(FTransform3dCameraParameter)
};

#undef UE_DEFINE_CAMERA_PARAMETER_VALUE_CONSTRUCTORS
#undef UE_DEFINE_CAMERA_PARAMETER_ALL_CONSTRUCTORS

template<typename ValueType>
bool CameraParameterValueEquals(typename TCallTraits<ValueType>::ParamType A, typename TCallTraits<ValueType>::ParamType B)
{
	return A == B;
}

template<>
inline bool CameraParameterValueEquals<FTransform3f>(const FTransform3f& A, const FTransform3f& B)
{
	return A.Equals(B);
}

template<>
inline bool CameraParameterValueEquals<FTransform3d>(const FTransform3d& A, const FTransform3d& B)
{
	return A.Equals(B);
}

// Any camera parameter might replace a previously non-parameterized property (i.e. a "fixed" property
// of the underlying type, like bool, int32, float, etc.)
// When someone upgrades the fixed property to a parameterized property, any previously saved data will
// run into a mismatched tag. So the parameters will handle that by loading the saved value inside of
// them.
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	template<> struct TStructOpsTypeTraits<F##ValueName##CameraParameter>\
		: public TStructOpsTypeTraitsBase2<F##ValueName##CameraParameter>\
	{\
		enum { WithStructuredSerializeFromMismatchedTag = true };\
	};
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

