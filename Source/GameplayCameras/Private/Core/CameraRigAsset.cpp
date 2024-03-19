// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAsset.h"

#include "Core/CameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigAsset)

#if WITH_EDITOR

void UCameraRigAsset::GatherPackages(FCameraRigPackages& OutPackages) const
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

void UCameraRigAsset::GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePosX;
	NodePosY = GraphNodePosY;
}

void UCameraRigAsset::OnGraphNodeMoved(int32 NodePosX, int32 NodePosY)
{
	GraphNodePosX = NodePosX;
	GraphNodePosY = NodePosY;
}

const FString& UCameraRigAsset::GetGraphNodeCommentText() const
{
	return GraphNodeComment;
}

void UCameraRigAsset::OnUpdateGraphNodeCommentText(const FString& NewComment)
{
	GraphNodeComment = NewComment;
}

#endif  // WITH_EDITOR
