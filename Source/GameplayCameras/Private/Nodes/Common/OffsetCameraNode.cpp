// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/OffsetCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraParameterReader.h"
#include "GameplayCameras.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OffsetCameraNode)

class FOffsetCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(FOffsetCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	FCameraParameterReader<FVector3d> OffsetReader;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FOffsetCameraNodeEvaluator)

void FOffsetCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	const UOffsetCameraNode* OffsetNode = GetCameraNodeAs<UOffsetCameraNode>();
	OffsetReader.Initialize(OffsetNode->Offset);
}

void FOffsetCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	FVector3d LocalOffset = OffsetReader.Get(OutResult.VariableTable);

	const UOffsetCameraNode* OffsetNode = GetCameraNodeAs<UOffsetCameraNode>();
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

