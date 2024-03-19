// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraNode.h"

#include "Core/CameraNodeEvaluator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraNode)

FCameraNodeChildrenView UCameraNode::GetChildren()
{
	return OnGetChildren();
}

void UCameraNode::BuildAllocationInfo(FCameraRigAllocationInfo& AllocationInfo) const
{
	OnBuildAllocationInfo(AllocationInfo);
}

FCameraNodeEvaluatorPtr UCameraNode::BuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	FCameraNodeEvaluator* NewEvaluator = OnBuildEvaluator(Builder);
	NewEvaluator->SetPrivateCameraNode(this);
	return NewEvaluator;
}

#if WITH_EDITOR

void UCameraNode::GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePosX;
	NodePosY = GraphNodePosY;
}

void UCameraNode::OnGraphNodeMoved(int32 NodePosX, int32 NodePosY)
{
	GraphNodePosX = NodePosX;
	GraphNodePosY = NodePosY;
}

const FString& UCameraNode::GetGraphNodeCommentText() const
{
	return GraphNodeComment;
}

void UCameraNode::OnUpdateGraphNodeCommentText(const FString& NewComment)
{
	GraphNodeComment = NewComment;
}

#endif  // WITH_EDITOR

