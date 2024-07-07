// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/Array.h"

class UCameraAsset;
class UObject;

namespace UE::Cameras
{

/**
 * A helper class for working with camera directors.
 */
class FCameraDirectorHelper
{
public:

	/** Gets the list of camera assets that reference the given object as their camera director. */
	static void GetReferencingCameraAssets(UObject* CameraDirectorObject, TArray<UCameraAsset*>& OutReferencers);
};

}  // namespace UE::Cameras

