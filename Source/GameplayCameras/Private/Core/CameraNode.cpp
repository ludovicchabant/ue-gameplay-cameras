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

