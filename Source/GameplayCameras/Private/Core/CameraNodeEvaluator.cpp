// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraNodeEvaluator.h"

#include "Core/CameraNode.h"
#include "UObject/UObjectGlobals.h"

namespace UE::Cameras
{

UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(FCameraNodeEvaluator)

void FCameraNodeEvaluationResult::Reset()
{
	CameraPose.Reset();
	bIsCameraCut = false;
	bIsValid = false;
}

FCameraNodeEvaluator* FCameraNodeEvaluatorBuildParams::BuildEvaluator(const UCameraNode* InNode) const
{
	FCameraNodeEvaluator* NewEvaluator = InNode->BuildEvaluator(Builder);
	NewEvaluator->Build(*this);
	return NewEvaluator;
}

FCameraNodeEvaluator::FCameraNodeEvaluator()
{
}

void FCameraNodeEvaluator::SetPrivateCameraNode(TObjectPtr<const UCameraNode> InCameraNode)
{
	PrivateCameraNode = InCameraNode;
}

FCameraNodeEvaluatorChildrenView FCameraNodeEvaluator::GetChildren()
{
	return OnGetChildren();
}

void FCameraNodeEvaluator::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (PrivateCameraNode)
	{
		Collector.AddReferencedObject(PrivateCameraNode);
	}

	OnAddReferencedObjects(Collector);

	for (FCameraNodeEvaluator* Child : GetChildren())
	{
		if (Child)
		{
			Child->AddReferencedObjects(Collector);
		}
	}
}

void FCameraNodeEvaluator::Build(const FCameraNodeEvaluatorBuildParams& Params)
{
	OnBuild(Params);
}

void FCameraNodeEvaluator::Initialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	OnInitialize(Params);

	for (FCameraNodeEvaluator* Child : GetChildren())
	{
		if (Child)
		{
			Child->Initialize(Params);
		}
	}
}

void FCameraNodeEvaluator::Run(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	if (PrivateCameraNode->bIsEnabled)
	{
		OnRun(Params, OutResult);
	}
}

}  // namespace UE::Cameras

