// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraAssetReference.h"

#include "Core/CameraAsset.h"
#include "Core/CameraNodeEvaluator.h"
#include "Helpers/CameraAssetParameterOverrideEvaluator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraAssetReference)

FCameraAssetReference::FCameraAssetReference()
{
}

FCameraAssetReference::FCameraAssetReference(UCameraAsset* InCameraAsset)
	: CameraAsset(InCameraAsset)
{
}

void FCameraAssetReference::ApplyParameterOverrides(UE::Cameras::FCameraNodeEvaluationResult& OutResult, bool bDrivenOverridesOnly)
{
	using namespace UE::Cameras;
	FCameraAssetParameterOverrideEvaluator OverrideEvaluator(*this);
	OverrideEvaluator.ApplyParameterOverrides(OutResult.VariableTable, OutResult.ContextDataTable, bDrivenOverridesOnly);
}

bool FCameraAssetReference::IsParameterOverridden(const FGuid PropertyID) const
{
	return ParameterOverrideGuids.Contains(PropertyID);
}

void FCameraAssetReference::SetParameterOverridden(const FGuid PropertyID, bool bIsOverridden)
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

bool FCameraAssetReference::IsParameterAnimated(const FGuid PropertyID) const
{
	return ParameterAnimatedGuids.Contains(PropertyID);
}

void FCameraAssetReference::SetParameterAnimated(const FGuid PropertyID, bool bIsAnimated)
{
	if (bIsAnimated)
	{
		ParameterAnimatedGuids.AddUnique(PropertyID);
	}
	else
	{
		ParameterAnimatedGuids.Remove(PropertyID);
	}
}

bool FCameraAssetReference::NeedsRebuildParameters() const
{
	if ((!CameraAsset && Parameters.IsValid()) || (CameraAsset && !Parameters.IsValid()))
	{
		return true;
	}

	if (CameraAsset)
	{
		const UPropertyBag* AssetParametersType = CameraAsset->GetDefaultParameters().GetPropertyBagStruct();
		const UPropertyBag* ReferenceParametersType = Parameters.GetPropertyBagStruct();
		if (AssetParametersType != ReferenceParametersType)
		{
			return true;
		}
	}

	return false;
}

bool FCameraAssetReference::RebuildParametersIfNeeded()
{
	if (NeedsRebuildParameters())
	{
		RebuildParameters();
		return true;
	}
	return false;
}

void FCameraAssetReference::RebuildParameters()
{
	if (CameraAsset)
	{
		Parameters.MigrateToNewBagInstanceWithOverrides(CameraAsset->GetDefaultParameters(), ParameterOverrideGuids);
		
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

bool FCameraAssetReference::SerializeFromMismatchedTag(FPropertyTag const& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_SoftObjectProperty)
	{
		FSoftObjectPtr CameraAssetPath;
		Slot << CameraAssetPath;
		CameraAsset = Cast<UCameraAsset>(CameraAssetPath.Get());
		return true;
	}
	return false;
}

