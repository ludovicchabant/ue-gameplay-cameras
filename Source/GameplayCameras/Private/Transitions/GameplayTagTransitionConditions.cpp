// Copyright Epic Games, Inc. All Rights Reserved.

#include "Transitions/GameplayTagTransitionConditions.h"

#include "Core/CameraRigAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagTransitionConditions)

bool UPreviousGameplayTagTransitionCondition::OnTransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const
{
	if (Params.FromCameraRig)
	{
		FGameplayTagContainer TagContainer;
		Params.FromCameraRig->GetOwnedGameplayTags(TagContainer);
		if (TagContainer.MatchesQuery(GameplayTagQuery))
		{
			return true;
		}
	}
	return false;
}

bool UNextGameplayTagTransitionCondition::OnTransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const
{
	if (Params.ToCameraRig)
	{
		FGameplayTagContainer TagContainer;
		Params.ToCameraRig->GetOwnedGameplayTags(TagContainer);
		if (TagContainer.MatchesQuery(GameplayTagQuery))
		{
			return true;
		}
	}
	return false;
}

