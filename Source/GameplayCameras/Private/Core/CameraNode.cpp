// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraNode.h"

#include "Core/CameraNodeEvaluator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraNode)

namespace UE::Cameras
{

void FCameraNodeEvaluationResult::Reset()
{
	CameraPose.Reset();
	bIsCameraCut = false;
	bIsValid = false;
}

}  // namespace UE::Cameras

FCameraNodeChildrenView UCameraNode::GetChildren()
{
	return OnGetChildren();
}

FCameraNodeAllocationInfo UCameraNode::GetAllocationInfo() const
{
	return OnGetAllocationInfo();
}

FCameraNodeEvaluatorPtr UCameraNode::BuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	FCameraNodeEvaluator* NewEvaluator = OnBuildEvaluator(Builder);
	NewEvaluator->SetPrivateCameraNode(this);
	return NewEvaluator;
}

