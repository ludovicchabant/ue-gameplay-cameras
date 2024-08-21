// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/Array.h"

class UCameraAsset;
class UObject;

namespace UE::Cameras
{

/**
 * A helper class for gathering camera assets referencing a given object, such as a
 * camera director dependency.
 */
class FCameraAssetReferenceGatherer
{
public:

	/** Gets the list of camera assets that reference the given object. */
	static void GetReferencingCameraAssets(UObject* ReferencedObject, TArray<UCameraAsset*>& OutReferencers);
};

}  // namespace UE::Cameras

