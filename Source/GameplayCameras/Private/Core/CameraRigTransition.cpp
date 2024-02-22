// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigTransition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigTransition)

bool UCameraRigTransitionCondition::TransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const
{
	return OnTransitionMatches(Params);
}

