// Copyright Epic Games, Inc. All Rights Reserved.

#include "StructUtils/OverridablePropertyBag.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OverridablePropertyBag)

bool FInstancedOverridablePropertyBag::IsPropertyOverriden(const FGuid& InPropertyID) const
{
	return OverridenPropertyIDs.Contains(InPropertyID);
}

void FInstancedOverridablePropertyBag::SetPropertyOverriden(const FGuid& InPropertyID, bool bIsOverriden)
{
	if (bIsOverriden)
	{
		OverridenPropertyIDs.AddUnique(InPropertyID);
	}
	else
	{
		OverridenPropertyIDs.Remove(InPropertyID);
	}
}

void FInstancedOverridablePropertyBag::MigrateToNewBagInstanceWithOverrides(const FInstancedPropertyBag& NewBagInstance)
{
	FInstancedPropertyBag::MigrateToNewBagInstanceWithOverrides(NewBagInstance, OverridenPropertyIDs);

	// Remove overrides for propeties that don't exist anymore.
	if (const UPropertyBag* ParametersType = GetPropertyBagStruct())
	{
		for (TArray<FGuid>::TIterator It = OverridenPropertyIDs.CreateIterator(); It; ++It)
		{
			if (!ParametersType->FindPropertyDescByID(*It))
			{
				It.RemoveCurrentSwap();
			}
		}
	}
}

bool FInstancedOverridablePropertyBag::SerializeFromMismatchedTag(FPropertyTag const& Tag, FStructuredArchive::FSlot Slot)
{
	static const FName NAME_InstancedPropertyBag = FInstancedPropertyBag::StaticStruct()->GetFName();
	if (Tag.GetType().IsStruct(NAME_InstancedPropertyBag))
	{
		// Read the structured data as an FInstancedPropertyBag. Our list of overriden property IDs
		// will stay empty.
		Serialize(Slot.GetUnderlyingArchive());
		return true;
	}
	return false;
}

