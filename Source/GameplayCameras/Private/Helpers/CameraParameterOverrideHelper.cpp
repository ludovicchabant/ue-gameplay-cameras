// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/CameraParameterOverrideHelper.h"

#include "Core/CameraContextDataTable.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigParameterDefinition.h"
#include "Core/CameraVariableTable.h"

namespace UE::Cameras
{

namespace Internal
{

template<typename ParameterType>
void ApplyBlendableParameterOverride(
		const UCameraRigAsset* CameraRig,
		const FCameraRigParameterDefinition& ParameterDefinition,
		const ParameterType& ParameterValue,
		FCameraVariableTable& VariableTable,
		bool bDrivenOverridesOnly)
{
	using ValueType = typename ParameterType::ValueType;

	const FCameraVariableID ParameterVariableID(ParameterDefinition.VariableID);
	if (ParameterValue.Variable != nullptr)
	{
		// The override is driven by a variable... read its value and set it as the value for the
		// prefab's variable. Basically, we forward the value from one variable to the next.
		FCameraVariableDefinition OverrideDefinition(ParameterValue.Variable->GetVariableDefinition());

		const ValueType OverrideValue = VariableTable.GetValue<ValueType>(
				OverrideDefinition.VariableID, ParameterValue.Variable->GetDefaultValue());
		VariableTable.SetValue<ValueType>(ParameterVariableID, OverrideValue);
	}
	else if (!bDrivenOverridesOnly)
	{
		// The override is a fixed value. Just set that on the prefab's variable.
		VariableTable.SetValue<ValueType>(ParameterVariableID, ParameterValue.Value);
	}
}

void ApplyBlendableParameterOverride(
		const UCameraRigAsset* CameraRig,
		const FCameraRigParameterDefinition& ParameterDefinition,
		const FInstancedPropertyBag& PropertyBag,
		const FPropertyBagPropertyDesc& PropertyBagPropertyDesc,
		FCameraVariableTable& VariableTable,
		bool bDrivenOverridesOnly)
{
	ensure(ParameterDefinition.ParameterType == ECameraRigInterfaceParameterType::Blendable);

	if (!ParameterDefinition.VariableID)
	{
		// Ignore un-built parameter overrides in the editor since the user could have just added
		// an override while PIE is running. They need to hit the Build button for the override
		// to apply.
		// Outside of the editor, report this as an error.
#if !WITH_EDITOR
		UE_LOG(LogCameraSystem, Error,
				TEXT("Invalid blendable parameter override '%s' in camera rig '%s'. Was it built/cooked?"),
				*ParameterDefinition.ParameterName.ToString(),
				*GetPathNameSafe(CameraRig));
#endif
		return;
	}

	TValueOrError<FStructView, EPropertyBagResult> ParameterValueOrError = PropertyBag.GetValueStruct(PropertyBagPropertyDesc);
	if (!ensureMsgf(
				ParameterValueOrError.HasValue() && !ParameterValueOrError.HasError(),
				TEXT("Camera parameter has no valid value! Error: %s"),
				*UEnum::GetValueAsString(ParameterValueOrError.GetError())))
	{
		return;
	}

	const FStructView& ParameterValue = ParameterValueOrError.GetValue();
	const UScriptStruct* ParameterType = ParameterValue.GetScriptStruct();

	switch (ParameterDefinition.VariableType)
	{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		case ECameraVariableType::ValueName:\
			{\
				check(ParameterType == F##ValueName##CameraParameter::StaticStruct());\
				const F##ValueName##CameraParameter& TypedParameterValue = ParameterValue.Get<F##ValueName##CameraParameter>();\
				ApplyBlendableParameterOverride(CameraRig, ParameterDefinition, TypedParameterValue, VariableTable, bDrivenOverridesOnly);\
			}\
			break;
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		case ECameraVariableType::BlendableStruct:
			{
				const uint8* RawValuePtr = ParameterValue.GetMemory();
				VariableTable.SetValue(ParameterDefinition.VariableID, ParameterDefinition.VariableType, ParameterDefinition.BlendableStructType, RawValuePtr);
			}
			break;
		default:
			ensure(false);
			break;
	}
}

template<typename ParameterType>
void OverrideContextDataTableEntry(
		const FCameraRigParameterDefinition& ParameterDefinition,
		const ParameterType& ParameterValue,
		FCameraContextDataTable& ContextDataTable)
{
	const uint8* RawParameterValue = reinterpret_cast<const uint8*>(&ParameterValue);
	ContextDataTable.SetData(ParameterDefinition.DataID, ParameterDefinition.DataType, ParameterDefinition.DataTypeObject, RawParameterValue);
}

template<>
void OverrideContextDataTableEntry<FStructView>(
		const FCameraRigParameterDefinition& ParameterDefinition,
		const FStructView& ParameterValue,
		FCameraContextDataTable& ContextDataTable)
{
	const uint8* RawParameterValue = ParameterValue.GetMemory();
	ContextDataTable.SetData(ParameterDefinition.DataID, ParameterDefinition.DataType, ParameterDefinition.DataTypeObject, RawParameterValue);
}

template<typename ParameterType>
void ApplyDataParameterOverride(
		const UCameraRigAsset* CameraRig,
		const FCameraRigParameterDefinition& ParameterDefinition,
		const TValueOrError<ParameterType, EPropertyBagResult>& ParameterValueOrError,
		FCameraContextDataTable& ContextDataTable)
{
	if (!ensureMsgf(
				ParameterValueOrError.HasValue() && !ParameterValueOrError.HasError(),
				TEXT("Camera parameter has no valid value! Error: %s"),
				*UEnum::GetValueAsString(ParameterValueOrError.GetError())))
	{
		return;
	}

	// Write the override value into the context data table.
	const ParameterType& ParameterValue = ParameterValueOrError.GetValue();
	OverrideContextDataTableEntry<ParameterType>(ParameterDefinition, ParameterValue, ContextDataTable);
}

void ApplyDataParameterOverride(
		const UCameraRigAsset* CameraRig,
		const FCameraRigParameterDefinition& ParameterDefinition,
		const FInstancedPropertyBag& PropertyBag,
		const FPropertyBagPropertyDesc& PropertyBagPropertyDesc,
		FCameraContextDataTable& ContextDataTable)
{
	ensure(ParameterDefinition.ParameterType == ECameraRigInterfaceParameterType::Data);

	if (!ParameterDefinition.DataID)
	{
#if !WITH_EDITOR
		UE_LOG(LogCameraSystem, Error,
				TEXT("Invalid data parameter override '%s' in camera rig '%s'. Was it built/cooked?"),
				*ParameterDefinition.ParameterName.ToString(),
				*GetPathNameSafe(CameraRig));
		return;
#endif
	}

	switch (ParameterDefinition.DataType)
	{
		case ECameraContextDataType::Name:
			{
				TValueOrError<FName, EPropertyBagResult> ParameterValueOrError = PropertyBag.GetValueName(PropertyBagPropertyDesc);
				ApplyDataParameterOverride(CameraRig, ParameterDefinition, ParameterValueOrError, ContextDataTable);
			}
			break;
		case ECameraContextDataType::String:
			{
				TValueOrError<FString, EPropertyBagResult> ParameterValueOrError = PropertyBag.GetValueString(PropertyBagPropertyDesc);
				ApplyDataParameterOverride(CameraRig, ParameterDefinition, ParameterValueOrError, ContextDataTable);
			}
			break;
		case ECameraContextDataType::Enum:
			{
				const UEnum* EnumType = Cast<const UEnum>(ParameterDefinition.DataTypeObject);
				if (ensure(EnumType))
				{
					TValueOrError<uint8, EPropertyBagResult> ParameterValueOrError = PropertyBag.GetValueEnum(PropertyBagPropertyDesc, EnumType);
					ApplyDataParameterOverride(CameraRig, ParameterDefinition, ParameterValueOrError, ContextDataTable);
				}
			}
			break;
		case ECameraContextDataType::Struct:
			{
				const UScriptStruct* StructType = Cast<const UScriptStruct>(ParameterDefinition.DataTypeObject);
				if (ensure(StructType))
				{
					TValueOrError<FStructView, EPropertyBagResult> ParameterValueOrError = PropertyBag.GetValueStruct(PropertyBagPropertyDesc, StructType);
					ApplyDataParameterOverride(CameraRig, ParameterDefinition, ParameterValueOrError, ContextDataTable);
				}
			}
			break;
		case ECameraContextDataType::Object:
			{
				TValueOrError<UObject*, EPropertyBagResult> ParameterValueOrError = PropertyBag.GetValueObject(PropertyBagPropertyDesc);
				ApplyDataParameterOverride(CameraRig, ParameterDefinition, ParameterValueOrError, ContextDataTable);
			}
			break;
		case ECameraContextDataType::Class:
			{
				TValueOrError<UClass*, EPropertyBagResult> ParameterValueOrError = PropertyBag.GetValueClass(PropertyBagPropertyDesc);
				ApplyDataParameterOverride(CameraRig, ParameterDefinition, ParameterValueOrError, ContextDataTable);
			}
			break;
		default:
			ensure(false);
			break;
	}
}

}  // namespace Internal

FCameraParameterOverrideHelper::FCameraParameterOverrideHelper(FCameraVariableTable& OutVariableTable, FCameraContextDataTable& OutContextDataTable)
	: VariableTable(OutVariableTable)
	, ContextDataTable(OutContextDataTable)
{
}

void FCameraParameterOverrideHelper::ApplyParameterOverride(
		const UCameraRigAsset* CameraRig,
		const FCameraRigParameterDefinition& ParameterDefinition,
		const FInstancedPropertyBag& PropertyBag,
		const FPropertyBagPropertyDesc& PropertyBagPropertyDesc,
		bool bDrivenOverridesOnly)
{
	using namespace Internal;

	switch (ParameterDefinition.ParameterType)
	{
		case ECameraRigInterfaceParameterType::Blendable:
			{
				ApplyBlendableParameterOverride(
						CameraRig, 
						ParameterDefinition,
						PropertyBag, PropertyBagPropertyDesc, 
						VariableTable, bDrivenOverridesOnly);
			}
			break;
		case ECameraRigInterfaceParameterType::Data:
			{
				ApplyDataParameterOverride(
						CameraRig, 
						ParameterDefinition,
						PropertyBag, PropertyBagPropertyDesc, 
						ContextDataTable);
			}
			break;
	}
}

void FCameraParameterOverrideHelper::ApplyDefaultBlendableParameters(const UCameraRigAsset* CameraRig, FCameraVariableTable& OutVariableTable)
{
	if (!ensure(CameraRig))
	{
		return;
	}

	const FInstancedPropertyBag& DefaultParameters = CameraRig->GetDefaultParameters();
	const uint8* RawDefaultParametersContainer = DefaultParameters.GetValue().GetMemory();

	for (const FCameraRigParameterDefinition& Definition : CameraRig->GetParameterDefinitions())
	{
		if (Definition.ParameterType != ECameraRigInterfaceParameterType::Blendable)
		{
			continue;
		}
		if (!Definition.VariableID.IsValid())
		{
			continue;
		}

		const FPropertyBagPropertyDesc* PropertyDesc = DefaultParameters.FindPropertyDescByID(Definition.ParameterGuid);
		if (!ensure(PropertyDesc && PropertyDesc->CachedProperty))
		{
			continue;
		}

		const void* RawValuePtr = PropertyDesc->CachedProperty->ContainerPtrToValuePtr<void>(RawDefaultParametersContainer);
		OutVariableTable.SetValue(Definition.VariableID, Definition.VariableType, Definition.BlendableStructType, (const uint8*)RawValuePtr);
	}
}

}  // namespace UE::Cameras

