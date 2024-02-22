// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Blends/SimpleBlendCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SimpleBlendCameraNode)

namespace UE::Cameras
{

UE_DEFINE_BLEND_CAMERA_NODE_EVALUATOR(FSimpleBlendCameraNodeEvaluator)

void FSimpleBlendCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	FSimpleBlendCameraNodeEvaluationResult FactorResult;
	OnComputeBlendFactor(Params, FactorResult);
	BlendFactor = FactorResult.BlendFactor;
}

void FSimpleBlendCameraNodeEvaluator::OnBlendResults(const FCameraNodeBlendParams& Params, FCameraNodeBlendResult& OutResult)
{
	const FCameraNodeEvaluationResult& ChildResult(Params.ChildResult);
	FCameraNodeEvaluationResult& BlendedResult(OutResult.BlendedResult);

	BlendedResult.CameraPose.LerpChanged(
			ChildResult.CameraPose, 
			BlendFactor,
			ChildResult.CameraPose.GetChangedFlags(), // or is it BlendedResult flags?
			false,
			BlendedResult.CameraPose.GetChangedFlags());

	BlendedResult.VariableTable.LerpChanged(ChildResult.VariableTable, BlendFactor);

	// If we have even a fraction of a camera cut, we need to make the
	// whole result into a camera cut.
	if (BlendFactor > 0.f && ChildResult.bIsCameraCut)
	{
		BlendedResult.bIsCameraCut = true;
	}

	OutResult.bIsBlendFull = BlendFactor >= 1.f;
	OutResult.bIsBlendFinished = bIsBlendFinished;
}

UE_DEFINE_BLEND_CAMERA_NODE_EVALUATOR(FSimpleFixedTimeBlendCameraNodeEvaluator)

void FSimpleFixedTimeBlendCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const USimpleFixedTimeBlendCameraNode* BlendNode = GetCameraNodeAs<USimpleFixedTimeBlendCameraNode>();
	CurrentTime += Params.DeltaTime;
	if (CurrentTime >= BlendNode->BlendTime)
	{
		CurrentTime = BlendNode->BlendTime;
		SetBlendFinished();
	}

	FSimpleBlendCameraNodeEvaluator::OnRun(Params, OutResult);
}

float FSimpleFixedTimeBlendCameraNodeEvaluator::GetTimeFactor() const
{
	const USimpleFixedTimeBlendCameraNode* BlendNode = GetCameraNodeAs<USimpleFixedTimeBlendCameraNode>();
	return CurrentTime / BlendNode->BlendTime;
}

}  // namespace UE::Cameras

