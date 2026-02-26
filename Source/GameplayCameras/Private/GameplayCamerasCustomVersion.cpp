// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCamerasCustomVersion.h"

#include "Serialization/CustomVersion.h"

FCustomVersionRegistration GGameplayCamerasCustomVersion(
		FGameplayCamerasCustomVersion::GUID,
		FGameplayCamerasCustomVersion::LatestVersion,
		TEXT("GameplayCamerasCustomVersion"));

