// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAssetReference.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "Helpers/CameraRigParameterOverrideEvaluator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigAssetReference)

PRAGMA_DISABLE_DEPRECATION_WARNINGS

FCameraRigAssetReference::FCameraRigAssetReference()
{
}

FCameraRigAssetReference::FCameraRigAssetReference(UCameraRigAsset* InCameraRig)
	: CameraRig(InCameraRig)
{
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS

void FCameraRigAssetReference::ApplyParameterOverrides(UE::Cameras::FCameraNodeEvaluationResult& OutResult, bool bDrivenOverridesOnly)
{
	using namespace UE::Cameras;
	FCameraRigParameterOverrideEvaluator OverrideEvaluator(*this);
	OverrideEvaluator.ApplyParameterOverrides(OutResult.VariableTable, OutResult.ContextDataTable, bDrivenOverridesOnly);
}

const FCameraRigAssetReferenceParameterMetaData* FCameraRigAssetReference::FindMetaData(const FGuid& PropertyID) const
{
	return ParameterMetaData.FindByPredicate(
			[PropertyID](const FCameraRigAssetReferenceParameterMetaData& Item)
			{
				return Item.ParameterGuid == PropertyID;
			});
}

FCameraRigAssetReferenceParameterMetaData& FCameraRigAssetReference::FindOrAddMetaData(const FGuid& PropertyID)
{
	FCameraRigAssetReferenceParameterMetaData* ExistingMetaData = ParameterMetaData.FindByPredicate(
			[PropertyID](FCameraRigAssetReferenceParameterMetaData& Item)
			{
				return Item.ParameterGuid == PropertyID;
			});
	if (ExistingMetaData)
	{
		return *ExistingMetaData;
	}

	FCameraRigAssetReferenceParameterMetaData& NewMetaData = ParameterMetaData.Emplace_GetRef();
	NewMetaData.ParameterGuid = PropertyID;
	return NewMetaData;
}

void FCameraRigAssetReference::GenerateOverriddenParameterGuidArray(TArray<FGuid>& OutOverriddenIDs) const
{
	for (const FCameraRigAssetReferenceParameterMetaData& MetaData : ParameterMetaData)
	{
		if (MetaData.bIsOverridden)
		{
			OutOverriddenIDs.Add(MetaData.ParameterGuid);
		}
	}
}

bool FCameraRigAssetReference::IsParameterOverridden(const FGuid& PropertyID) const
{
	if (const FCameraRigAssetReferenceParameterMetaData* MetaData = FindMetaData(PropertyID))
	{
		return MetaData->bIsOverridden;
	}
	return false;
}

void FCameraRigAssetReference::SetParameterOverridden(const FGuid& PropertyID, bool bIsOverridden)
{
	FCameraRigAssetReferenceParameterMetaData& MetaData = FindOrAddMetaData(PropertyID);
	MetaData.bIsOverridden = bIsOverridden;
}

bool FCameraRigAssetReference::IsParameterAnimated(const FGuid& PropertyID) const
{
	if (const FCameraRigAssetReferenceParameterMetaData* MetaData = FindMetaData(PropertyID))
	{
		return MetaData->bIsAnimated;
	}
	return false;
}

void FCameraRigAssetReference::SetParameterAnimated(const FGuid& PropertyID, bool bIsAnimated)
{
	FCameraRigAssetReferenceParameterMetaData& MetaData = FindOrAddMetaData(PropertyID);
	MetaData.bIsAnimated = bIsAnimated;
}

bool FCameraRigAssetReference::NeedsRebuildParameters() const
{
	if ((!CameraRig && Parameters.IsValid()) || (CameraRig && !Parameters.IsValid()))
	{
		return true;
	}

	if (CameraRig)
	{
		const UPropertyBag* AssetParametersType = CameraRig->GetDefaultParameters().GetPropertyBagStruct();
		const UPropertyBag* ReferenceParametersType = Parameters.GetPropertyBagStruct();
		if (AssetParametersType != ReferenceParametersType)
		{
			return true;
		}
	}

	return false;
}

bool FCameraRigAssetReference::RebuildParametersIfNeeded()
{
	if (NeedsRebuildParameters())
	{
		RebuildParameters();
		return true;
	}
	return false;
}

void FCameraRigAssetReference::RebuildParameters()
{
	if (CameraRig)
	{
		TArray<FGuid> ParameterOverrideGuids;
		GenerateOverriddenParameterGuidArray(ParameterOverrideGuids);
		Parameters.MigrateToNewBagInstanceWithOverrides(CameraRig->GetDefaultParameters(), ParameterOverrideGuids);

		if (const UPropertyBag* ParametersType = Parameters.GetPropertyBagStruct())
		{
			// Remove metadata for parameters that don't exist anymore, and add default metadata for
			// new parameters.
			TSet<FGuid> ExistingMetaDataIDs;
			for (TArray<FCameraRigAssetReferenceParameterMetaData>::TIterator It = ParameterMetaData.CreateIterator(); It; ++It)
			{
				ExistingMetaDataIDs.Add(It->ParameterGuid);
			}

			TSet<FGuid> WantedMetaDataIDs;
			TMap<FGuid, ECameraRigInterfaceParameterType> WantedParameterTypes;
			for (const FCameraRigParameterDefinition& Definition : CameraRig->GetParameterDefinitions())
			{
				WantedMetaDataIDs.Add(Definition.ParameterGuid);
				WantedParameterTypes.Add(Definition.ParameterGuid, Definition.ParameterType);
			}

			TSet<FGuid> RemovedMetaDataIDs = ExistingMetaDataIDs.Difference(WantedMetaDataIDs);
			for (TArray<FCameraRigAssetReferenceParameterMetaData>::TIterator It = ParameterMetaData.CreateIterator(); It; ++It)
			{
				if (RemovedMetaDataIDs.Contains(It->ParameterGuid))
				{
					It.RemoveCurrentSwap();
				}
			}

			TSet<FGuid> AddedMetaDataIDs = WantedMetaDataIDs.Difference(ExistingMetaDataIDs);
			for (const FGuid& ParameterGuid : AddedMetaDataIDs)
			{
				FCameraRigAssetReferenceParameterMetaData NewMetaData;
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

void FCameraRigAssetReference::GetCustomCameraNodeParameters(FCustomCameraNodeParameterInfos& OutParameterInfos)
{
	if (!CameraRig)
	{
		return;
	}

	RebuildParametersIfNeeded();

	const UPropertyBag* ParametersStruct = Parameters.GetPropertyBagStruct();
	const FInstancedPropertyBag& DefaultParameters = CameraRig->GetDefaultParameters();
	if (!ensure(ParametersStruct && ParametersStruct == DefaultParameters.GetPropertyBagStruct()))
	{
		return;
	}

	for (const FCameraRigParameterDefinition& Definition : CameraRig->GetParameterDefinitions())
	{
		const FPropertyBagPropertyDesc* PropertyDesc = ParametersStruct->FindPropertyDescByID(Definition.ParameterGuid);
		if (!ensure(PropertyDesc))
		{
			continue;
		}

		if (Definition.ParameterType == ECameraRigInterfaceParameterType::Blendable)
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

							FCameraRigAssetReferenceParameterMetaData& MetaData = FindOrAddMetaData(Definition.ParameterGuid);
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
		else if (Definition.ParameterType == ECameraRigInterfaceParameterType::Data)
		{
			FCameraRigAssetReferenceParameterMetaData& MetaData = FindOrAddMetaData(Definition.ParameterGuid);

			switch (Definition.DataType)
			{
				case ECameraContextDataType::Name:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Name))
					{
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueName(*PropertyDesc).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::Name, nullptr, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
				case ECameraContextDataType::String:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::String))
					{
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueString(*PropertyDesc).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::String, nullptr, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
				case ECameraContextDataType::Enum:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Enum &&
								PropertyDesc->ValueTypeObject == Definition.DataTypeObject))
					{
						const UEnum* EnumType = CastChecked<const UEnum>(Definition.DataTypeObject);
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueEnum(*PropertyDesc, EnumType).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::Enum, EnumType, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
				case ECameraContextDataType::Struct:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Struct &&
								PropertyDesc->ValueTypeObject == Definition.DataTypeObject))
					{
						const UScriptStruct* DataType = CastChecked<const UScriptStruct>(Definition.DataTypeObject);
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueStruct(*PropertyDesc, DataType).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::Struct, DataType, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
				case ECameraContextDataType::Object:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Object))
					{
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueObject(*PropertyDesc).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::Object, Definition.DataTypeObject, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
				case ECameraContextDataType::Class:
					if (ensure(PropertyDesc->ValueType == EPropertyBagPropertyType::Class))
					{
						const uint8* DefaultValue = reinterpret_cast<uint8*>(DefaultParameters.GetValueClass(*PropertyDesc).TryGetValue());
						OutParameterInfos.AddDataParameter(Definition.ParameterName, ECameraContextDataType::Class, Definition.DataTypeObject, DefaultValue, &MetaData.OverrideDataID);
					}
					break;
			}
		}
	}
}

bool FCameraRigAssetReference::SerializeFromMismatchedTag(struct FPropertyTag const& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_SoftObjectProperty)
	{
		FSoftObjectPtr CameraRigPath;
		Slot << CameraRigPath;
		CameraRig = Cast<UCameraRigAsset>(CameraRigPath.Get());
		return true;
	}
	return false;
}

void FCameraRigAssetReference::PostSerialize(const FArchive& Ar)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS

	// Make a property bag with the legacy overrides, and then set the values in it.
	bool bHasAnyLegacyOverride = false;
	TArray<FPropertyBagPropertyDesc> LegacyParameterProperties;
	TArray<FCameraRigAssetReferenceParameterMetaData> LegacyParameterMetaData;

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	for (F##ValueName##CameraRigParameterOverride& ParameterOverride : ParameterOverrides_DEPRECATED.ValueName##Overrides)\
	{\
		FName PropertyName(ParameterOverride.InterfaceParameterName);\
		EPropertyBagPropertyType PropertyType = EPropertyBagPropertyType::Struct;\
		const UObject* PropertyTypeObject = F##ValueName##CameraParameter::StaticStruct();\
		FPropertyBagPropertyDesc LegacyParameterProperty(PropertyName, PropertyType, PropertyTypeObject);\
		LegacyParameterProperty.ID = ParameterOverride.InterfaceParameterGuid;\
		LegacyParameterProperties.Add(LegacyParameterProperty);\
		FCameraRigAssetReferenceParameterMetaData MetaData;\
		MetaData.ParameterGuid = ParameterOverride.InterfaceParameterGuid;\
		MetaData.bIsOverridden = true;\
		LegacyParameterMetaData.Add(MetaData);\
		bHasAnyLegacyOverride = true;\
	}
	UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

	if (bHasAnyLegacyOverride)
	{
		Parameters = FInstancedPropertyBag();
		Parameters.AddProperties(LegacyParameterProperties);

		ParameterMetaData = LegacyParameterMetaData;

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		for (F##ValueName##CameraRigParameterOverride& ParameterOverride : ParameterOverrides_DEPRECATED.ValueName##Overrides)\
		{\
			FName PropertyName(ParameterOverride.InterfaceParameterName);\
			Parameters.SetValueStruct<F##ValueName##CameraParameter>(PropertyName, ParameterOverride.Value);\
		}
		UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

		ParameterOverrides_DEPRECATED = FCameraRigParameterOverrides();
	}

	PRAGMA_ENABLE_DEPRECATION_WARNINGS

	if (!ParameterOverrideGuids_DEPRECATED.IsEmpty())
	{
		for (const FGuid& Guid : ParameterOverrideGuids_DEPRECATED)
		{
			FCameraRigAssetReferenceParameterMetaData MetaData;
			MetaData.ParameterGuid = Guid;
			MetaData.bIsOverridden = true;
			ParameterMetaData.Add(MetaData);
		}

		ParameterOverrideGuids_DEPRECATED.Reset();
	}
}

