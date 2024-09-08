// Copyright Epic Games, Inc. All Rights Reserved.

#include "Transitions/DefaultTransitionConditions.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DefaultTransitionConditions)

bool UIsCameraRigTransitionCondition::OnTransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const
{
	const bool bPreviousMatches = (!PreviousCameraRig || PreviousCameraRig == Params.FromCameraRig);
	const bool bNextMatches = (!NextCameraRig || NextCameraRig == Params.ToCameraRig);
	return bPreviousMatches && bNextMatches;
}

