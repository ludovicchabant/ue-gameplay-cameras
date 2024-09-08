// Copyright Epic Games, Inc. All Rights Reserved.

#include "Transitions/GameplayTagTransitionConditions.h"

#include "Core/CameraRigAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagTransitionConditions)

bool UGameplayTagTransitionCondition::OnTransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const
{
	bool bPreviousMatches = true;

	if (!PreviousGameplayTagQuery.IsEmpty())
	{
		if (Params.FromCameraRig)
		{
			FGameplayTagContainer TagContainer;
			Params.FromCameraRig->GetOwnedGameplayTags(TagContainer);
			if (TagContainer.MatchesQuery(PreviousGameplayTagQuery))
			{
				bPreviousMatches = true;
			}
		}
		else
		{
			bPreviousMatches = false;
		}
	}

	bool bNextMatches = true;

	if (!NextGameplayTagQuery.IsEmpty())
	{
		if (Params.ToCameraRig)
		{
			FGameplayTagContainer TagContainer;
			Params.ToCameraRig->GetOwnedGameplayTags(TagContainer);
			if (TagContainer.MatchesQuery(NextGameplayTagQuery))
			{
				bNextMatches = true;
			}
		}
		else
		{
			bNextMatches = false;
		}
	}

	return bPreviousMatches && bNextMatches;
}

