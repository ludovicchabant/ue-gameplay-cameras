// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/ObjectPtr.h"

class UCameraAsset;
class UCameraRigAsset;
class UCameraRigTransition;

namespace UE::Cameras
{

/**
 * Helper class for finding a camera rig transition that matches a given situation.
 */
class FCameraRigTransitionFinder
{
public:

	static const UCameraRigTransition* FindTransition(
			TArrayView<const TObjectPtr<UCameraRigTransition>> Transitions, 
			const UCameraRigAsset* FromCameraRig, const UCameraAsset* FromCameraAsset, bool bFromFrozen,
			const UCameraRigAsset* ToCameraRig, const UCameraAsset* ToCameraAsset);
};

}  // namespace UE::Cameras

