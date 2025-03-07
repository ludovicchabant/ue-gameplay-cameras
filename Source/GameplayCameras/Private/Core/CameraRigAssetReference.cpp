// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAssetReference.h"

#include "Build/CameraObjectInterfaceParameterBuilder.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraNodeEvaluator.h"
#include "Helpers/CameraObjectReferenceParameterOverrideEvaluator.h"

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

const UBaseCameraObject* FCameraRigAssetReference::GetCameraObject() const
{
	return CameraRig;
}

void FCameraRigAssetReference::ApplyParameterOverrides(UE::Cameras::FCameraNodeEvaluationResult& OutResult, bool bDrivenOverridesOnly)
{
	using namespace UE::Cameras;
	FCameraObjectReferenceParameterOverrideEvaluator OverrideEvaluator(*this);
	OverrideEvaluator.ApplyParameterOverrides(OutResult.VariableTable, OutResult.ContextDataTable, bDrivenOverridesOnly);
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
	TArray<FCameraObjectInterfaceParameterMetaData> LegacyParameterMetaData;

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	for (F##ValueName##CameraRigParameterOverride& ParameterOverride : ParameterOverrides_DEPRECATED.ValueName##Overrides)\
	{\
		FName PropertyName(ParameterOverride.InterfaceParameterName);\
		EPropertyBagPropertyType PropertyType = EPropertyBagPropertyType::Struct;\
		const UObject* PropertyTypeObject = F##ValueName##CameraParameter::StaticStruct();\
		FPropertyBagPropertyDesc LegacyParameterProperty(PropertyName, PropertyType, PropertyTypeObject);\
		LegacyParameterProperty.ID = ParameterOverride.InterfaceParameterGuid;\
		LegacyParameterProperties.Add(LegacyParameterProperty);\
		FCameraObjectInterfaceParameterMetaData MetaData;\
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
			FCameraObjectInterfaceParameterMetaData MetaData;
			MetaData.ParameterGuid = Guid;
			MetaData.bIsOverridden = true;
			ParameterMetaData.Add(MetaData);
		}

		ParameterOverrideGuids_DEPRECATED.Reset();
	}
}

