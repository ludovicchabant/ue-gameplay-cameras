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

	/** Returns whether this collection has any set post-process settings. */
	bool HasAnyPostProcessSettings() const { return bHasAnySetting; }

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

	/**
	 * Interpolates the post-process settings towards the values in the given other collection. Only
	 * values from the other collection that have their bOverride_Xxx set to true will be interpolated.
	 * If the current value isn't overriden, it will interpolate from the default value. All affected
	 * values will get their bOverride_Xxx flag set.
	 */
	void LerpChanged(const FPostProcessSettingsCollection& ToCollection, float BlendFactor);
	void LerpChanged(const FPostProcessSettings& ToPostProcessSettings, float BlendFactor);

	/** Serializes this collection into the given archive. */
	void Serialize(FArchive& Ar);

private:

	void InternalLerpChanged(const FPostProcessSettings& ToPostProcessSettings, float BlendFactor, bool bChangedOnly);

private:

	FPostProcessSettings PostProcessSettings;
	bool bHasAnySetting = false;
};

}  // namespace UE::Cameras

