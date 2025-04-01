// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/Scene.h"

namespace UE::Cameras
{

/**
 * A class that can collect post-process settings, combining them with their associated
 * blend weights.
 */
struct GAMEPLAYCAMERAS_API FPostProcessSettingsCollection
{
	/** Gets the effective post-process settings. */
	FPostProcessSettings& Get() { return PostProcessSettings; }

	/** Gets the effective post-process settings. */
	const FPostProcessSettings& Get() const { return PostProcessSettings; }

	/** Resets this collection to the default post-process settings. */
	void Reset();

	/** 
	 * Overwrites the post-process settings in this collection with the values in the other.
	 */
	void OverrideAll(const FPostProcessSettingsCollection& OtherCollection);

	/** 
	 * Overwrites the post-process settings in this collection with any changed values in the other.
	 * Changed values are those whose bOverride_Xxx flag is true. Functionally equivalent to
	 * LerpAll with a blend factor of 100%.
	 */
	void OverrideChanged(const FPostProcessSettingsCollection& OtherCollection);
	void OverrideChanged(const FPostProcessSettings& OtherPostProcessSettings);

	/**
	 * Interpolates the post-process settings towards the values in the given other collection. All
	 * values are interpolated if either post-process settings have the bOverride_Xxx flag set.
	 * This means that some values will interpolate to and/or from default values.
	 */
	void LerpAll(const FPostProcessSettingsCollection& ToCollection, float BlendFactor);
	void LerpAll(const FPostProcessSettings& ToPostProcessSettings, float BlendFactor);

	/** Serializes this collection into the given archive. */
	void Serialize(FArchive& Ar);

private:

	void InternalLerp(const FPostProcessSettings& ToPostProcessSettings, float BlendFactor);

private:

	FPostProcessSettings PostProcessSettings;
};

}  // namespace UE::Cameras

