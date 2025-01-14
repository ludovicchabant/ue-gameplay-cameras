// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/PostProcessSettingsCollection.h"

#include "Engine/PostProcessUtils.h"

namespace UE::Cameras
{

void FPostProcessSettingsCollection::Reset()
{
	static const FPostProcessSettings DefaultPostProcessSettings;

	PostProcessSettings = DefaultPostProcessSettings;
	bHasAnySetting = false;
}

void FPostProcessSettingsCollection::OverrideAll(const FPostProcessSettingsCollection& OtherCollection)
{
	PostProcessSettings = OtherCollection.PostProcessSettings;
	bHasAnySetting = OtherCollection.bHasAnySetting;
}

void FPostProcessSettingsCollection::OverrideChanged(const FPostProcessSettingsCollection& OtherCollection)
{
	OverrideChanged(OtherCollection.PostProcessSettings);
}

void FPostProcessSettingsCollection::OverrideChanged(const FPostProcessSettings& OtherPostProcessSettings)
{
	FPostProcessSettings& ThisPP = PostProcessSettings;
	const FPostProcessSettings& OtherPP = OtherPostProcessSettings;

	const bool bAnyOverwritten = FPostProcessUtils::OverridePostProcessSettings(ThisPP, OtherPP);
	bHasAnySetting |= bAnyOverwritten;
}

void FPostProcessSettingsCollection::LerpAll(const FPostProcessSettingsCollection& ToCollection, float BlendFactor)
{
	LerpAll(ToCollection.PostProcessSettings, BlendFactor);
}

void FPostProcessSettingsCollection::LerpAll(const FPostProcessSettings& ToPostProcessSettings, float BlendFactor)
{
	InternalLerp(ToPostProcessSettings, BlendFactor);
}

void FPostProcessSettingsCollection::InternalLerp(const FPostProcessSettings& ToPostProcessSettings, float BlendFactor)
{
	FPostProcessSettings& ThisPP = PostProcessSettings;
	const FPostProcessSettings& ToPP = ToPostProcessSettings;

	const bool bAnyOverwritten = FPostProcessUtils::BlendPostProcessSettings(ThisPP, ToPP, BlendFactor);
	bHasAnySetting |= bAnyOverwritten;
}

void FPostProcessSettingsCollection::Serialize(FArchive& Ar)
{
	static const FPostProcessSettings DefaultPostProcessSettings;

	UScriptStruct* PostProcessSettingsStruct = FPostProcessSettings::StaticStruct();
	PostProcessSettingsStruct->SerializeItem(Ar, &PostProcessSettings, &DefaultPostProcessSettings);

	Ar << bHasAnySetting;
}

}  // namespace UE::Cameras

