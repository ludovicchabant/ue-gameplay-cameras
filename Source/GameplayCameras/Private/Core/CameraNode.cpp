// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraNode.h"

#include "Core/CameraNodeEvaluator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraNode)

void FCameraNodeEvaluationResult::Reset()
{
	CameraPose.Reset();
	bIsCameraCut = false;
	bIsValid = false;
}

FCameraNodeEvaluatorAllocationInfo UCameraNode::GetEvaluatorInfo() const
{
	return OnGetEvaluatorInfo();
}

FCameraNodeEvaluatorPtr UCameraNode::BuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	FCameraNodeEvaluator* NewEvaluator = OnBuildEvaluator(Builder);
	NewEvaluator->SetPrivateCameraNode(this);
	return NewEvaluator;
}

