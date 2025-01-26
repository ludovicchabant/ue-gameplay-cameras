// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigParameterDefinition.h"

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "StructUtils/PropertyBag.h"

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
		const UObject* PropertyTypeObject = nullptr;

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
					break;
				case ECameraContextDataType::String:
					PropertyType = EPropertyBagPropertyType::String;
					break;
				case ECameraContextDataType::Enum:
					PropertyType = EPropertyBagPropertyType::Enum;
					ensure(PropertyTypeObject && PropertyTypeObject->IsA<UEnum>());
					break;
				case ECameraContextDataType::Struct:
					PropertyType = EPropertyBagPropertyType::Struct;
					ensure(PropertyTypeObject && PropertyTypeObject->IsA<UScriptStruct>());
					break;
				case ECameraContextDataType::Object:
					PropertyType = EPropertyBagPropertyType::Object;
					break;
				case ECameraContextDataType::Class:
					PropertyType = EPropertyBagPropertyType::Class;
					break;
				default:
					bIsValidProperty = false;
					break;
			}
		}
		else
		{
			bIsValidProperty = false;
		}

		if (ensure(bIsValidProperty))
		{
			FPropertyBagPropertyDesc NewProperty(Definition.ParameterName, PropertyType, PropertyTypeObject);
			// Make the property bag match the camera interface parameter GUIDs.
			NewProperty.ID = Definition.ParameterGuid;

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

		const uint8* RawSourceValuePtr = nullptr;

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
							if (StructProperty->Struct == CameraParameterType::StaticStruct())\
							{\
								auto* CameraParameterPtr = StructProperty->ContainerPtrToValuePtr<CameraParameterType>(CameraNode);\
								RawSourceValuePtr = reinterpret_cast<uint8*>(&CameraParameterPtr->Value);\
							}\
						}\
						break;
					UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
				}
			}
		}

		if (!ensure(RawSourceValuePtr))
		{
			continue;
		}

		const FPropertyBagPropertyDesc* PropertyDesc = PropertyBagStruct->FindPropertyDescByID(BlendableParameter->GetGuid());
		if (!ensure(PropertyDesc && PropertyDesc->CachedProperty))
		{
			continue;
		}

		void* RawDestinationValuePtr = PropertyDesc->CachedProperty->ContainerPtrToValuePtr<void>(PropertyBagValue);

		if (!ensure(RawDestinationValuePtr))
		{
			continue;
		}

		switch (BlendableParameter->ParameterType)
		{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
			case ECameraVariableType::ValueName:\
				{\
					using CameraParameterType = F##ValueName##CameraParameter;\
					const ValueType* SourceValuePtr = reinterpret_cast<const ValueType*>(RawSourceValuePtr);\
					CameraParameterType* DestinationParameter = reinterpret_cast<CameraParameterType*>(RawDestinationValuePtr);\
					DestinationParameter->Value = *SourceValuePtr;\
				}\
				break;
			UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
			case ECameraVariableType::BlendableStruct:
				if (ensure(BlendableParameter->BlendableStructType))
				{
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
				RawSourceValuePtr = (uint8*)Property->ContainerPtrToValuePtr<void>(TargetClass);
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

		void* RawDestinationValuePtr = PropertyDesc->CachedProperty->ContainerPtrToValuePtr<void>(PropertyBagValue);

		if (!ensure(RawDestinationValuePtr))
		{
			continue;
		}

		switch (DataParameter->DataType)
		{
			case ECameraContextDataType::Name:
				*((FName*)RawDestinationValuePtr) = *((FName*)RawSourceValuePtr);
				break;
			case ECameraContextDataType::String:
				*((FString*)RawDestinationValuePtr) = *((FString*)RawSourceValuePtr);
				break;
			case ECameraContextDataType::Enum:
				*((uint8*)RawDestinationValuePtr) = *((uint8*)RawSourceValuePtr);
				break;
			case ECameraContextDataType::Struct:
				{
					const UScriptStruct* StructType = CastChecked<const UScriptStruct>(DataParameter->DataTypeObject);
					StructType->CopyScriptStruct(RawDestinationValuePtr, RawSourceValuePtr);
				}
				break;
			case ECameraContextDataType::Object:
				*((FObjectPtr*)RawDestinationValuePtr) = *((FObjectPtr*)RawSourceValuePtr);
				break;
			case ECameraContextDataType::Class:
				*((FObjectPtr*)RawDestinationValuePtr) = *((FObjectPtr*)RawSourceValuePtr);
				break;
		}
	}
}

}  // namespace UE::Cameras

