// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/CameraParameterMigrationHelper.h"

#include "Core/BaseCameraObject.h"
#include "Core/CameraParameters.h"
#include "StructUtils/OverridablePropertyBag.h"
#include "UObject/Object.h"

namespace UE::Cameras
{

/** Structure for saving the value of a single-precision camera parameter value. */
struct FParameterPropertyBackup
{
	FGuid PropertyID;
	FName PropertyName;
	const UObject* PropertyTypeObject = nullptr;

	union
	{
		float FloatValue;
		FVector2f Vector2fValue;
		FVector3f Vector3fValue;
		FVector4f Vector4fValue;
		FRotator3f Rotator3fValue;
		FTransform3f Transform3fValue;
	};

	template<typename ParameterType>
	void SaveBackup(const FInstancedPropertyBag& Parameters, const FPropertyBagPropertyDesc& PropertyDesc)
	{
		TValueOrError<FStructView, EPropertyBagResult> ValueOrError = Parameters.GetValueStruct(PropertyDesc, ParameterType::StaticStruct());
		ensure(ValueOrError.HasValue() && !ValueOrError.HasError());
		const ParameterType& ParameterStruct = ValueOrError.GetValue().Get<ParameterType>();
		SetValueImpl<typename ParameterType::ValueType>(ParameterStruct.Value);
	}

	void TryRestoreBackup(FInstancedPropertyBag& Parameters) const
	{
		const FPropertyBagPropertyDesc* NewPropertyDesc = Parameters.FindPropertyDescByID(PropertyID);
		if (!NewPropertyDesc)
		{
			// The property was removed.
			return;
		}
		if (NewPropertyDesc->ContainerTypes.Num() > 0)
		{
			// The property was turned into an array or other container.
			return;
		}
		if (NewPropertyDesc->ValueType != EPropertyBagPropertyType::Struct)
		{
			// The property was turned into a non-struct type (so not a camera parameter anymore).
			return;
		}

		// We support converting from single-precision to double-precision parameters. We don't need to support 
		// converting the other way since we want all user-facing data to be double-precision.
		if (NewPropertyDesc->ValueTypeObject == FDoubleCameraParameter::StaticStruct())
		{
			RestoreBackup<FDoubleCameraParameter>(Parameters, *NewPropertyDesc);
		}
		else if (NewPropertyDesc->ValueTypeObject == FVector2dCameraParameter::StaticStruct())
		{
			RestoreBackup<FVector2dCameraParameter>(Parameters, *NewPropertyDesc);
		}
		else if (NewPropertyDesc->ValueTypeObject == FVector3dCameraParameter::StaticStruct())
		{
			RestoreBackup<FVector3dCameraParameter>(Parameters, *NewPropertyDesc);
		}
		else if (NewPropertyDesc->ValueTypeObject == FVector4dCameraParameter::StaticStruct())
		{
			RestoreBackup<FVector4dCameraParameter>(Parameters, *NewPropertyDesc);
		}
		else if (NewPropertyDesc->ValueTypeObject == FRotator3dCameraParameter::StaticStruct())
		{
			RestoreBackup<FRotator3dCameraParameter>(Parameters, *NewPropertyDesc);
		}
		else if (NewPropertyDesc->ValueTypeObject == FTransform3dCameraParameter::StaticStruct())
		{
			RestoreBackup<FTransform3dCameraParameter>(Parameters, *NewPropertyDesc);
		}
	}

private:

	template<typename ParameterType>
	void RestoreBackup(FInstancedPropertyBag& Parameters, const FPropertyBagPropertyDesc& PropertyDesc) const
	{
		TValueOrError<ParameterType*, EPropertyBagResult> ValueOrError = Parameters.GetValueStruct<ParameterType>(PropertyDesc);
		ensure(ValueOrError.HasValue() && !ValueOrError.HasError());
		ParameterType* ParameterStruct = ValueOrError.GetValue();
		ParameterStruct->Value = GetConvertedValueImpl<typename ParameterType::ValueType>();
	}

private:

	template<typename ValueType>
	void SetValueImpl(ValueType InValue);

	template<typename ConvertedValueType>
	ConvertedValueType GetConvertedValueImpl() const;
};

template<> void FParameterPropertyBackup::SetValueImpl<float>(float InValue) { FloatValue = InValue; }
template<> void FParameterPropertyBackup::SetValueImpl<FVector2f>(FVector2f InValue) { Vector2fValue = InValue; }
template<> void FParameterPropertyBackup::SetValueImpl<FVector3f>(FVector3f InValue) { Vector3fValue = InValue; }
template<> void FParameterPropertyBackup::SetValueImpl<FVector4f>(FVector4f InValue) { Vector4fValue = InValue; }
template<> void FParameterPropertyBackup::SetValueImpl<FRotator3f>(FRotator3f InValue) { Rotator3fValue = InValue; }
template<> void FParameterPropertyBackup::SetValueImpl<FTransform3f>(FTransform3f InValue) { Transform3fValue = InValue; }

template<> double FParameterPropertyBackup::GetConvertedValueImpl<double>() const { return (double)FloatValue; }
template<> FVector2d FParameterPropertyBackup::GetConvertedValueImpl<FVector2d>() const { return FVector2d(Vector2fValue); }
template<> FVector3d FParameterPropertyBackup::GetConvertedValueImpl<FVector3d>() const { return FVector3d(Vector3fValue); }
template<> FVector4d FParameterPropertyBackup::GetConvertedValueImpl<FVector4d>() const { return FVector4d(Vector4fValue); }
template<> FRotator3d FParameterPropertyBackup::GetConvertedValueImpl<FRotator3d>() const { return FRotator3d(Rotator3fValue); }
template<> FTransform3d FParameterPropertyBackup::GetConvertedValueImpl<FTransform3d>() const { return FTransform3d(Transform3fValue); }

template<typename ParameterType>
void ProcessParameterPropertyDesc(
		const FInstancedPropertyBag& Parameters,
		const FPropertyBagPropertyDesc& PropertyDesc,
		TArray<FParameterPropertyBackup>& OutPropertyBackups)
{
	ensure(PropertyDesc.ValueTypeObject == ParameterType::StaticStruct());

	FParameterPropertyBackup PropertyBackup{ PropertyDesc.ID, PropertyDesc.Name, PropertyDesc.ValueTypeObject };
	PropertyBackup.SaveBackup<ParameterType>(Parameters, PropertyDesc);
	OutPropertyBackups.Add(PropertyBackup);
}

void FCameraParameterMigrationHelper::MigrateToNewBagInstanceWithOverrides(FInstancedOverridablePropertyBag& Parameters, const FInstancedPropertyBag& NewBagInstance)
{
	// The property bag's default migration implementation does convert a few types values, when a property keeps the
	// same ID but changes type. For instance, it supports migrating numeric values (e.g. from integer to float and
	// so on). Here we need to add support for camera parameters too (e.g. from float parameter to double parameter).
	// This is important to help users remove the deprecation warning about single-precision camera rig parameters.
	//
	// First, we save a backup of the camera parameter values. Then we do the default migration. Then we restore the
	// saved values.
	//
	TArray<FParameterPropertyBackup> PropertyBackups;
	const UPropertyBag* ParametersStruct = Parameters.GetPropertyBagStruct();

	if (ParametersStruct)
	{
		for (const FPropertyBagPropertyDesc& PropertyDesc : ParametersStruct->GetPropertyDescs())
		{
			if (PropertyDesc.ValueType == EPropertyBagPropertyType::Struct && PropertyDesc.ContainerTypes.Num() == 0)
			{
				// Convert from float-based camera parameter to double-based camera parameter.
				if (PropertyDesc.ValueTypeObject == FFloatCameraParameter::StaticStruct())
				{
					ProcessParameterPropertyDesc<FFloatCameraParameter>(Parameters, PropertyDesc, PropertyBackups);
				}
				else if (PropertyDesc.ValueTypeObject == FVector2fCameraParameter::StaticStruct())
				{
					ProcessParameterPropertyDesc<FVector2fCameraParameter>(Parameters, PropertyDesc, PropertyBackups);
				}
				else if (PropertyDesc.ValueTypeObject == FVector3fCameraParameter::StaticStruct())
				{
					ProcessParameterPropertyDesc<FVector3fCameraParameter>(Parameters, PropertyDesc, PropertyBackups);
				}
				else if (PropertyDesc.ValueTypeObject == FVector4fCameraParameter::StaticStruct())
				{
					ProcessParameterPropertyDesc<FVector4fCameraParameter>(Parameters, PropertyDesc, PropertyBackups);
				}
				else if (PropertyDesc.ValueTypeObject == FRotator3fCameraParameter::StaticStruct())
				{
					ProcessParameterPropertyDesc<FRotator3fCameraParameter>(Parameters, PropertyDesc, PropertyBackups);
				}
				else if (PropertyDesc.ValueTypeObject == FTransform3fCameraParameter::StaticStruct())
				{
					ProcessParameterPropertyDesc<FTransform3fCameraParameter>(Parameters, PropertyDesc, PropertyBackups);
				}
			}
		}
	}

	// Do the default migration.
	Parameters.MigrateToNewBagInstanceWithOverrides(NewBagInstance);

	// Restore the backed-up values.
	for (const FParameterPropertyBackup& PropertyBackup : PropertyBackups)
	{
		PropertyBackup.TryRestoreBackup(Parameters);
	}
}

}  // namespace UE::Cameras

