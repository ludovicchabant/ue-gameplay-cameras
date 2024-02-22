// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraMode.h"

#include "Core/CameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraMode)

#if WITH_EDITOR

void UCameraMode::GatherPackages(FCameraModePackages& OutPackages) const
{
	TArray<UCameraNode*> NodeStack;
	if (RootNode)
	{
		NodeStack.Add(RootNode);
	}
	while (!NodeStack.IsEmpty())
	{
		UCameraNode* CurrentNode = NodeStack.Pop();
		const UPackage* CurrentPackage = CurrentNode->GetOutermost();
		OutPackages.AddUnique(CurrentPackage);

		FCameraNodeChildrenView CurrentChildren = CurrentNode->GetChildren();
		for (UCameraNode* CurrentChild : ReverseIterate(CurrentChildren))
		{
			NodeStack.Add(CurrentChild);
		}
	}
}

#endif  // WITH_EDITOR
