// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigParameterDefinition.h"

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "StructUtils/PropertyBag.h"
#include "UObject/UnrealType.h"

namespace UE::Cameras
{

void FCameraRigParameterBuilder::BuildDefaultParameters(const UCameraRigAsset* CameraRig, FInstancedPropertyBag& OutPropertyBag)
{
	TArray<FPropertyBagPropertyDesc> DefaultParameterProperties;
	AppendDefaultParameterProperties(CameraRig, DefaultParameterProperties);
	OutPropertyBag.AddProperties(DefaultParameterProperties);
	SetDefaultParameterValues(CameraRig, OutPropertyBag);
}

void FCameraRigParameterBuilder::AppendDefaultParameterProperties(const UCameraRigAsset* CameraRig, TArray<FPropertyBagPropertyDesc>& OutProperties)
{
	for (const FCameraRigParameterDefinition& Definition : CameraRig->GetParameterDefinitions())
	{
		bool bIsValidProperty = true;
		EPropertyBagPropertyType PropertyType = EPropertyBagPropertyType::Struct;
		EPropertyBagContainerType ContainerType = EPropertyBagContainerType::None;
		const UObject* PropertyTypeObject = nullptr;
		EPropertyFlags PropertyFlags = CPF_None;

		if (Definition.ParameterType == ECameraRigInterfaceParameterType::Blendable)
		{
			switch (Definition.VariableType)
			{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
				case ECameraVariableType::ValueName:\
					PropertyTypeObject = F##ValueName##CameraParameter::StaticStruct();\
					break;
				UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
				case ECameraVariableType::BlendableStruct:
					PropertyTypeObject = Definition.BlendableStructType;
					PropertyFlags = CPF_Interp;
					break;
				default:
					ensure(false);
					break;
			}
		}
		else if (Definition.ParameterType == ECameraRigInterfaceParameterType::Data)
		{
			PropertyTypeObject = Definition.DataTypeObject;

			switch (Definition.DataType)
			{
				case ECameraContextDataType::Name:
					PropertyType = EPropertyBagPropertyType::Name;
					PropertyFlags = CPF_Interp;
					break;
				case ECameraContextDataType::String:
					PropertyType = EPropertyBagPropertyType::String;
					PropertyFlags = CPF_Interp;
					break;
				case ECameraContextDataType::Enum:
					PropertyType = EPropertyBagPropertyType::Enum;
					ensure(PropertyTypeObject && PropertyTypeObject->IsA<UEnum>());
					PropertyFlags = CPF_Interp;
					break;
				case ECameraContextDataType::Struct:
					PropertyType = EPropertyBagPropertyType::Struct;
					ensure(PropertyTypeObject && PropertyTypeObject->IsA<UScriptStruct>());
					break;
				case ECameraContextDataType::Object:
					PropertyType = EPropertyBagPropertyType::Object;
					PropertyFlags = CPF_Interp;
					break;
				case ECameraContextDataType::Class:
					PropertyType = EPropertyBagPropertyType::Class;
					PropertyFlags = CPF_Interp;
					break;
				default:
					bIsValidProperty = false;
					break;
			}

			switch (Definition.DataContainerType)
			{
				case ECameraContextDataContainerType::Array:
					ContainerType = EPropertyBagContainerType::Array;
					break;
			}
		}
		else
		{
			bIsValidProperty = false;
		}

		if (ensure(bIsValidProperty))
		{
			FPropertyBagPropertyDesc NewProperty(Definition.ParameterName, ContainerType, PropertyType, PropertyTypeObject);
			// Make the property bag match the camera interface parameter GUIDs.
			NewProperty.ID = Definition.ParameterGuid;
			NewProperty.PropertyFlags |= PropertyFlags;

			OutProperties.Add(NewProperty);
		}
	}
}

void FCameraRigParameterBuilder::SetDefaultParameterValues(const UCameraRigAsset* CameraRig, FInstancedPropertyBag& PropertyBag)
{
	uint8* PropertyBagValue = PropertyBag.GetMutableValue().GetMemory();
	const UPropertyBag* PropertyBagStruct = PropertyBag.GetPropertyBagStruct();
	if (!ensure(PropertyBagValue && PropertyBagStruct))
	{
		return;
	}

	for (const UCameraRigBlendableParameter* BlendableParameter : CameraRig->Interface.BlendableParameters)
	{
		if (!ensure(BlendableParameter))
		{
			continue;
		}

		UCameraNode* CameraNode = BlendableParameter->Target;
		if (!CameraNode)
		{
			continue;
		}

		const void* RawSourceValuePtr = nullptr;

		// First check if the value is found on a custom parameter.
		if (ICustomCameraNodeParameterProvider* CustomParameterProvider = Cast<ICustomCameraNodeParameterProvider>(CameraNode))
		{
			FCustomCameraNodeParameterInfos CustomParameters;
			CustomParameterProvider->GetCustomCameraNodeParameters(CustomParameters);

			FCustomCameraNodeParameterInfos::FBlendableParameterInfo* TargetCustomParameter = 
				CustomParameters.BlendableParameters.FindByPredicate(
						[BlendableParameter](FCustomCameraNodeParameterInfos::FBlendableParameterInfo& CustomParameter)
						{
							return CustomParameter.ParameterName == BlendableParameter->TargetPropertyName;
						});
			if (TargetCustomParameter)
			{
				RawSourceValuePtr = TargetCustomParameter->DefaultValue;
			}
		}

		// If not found, check on a reflected UObject property.
		if (!RawSourceValuePtr)
		{
			const UClass* TargetClass = CameraNode->GetClass();
			FStructProperty* StructProperty = CastField<FStructProperty>(
					TargetClass->FindPropertyByName(BlendableParameter->TargetPropertyName));
			if (StructProperty)
			{
				switch (BlendableParameter->ParameterType)
				{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
					case ECameraVariableType::ValueName:\
						{\
							using CameraParameterType = F##ValueName##CameraParameter;\
							if (ensure(StructProperty->Struct == CameraParameterType::StaticStruct()))\
							{\
								CameraParameterType* CameraParameterPtr = StructProperty->ContainerPtrToValuePtr<CameraParameterType>(CameraNode);\
								RawSourceValuePtr = static_cast<void*>(&CameraParameterPtr->Value);\
							}\
						}\
						break;
					UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
					case ECameraVariableType::BlendableStruct:
						{
							RawSourceValuePtr = StructProperty->ContainerPtrToValuePtr<void>(CameraNode);
						}
						break;
				}
			}
		}

		if (!ensure(RawSourceValuePtr))
		{
			continue;
		}

		// Find the corresponding property on the default parameters' property bag.
		const FPropertyBagPropertyDesc* PropertyDesc = PropertyBagStruct->FindPropertyDescByID(BlendableParameter->GetGuid());
		if (!ensure(PropertyDesc && PropertyDesc->CachedProperty))
		{
			continue;
		}

		// This property should be a structure: either a camera parameter for all the standard blendable types,
		// or a blendable structure.
		const FStructProperty* DefaultParameterProperty = CastField<FStructProperty>(PropertyDesc->CachedProperty);
		if (!ensure(DefaultParameterProperty))
		{
			continue;
		}

		switch (BlendableParameter->ParameterType)
		{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
			case ECameraVariableType::ValueName:\
				{\
					using CameraParameterType = F##ValueName##CameraParameter;\
					if (ensure(DefaultParameterProperty->Struct == CameraParameterType::StaticStruct()))\
					{\
						const ValueType* SourceValuePtr = reinterpret_cast<const ValueType*>(RawSourceValuePtr);\
						CameraParameterType* DestinationParameter = DefaultParameterProperty->ContainerPtrToValuePtr<CameraParameterType>(PropertyBagValue);\
						DestinationParameter->Value = *SourceValuePtr;\
					}\
				}\
				break;
			UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
			case ECameraVariableType::BlendableStruct:
				if (ensure(DefaultParameterProperty->Struct == BlendableParameter->BlendableStructType))
				{
					void* RawDestinationValuePtr = DefaultParameterProperty->ContainerPtrToValuePtr<void>(PropertyBagValue);
					BlendableParameter->BlendableStructType->CopyScriptStruct(RawDestinationValuePtr, RawSourceValuePtr);
				}
				break;
		}
	}

	for (const UCameraRigDataParameter* DataParameter : CameraRig->Interface.DataParameters)
	{
		if (!ensure(DataParameter && DataParameter->Target))
		{
			continue;
		}

		UCameraNode* CameraNode = DataParameter->Target;

		const uint8* RawSourceValuePtr = nullptr;

		if (ICustomCameraNodeParameterProvider* CustomParameterProvider = Cast<ICustomCameraNodeParameterProvider>(CameraNode))
		{
			FCustomCameraNodeParameterInfos CustomParameters;
			CustomParameterProvider->GetCustomCameraNodeParameters(CustomParameters);

			FCustomCameraNodeParameterInfos::FDataParameterInfo* TargetCustomParameter = 
				CustomParameters.DataParameters.FindByPredicate(
						[DataParameter](FCustomCameraNodeParameterInfos::FDataParameterInfo& CustomParameter)
						{
							return CustomParameter.ParameterName == DataParameter->TargetPropertyName;
						});
			if (TargetCustomParameter)
			{
				RawSourceValuePtr = TargetCustomParameter->DefaultValue;
			}
		}

		if (!RawSourceValuePtr)
		{
			const UClass* TargetClass = DataParameter->Target->GetClass();
			FProperty* Property = TargetClass->FindPropertyByName(DataParameter->TargetPropertyName);
			if (Property)
			{
				RawSourceValuePtr = (uint8*)Property->ContainerPtrToValuePtr<void>(DataParameter->Target);
			}
		}

		if (!ensure(RawSourceValuePtr))
		{
			continue;
		}

		const FPropertyBagPropertyDesc* PropertyDesc = PropertyBagStruct->FindPropertyDescByID(DataParameter->GetGuid());
		if (!ensure(PropertyDesc && PropertyDesc->CachedProperty))
		{
			continue;
		}

		if (DataParameter->DataContainerType == ECameraContextDataContainerType::None)
		{
			void* RawDestinationValuePtr = PropertyDesc->CachedProperty->ContainerPtrToValuePtr<void>(PropertyBagValue);
			if (ensure(RawDestinationValuePtr))
			{
				SetDefaultParameterValue(DataParameter, RawDestinationValuePtr, RawSourceValuePtr);
			}
		}
		else if (DataParameter->DataContainerType == ECameraContextDataContainerType::Array)
		{
			// Array properties are empty by default.
		}
	}
}

void FCameraRigParameterBuilder::SetDefaultParameterValue(const UCameraRigDataParameter* DataParameter, void* DestValuePtr, const void* SrcValuePtr)
{
	switch (DataParameter->DataType)
	{
		case ECameraContextDataType::Name:
			*((FName*)DestValuePtr) = *((FName*)SrcValuePtr);
			break;
		case ECameraContextDataType::String:
			*((FString*)DestValuePtr) = *((FString*)SrcValuePtr);
			break;
		case ECameraContextDataType::Enum:
			*((uint8*)DestValuePtr) = *((uint8*)SrcValuePtr);
			break;
		case ECameraContextDataType::Struct:
			{
				const UScriptStruct* StructType = CastChecked<const UScriptStruct>(DataParameter->DataTypeObject);
				StructType->CopyScriptStruct(DestValuePtr, SrcValuePtr);
			}
			break;
		case ECameraContextDataType::Object:
			*((FObjectPtr*)DestValuePtr) = *((FObjectPtr*)SrcValuePtr);
			break;
		case ECameraContextDataType::Class:
			*((FObjectPtr*)DestValuePtr) = *((FObjectPtr*)SrcValuePtr);
			break;
	}
}

}  // namespace UE::Cameras

