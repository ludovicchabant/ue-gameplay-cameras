// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/OffsetCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "GameplayCameras.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OffsetCameraNode)

class FOffsetCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(FOffsetCameraNodeEvaluator)

protected:

	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FOffsetCameraNodeEvaluator)

void FOffsetCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UOffsetCameraNode* OffsetNode = GetCameraNodeAs<UOffsetCameraNode>();
	FVector3d LocalOffset = OffsetNode->Offset;
	switch(OffsetNode->OffsetSpace)
	{
		case ECameraNodeSpace::CameraPose:
		default:
			{
				const FRotator3d Rotation = OutResult.CameraPose.GetRotation();
				LocalOffset = Rotation.RotateVector(LocalOffset);
			}
			break;
		case ECameraNodeSpace::Context:
			if (Params.EvaluationContext)
			{ 
				const FCameraNodeEvaluationResult& InitialResult = Params.EvaluationContext->GetInitialResult();
				ensureMsgf(InitialResult.bIsValid,
						TEXT("OffsetCameraNode: using invalid context result as offset space!"));
				const FRotator3d Rotation = InitialResult.CameraPose.GetRotation();
				LocalOffset = Rotation.RotateVector(LocalOffset);
			}
			else
			{
				UE_LOG(LogCameraSystem, Error, 
						TEXT("OffsetCameraNode: cannot offset in context space when there is "
							 "no current context set."));
				return;
			}
			break;
		case ECameraNodeSpace::World:
			// Nothing to do;
			break;
	}

	const FVector3d Location = OutResult.CameraPose.GetLocation();
	OutResult.CameraPose.SetLocation(Location + LocalOffset);
}

FCameraNodeEvaluatorPtr UOffsetCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	return Builder.BuildEvaluator<FOffsetCameraNodeEvaluator>();
}

