// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigTransition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigTransition)

bool UCameraRigTransitionCondition::TransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const
{
	return OnTransitionMatches(Params);
}

#if WITH_EDITOR

void UCameraRigTransitionCondition::GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePosX;
	NodePosY = GraphNodePosY;
}

void UCameraRigTransitionCondition::OnGraphNodeMoved(int32 NodePosX, int32 NodePosY)
{
	GraphNodePosX = NodePosX;
	GraphNodePosY = NodePosY;
}

const FString& UCameraRigTransitionCondition::GetGraphNodeCommentText() const
{
	return GraphNodeComment;
}

void UCameraRigTransitionCondition::OnUpdateGraphNodeCommentText(const FString& NewComment)
{
	GraphNodeComment = NewComment;
}

void UCameraRigTransition::GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePosX;
	NodePosY = GraphNodePosY;
}

void UCameraRigTransition::OnGraphNodeMoved(int32 NodePosX, int32 NodePosY)
{
	GraphNodePosX = NodePosX;
	GraphNodePosY = NodePosY;
}

const FString& UCameraRigTransition::GetGraphNodeCommentText() const
{
	return GraphNodeComment;
}

void UCameraRigTransition::OnUpdateGraphNodeCommentText(const FString& NewComment)
{
	GraphNodeComment = NewComment;
}

#endif  // WITH_EDITOR
