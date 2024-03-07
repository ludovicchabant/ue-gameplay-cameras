// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayCameras.h"
#include "Math/Color.h"
#include "Misc/OptionalFwd.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

/**
 * Standard debug drawing colors for the camera system.
 */
class FCameraDebugColors
{
public:

	static const FCameraDebugColors& Get();

	/** Debug category titles and section titles. */
	FColor Title;
	/** Normal text. */
	FColor Default;
	/** Unimportant text. */
	FColor Passive;
	FColor VeryPassive;
	/** Important or notable text. */
	FColor Hightlighted;
	/** Text with no specific importance, but that needs to be separate from the rest. */
	FColor Notice;
	FColor Notice2;
	/** Positive text. */
	FColor Good;
	/** Warning text. */
	FColor Warning;
	/** Error text. */
	FColor Error;
	/** Background. Should only be used for the background tile. */
	FColor Background;

public:

	static TOptional<FColor> GetFColorByName(const FString& InColorName);

private:

	FCameraDebugColors();

	static void UpdateColorMap(const FCameraDebugColors& Instance);

	static TMap<FString, FColor> ColorMap;
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

