// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/CameraRigTransitionFinder.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigTransition.h"

namespace UE::Cameras
{

const UCameraRigTransition* FCameraRigTransitionFinder::FindTransition(
		TArrayView<const TObjectPtr<UCameraRigTransition>> Transitions, 
		const UCameraRigAsset* FromCameraRig, const UCameraAsset* FromCameraAsset, bool bFromFrozen,
		const UCameraRigAsset* ToCameraRig, const UCameraAsset* ToCameraAsset)
{
	FCameraRigTransitionConditionMatchParams MatchParams;
	MatchParams.FromCameraRig = FromCameraRig;
	MatchParams.FromCameraAsset = FromCameraAsset;
	MatchParams.ToCameraRig = ToCameraRig;
	MatchParams.ToCameraAsset = ToCameraAsset;

	// The transition should be used if all its conditions pass.
	for (TObjectPtr<const UCameraRigTransition> Transition : Transitions)
	{
		const bool bConditionsPass = Transition->AllConditionsMatch(MatchParams);
		if (bConditionsPass)
		{
			return Transition;
		}
	}

	return nullptr;
}

}  // namespace UE::Cameras


