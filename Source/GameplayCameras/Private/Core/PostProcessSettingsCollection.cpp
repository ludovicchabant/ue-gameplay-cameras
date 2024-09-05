// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/PostProcessSettingsCollection.h"

namespace UE::Cameras
{

FArchive& operator<< (FArchive& Ar, FPostProcessSettingsCollectionEntry& Entry)
{
	static FPostProcessSettings DefaultPostProcessSettings;

	UScriptStruct* PostProcessSettingsStruct = FPostProcessSettings::StaticStruct();
	PostProcessSettingsStruct->SerializeItem(Ar, &Entry.PostProcessSettings, &DefaultPostProcessSettings);

	Ar << Entry.PostProcessBlendWeight;

	return Ar;
}

void FPostProcessSettingsCollection::Add(const FPostProcessSettings& InPostProcess, float InBlendWeight)
{
	Entries.Add(FEntry{ InPostProcess, InBlendWeight });
}

void FPostProcessSettingsCollection::Reset()
{
	Entries.Reset();
}

void FPostProcessSettingsCollection::OverrideAll(const FPostProcessSettingsCollection& OtherCollection)
{
	Entries = OtherCollection.Entries;
}

void FPostProcessSettingsCollection::LerpAll(const FPostProcessSettingsCollection& ToCollection, float BlendFactor)
{
	// Blend down our entries. Remove any entries with zero weight.
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		FEntry& ThisEntry = *It;
		ThisEntry.PostProcessBlendWeight *= (1.f - BlendFactor);
		
		if (ThisEntry.PostProcessBlendWeight < UE_SMALL_NUMBER)
		{
			It.RemoveCurrent();
		}
	}

	// Blend up the other collection's entries.
	for (const FEntry& OtherEntry : ToCollection.Entries)
	{
		Entries.Add(FEntry{ OtherEntry.PostProcessSettings, OtherEntry.PostProcessBlendWeight * BlendFactor });
	}
}

void FPostProcessSettingsCollection::Serialize(FArchive& Ar)
{
	Ar << Entries;
}

}  // namespace UE::Cameras

