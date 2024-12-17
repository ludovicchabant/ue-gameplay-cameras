// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAssetReference.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraNodeEvaluator.h"
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

bool FCameraRigAssetReference::IsParameterOverriden(const FGuid PropertyID) const
{
	return ParameterOverrideGuids.Contains(PropertyID);
}

void FCameraRigAssetReference::SetParameterOverriden(const FGuid PropertyID, bool bIsOverridden)
{
	if (bIsOverridden)
	{
		ParameterOverrideGuids.AddUnique(PropertyID);
	}
	else
	{
		ParameterOverrideGuids.Remove(PropertyID);
	}
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
		Parameters.MigrateToNewBagInstanceWithOverrides(CameraRig->GetDefaultParameters(), ParameterOverrideGuids);
		
		// Remove overrides for parameters that don't exist anymore.
		if (const UPropertyBag* ParametersType = Parameters.GetPropertyBagStruct())
		{
			for (TArray<FGuid>::TIterator It = ParameterOverrideGuids.CreateIterator(); It; ++It)
			{
				if (!ParametersType->FindPropertyDescByID(*It))
				{
					It.RemoveCurrentSwap();
				}
			}
		}
	}
	else
	{
		Parameters.Reset();
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
	TArray<FGuid> LegacyParameterOverrides;

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	for (F##ValueName##CameraRigParameterOverride& ParameterOverride : ParameterOverrides_DEPRECATED.ValueName##Overrides)\
	{\
		FName PropertyName(ParameterOverride.InterfaceParameterName);\
		EPropertyBagPropertyType PropertyType = EPropertyBagPropertyType::Struct;\
		const UObject* PropertyTypeObject = F##ValueName##CameraParameter::StaticStruct();\
		FPropertyBagPropertyDesc LegacyParameterProperty(PropertyName, PropertyType, PropertyTypeObject);\
		LegacyParameterProperty.ID = ParameterOverride.InterfaceParameterGuid;\
		LegacyParameterProperties.Add(LegacyParameterProperty);\
		LegacyParameterOverrides.Add(LegacyParameterProperty.ID);\
		bHasAnyLegacyOverride = true;\
	}
	UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

	if (bHasAnyLegacyOverride)
	{
		Parameters = FInstancedPropertyBag();
		Parameters.AddProperties(LegacyParameterProperties);

		ParameterOverrideGuids = LegacyParameterOverrides;

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
}

