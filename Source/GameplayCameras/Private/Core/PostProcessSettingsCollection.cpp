// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/PostProcessSettingsCollection.h"

#include "Engine/PostProcessUtils.h"

namespace UE::Cameras
{

void FPostProcessSettingsCollection::Reset()
{
	PostProcessSettings = FPostProcessSettings::GetDefault();
}

void FPostProcessSettingsCollection::OverrideAll(const FPostProcessSettingsCollection& OtherCollection)
{
	PostProcessSettings = OtherCollection.PostProcessSettings;
}

void FPostProcessSettingsCollection::OverrideChanged(const FPostProcessSettingsCollection& OtherCollection)
{
	OverrideChanged(OtherCollection.PostProcessSettings);
}

void FPostProcessSettingsCollection::OverrideChanged(const FPostProcessSettings& OtherPostProcessSettings)
{
	FPostProcessSettings& ThisPP = PostProcessSettings;
	const FPostProcessSettings& OtherPP = OtherPostProcessSettings;

	FPostProcessUtils::OverridePostProcessSettings(ThisPP, OtherPP);
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

	FPostProcessUtils::BlendPostProcessSettings(ThisPP, ToPP, BlendFactor);
}

void FPostProcessSettingsCollection::Serialize(FArchive& Ar)
{
	UScriptStruct* PostProcessSettingsStruct = FPostProcessSettings::StaticStruct();
	PostProcessSettingsStruct->SerializeItem(Ar, &PostProcessSettings, &FPostProcessSettings::GetDefault());
}

}  // namespace UE::Cameras

