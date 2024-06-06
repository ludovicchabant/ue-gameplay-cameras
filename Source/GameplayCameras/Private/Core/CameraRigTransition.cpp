// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigTransition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigTransition)

void UCameraRigTransitionCondition::PostLoad()
{
#if WITH_EDITORONLY_DATA

	if (GraphNodePosX_DEPRECATED != 0 || GraphNodePosY_DEPRECATED != 0)
	{
		GraphNodePos = FIntVector2(GraphNodePosX_DEPRECATED, GraphNodePosY_DEPRECATED);

		GraphNodePosX_DEPRECATED = 0;
		GraphNodePosY_DEPRECATED = 0;
	}

#endif

	Super::PostLoad();
}

bool UCameraRigTransitionCondition::TransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const
{
	return OnTransitionMatches(Params);
}

#if WITH_EDITOR

void UCameraRigTransitionCondition::GetGraphNodePosition(FName InGraphName, int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePos.X;
	NodePosY = GraphNodePos.Y;
}

void UCameraRigTransitionCondition::OnGraphNodeMoved(FName InGraphName, int32 NodePosX, int32 NodePosY, bool bMarkDirty)
{
	Modify(bMarkDirty);

	GraphNodePos.X = NodePosX;
	GraphNodePos.Y = NodePosY;
}

const FString& UCameraRigTransitionCondition::GetGraphNodeCommentText(FName InGraphName) const
{
	return GraphNodeComment;
}

void UCameraRigTransitionCondition::OnUpdateGraphNodeCommentText(FName InGraphName, const FString& NewComment)
{
	GraphNodeComment = NewComment;
}

#endif

void UCameraRigTransition::PostLoad()
{
#if WITH_EDITORONLY_DATA

	if (GraphNodePosX_DEPRECATED != 0 || GraphNodePosY_DEPRECATED != 0)
	{
		GraphNodePos = FIntVector2(GraphNodePosX_DEPRECATED, GraphNodePosY_DEPRECATED);

		GraphNodePosX_DEPRECATED = 0;
		GraphNodePosY_DEPRECATED = 0;
	}

#endif

	Super::PostLoad();
}

#if WITH_EDITOR

void UCameraRigTransition::GetGraphNodePosition(FName InGraphName, int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePos.X;
	NodePosY = GraphNodePos.Y;
}

void UCameraRigTransition::OnGraphNodeMoved(FName InGraphName, int32 NodePosX, int32 NodePosY, bool bMarkDirty)
{
	Modify(bMarkDirty);

	GraphNodePos.X = NodePosX;
	GraphNodePos.Y = NodePosY;
}

const FString& UCameraRigTransition::GetGraphNodeCommentText(FName InGraphName) const
{
	return GraphNodeComment;
}

void UCameraRigTransition::OnUpdateGraphNodeCommentText(FName InGraphName, const FString& NewComment)
{
	GraphNodeComment = NewComment;
}

#endif  // WITH_EDITOR

