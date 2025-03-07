// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BaseCameraObjectReference.h"

#include "Core/BaseCameraObject.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraParameters.h"
#include "Core/ICustomCameraNodeParameterProvider.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseCameraObjectReference)

const FCameraObjectInterfaceParameterMetaData* FBaseCameraObjectReference::FindMetaData(const FGuid& PropertyID) const
{
	return ParameterMetaData.FindByPredicate(
			[PropertyID](const FCameraObjectInterfaceParameterMetaData& Item)
			{
				return Item.ParameterGuid == PropertyID;
			});
}

FCameraObjectInterfaceParameterMetaData& FBaseCameraObjectReference::FindOrAddMetaData(const FGuid& PropertyID)
{
	FCameraObjectInterfaceParameterMetaData* ExistingMetaData = ParameterMetaData.FindByPredicate(
			[PropertyID](FCameraObjectInterfaceParameterMetaData& Item)
			{
				return Item.ParameterGuid == PropertyID;
			});
	if (ExistingMetaData)
	{
		return *ExistingMetaData;
	}

	FCameraObjectInterfaceParameterMetaData& NewMetaData = ParameterMetaData.Emplace_GetRef();
	NewMetaData.ParameterGuid = PropertyID;
	return NewMetaData;
}

bool FBaseCameraObjectReference::IsParameterOverridden(const FGuid& PropertyID) const
{
	if (const FCameraObjectInterfaceParameterMetaData* MetaData = FindMetaData(PropertyID))
	{
		return MetaData->bIsOverridden;
	}
	return false;
}

void FBaseCameraObjectReference::SetParameterOverridden(const FGuid& PropertyID, bool bIsOverridden)
{
	FCameraObjectInterfaceParameterMetaData& MetaData = FindOrAddMetaData(PropertyID);
	MetaData.bIsOverridden = bIsOverridden;
}

bool FBaseCameraObjectReference::IsParameterAnimated(const FGuid& PropertyID) const
{
	if (const FCameraObjectInterfaceParameterMetaData* MetaData = FindMetaData(PropertyID))
	{
		return MetaData->bIsAnimated;
	}
	return false;
}

void FBaseCameraObjectReference::SetParameterAnimated(const FGuid& PropertyID, bool bIsAnimated)
{
	FCameraObjectInterfaceParameterMetaData& MetaData = FindOrAddMetaData(PropertyID);
	MetaData.bIsAnimated = bIsAnimated;
}

bool FBaseCameraObjectReference::NeedsRebuildParameters() const
{
	const UBaseCameraObject* CameraObject = GetCameraObject();

	if ((!CameraObject && Parameters.IsValid()) || (CameraObject && !Parameters.IsValid()))
	{
		return true;
	}

	if (CameraObject)
	{
		const UPropertyBag* AssetParametersType = CameraObject->GetDefaultParameters().GetPropertyBagStruct();
		const UPropertyBag* ReferenceParametersType = Parameters.GetPropertyBagStruct();
		if (AssetParametersType != ReferenceParametersType)
		{
			return true;
		}
	}

	return false;
}

bool FBaseCameraObjectReference::RebuildParametersIfNeeded()
{
	if (NeedsRebuildParameters())
	{
		RebuildParameters();
		return true;
	}
	return false;
}

void FBaseCameraObjectReference::RebuildParameters()
{
	const UBaseCameraObject* CameraObject = GetCameraObject();

	if (CameraObject)
	{
		TArray<FGuid> ParameterOverrideGuids;
		GetOverriddenParameterGuids(ParameterOverrideGuids);
		Parameters.MigrateToNewBagInstanceWithOverrides(CameraObject->GetDefaultParameters(), ParameterOverrideGuids);

		if (const UPropertyBag* ParametersType = Parameters.GetPropertyBagStruct())
		{
			// Remove metadata for parameters that don't exist anymore, and add default metadata for
			// new parameters.
			TSet<FGuid> ExistingMetaDataIDs;
			for (TArray<FCameraObjectInterfaceParameterMetaData>::TIterator It = ParameterMetaData.CreateIterator(); It; ++It)
			{
				ExistingMetaDataIDs.Add(It->ParameterGuid);
			}

			TSet<FGuid> WantedMetaDataIDs;
			TMap<FGuid, ECameraObjectInterfaceParameterType> WantedParameterTypes;
			for (const FCameraObjectInterfaceParameterDefinition& Definition : CameraObject->GetParameterDefinitions())
			{
				WantedMetaDataIDs.Add(Definition.ParameterGuid);
				WantedParameterTypes.Add(Definition.ParameterGuid, Definition.ParameterType);
			}

			TSet<FGuid> RemovedMetaDataIDs = ExistingMetaDataIDs.Difference(WantedMetaDataIDs);
			for (TArray<FCameraObjectInterfaceParameterMetaData>::TIterator It = ParameterMetaData.CreateIterator(); It; ++It)
			{
				if (RemovedMetaDataIDs.Contains(It->ParameterGuid))
				{
					It.RemoveCurrentSwap();
				}
			}

			TSet<FGuid> AddedMetaDataIDs = WantedMetaDataIDs.Difference(ExistingMetaDataIDs);
			for (const FGuid& ParameterGuid : AddedMetaDataIDs)
			{
				FCameraObjectInterfaceParameterMetaData NewMetaData;
				NewMetaData.ParameterGuid = ParameterGuid;
				ParameterMetaData.Add(NewMetaData);
			}
		}
		else
		{
			ParameterMetaData.Reset();
		}
	}
	else
	{
		Parameters.Reset();
		ParameterMetaData.Reset();
	}
}

void FBaseCameraObjectReference::GetCustomCameraNodeParameters(FCustomCameraNodeParameterInfos& OutParameterInfos)
{
	RebuildParametersIfNeeded();

	const UBaseCameraObject* CameraObject = GetCameraObject();
	if (!CameraObject)
	{
		return;
	}

	const UPropertyBag* ParametersStruct = Parameters.GetPropertyBagStruct();
	const FInstancedPropertyBag& DefaultParameters = CameraObject->GetDefaultParameters();
	if (!ensure(ParametersStruct && ParametersStruct == DefaultParameters.GetPropertyBagStruct()))
	{
		return;
	}

	for (const FCameraObjectInterfaceParameterDefinition& Definition : CameraObject->GetParameterDefinitions())
	{
		const FPropertyBagPropertyDesc* PropertyDesc = ParametersStruct->FindPropertyDescByID(Definition.ParameterGuid);
		if (!ensure(PropertyDesc))
		{
			continue;
		}

		if (Definition.ParameterType == ECameraObjectInterfaceParameterType::Blendable)
		{
			if (!ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Struct))
			{
				continue;
			}

			switch (Definition.VariableType)
			{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
				case ECameraVariableType::ValueName:\
					if (ensure(PropertyDesc->ValueTypeObject == F##ValueName##CameraParameter::StaticStruct()))\
					{\
						using CameraParameterType = F##ValueName##CameraParameter;\
						TValueOrError<CameraParameterType*, EPropertyBagResult> PropertyValue =\
							Parameters.GetValueStruct<CameraParameterType>(*PropertyDesc);\
						if (ensure(PropertyValue.HasValue() && !PropertyValue.HasError()))\
						{\
							CameraParameterType* CameraParameter = PropertyValue.GetValue();\
							check(CameraParameter);\
							OutParameterInfos.AddBlendableParameter(\
									Definition.ParameterName,\
									Definition.VariableType,\
									nullptr,\
									reinterpret_cast<uint8*>(&CameraParameter->Value),\
									&CameraParameter->VariableID);\
						}\
					}\
					break;
				UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
				case ECameraVariableType::BlendableStruct:
					{
						TValueOrError<FStructView, EPropertyBagResult> PropertyValue =
							Parameters.GetValueStruct(*PropertyDesc, Definition.BlendableStructType);
						if (ensure(PropertyValue.HasValue() && !PropertyValue.HasError()))
						{
							FStructView& StructValue = PropertyValue.GetValue();
							check(StructValue.IsValid());

							FCameraObjectInterfaceParameterMetaData& MetaData = FindOrAddMetaData(Definition.ParameterGuid);
							OutParameterInfos.AddBlendableParameter(
									Definition.ParameterName,
									Definition.VariableType,
									Definition.BlendableStructType,
									StructValue.GetMemory(),
									&MetaData.OverrideVariableID);
						}
					}
					break;
			}
		}
		else if (Definition.ParameterType == ECameraObjectInterfaceParameterType::Data)
		{
			FCameraObjectInterfaceParameterMetaData& MetaData = FindOrAddMetaData(Definition.ParameterGuid);

			switch (Definition.DataType)
			{
				case ECameraContextDataType::Name:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Name))
					{
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueName(*PropertyDesc).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::Name, ECameraContextDataContainerType::None, nullptr, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
				case ECameraContextDataType::String:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::String))
					{
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueString(*PropertyDesc).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::String, ECameraContextDataContainerType::None, nullptr, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
				case ECameraContextDataType::Enum:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Enum &&
								PropertyDesc->ValueTypeObject == Definition.DataTypeObject))
					{
						const UEnum* EnumType = CastChecked<const UEnum>(Definition.DataTypeObject);
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueEnum(*PropertyDesc, EnumType).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::Enum, ECameraContextDataContainerType::None, EnumType, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
				case ECameraContextDataType::Struct:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Struct &&
								PropertyDesc->ValueTypeObject == Definition.DataTypeObject))
					{
						const UScriptStruct* DataType = CastChecked<const UScriptStruct>(Definition.DataTypeObject);
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueStruct(*PropertyDesc, DataType).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::Struct, ECameraContextDataContainerType::None, DataType, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
				case ECameraContextDataType::Object:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Object))
					{
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueObject(*PropertyDesc).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::Object, ECameraContextDataContainerType::None, Definition.DataTypeObject, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
				case ECameraContextDataType::Class:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Class))
					{
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueClass(*PropertyDesc).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::Class, ECameraContextDataContainerType::None, Definition.DataTypeObject, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
			}
		}
	}
}

