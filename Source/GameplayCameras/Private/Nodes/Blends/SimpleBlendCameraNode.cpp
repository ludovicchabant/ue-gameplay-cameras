// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Blends/SimpleBlendCameraNode.h"

#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SimpleBlendCameraNode)

namespace UE::Cameras
{

UE_DEFINE_BLEND_CAMERA_NODE_EVALUATOR(FSimpleBlendCameraNodeEvaluator)

UE_DECLARE_CAMERA_DEBUG_BLOCK_START(GAMEPLAYCAMERAS_API, FSimpleBlendCameraDebugBlock)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(float, BlendFactor)
UE_DECLARE_CAMERA_DEBUG_BLOCK_END()

UE_DEFINE_CAMERA_DEBUG_BLOCK_WITH_FIELDS(FSimpleBlendCameraDebugBlock)

void FSimpleBlendCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	FSimpleBlendCameraNodeEvaluationResult FactorResult;
	OnComputeBlendFactor(Params, FactorResult);
	BlendFactor = FactorResult.BlendFactor;
}

void FSimpleBlendCameraNodeEvaluator::OnBlendParameters(const FCameraNodePreBlendParams& Params, FCameraNodePreBlendResult& OutResult)
{
	const FCameraVariableTable& ChildVariableTable(Params.ChildVariableTable);
	OutResult.VariableTable.Lerp(ChildVariableTable, ECameraVariableTableFilter::Input | Params.ExtraVariableTableFilter, BlendFactor);

	OutResult.bIsBlendFull = BlendFactor >= 1.f;
	OutResult.bIsBlendFinished = bIsBlendFinished;
}

void FSimpleBlendCameraNodeEvaluator::OnBlendResults(const FCameraNodeBlendParams& Params, FCameraNodeBlendResult& OutResult)
{
	const FCameraNodeEvaluationResult& ChildResult(Params.ChildResult);
	FCameraNodeEvaluationResult& BlendedResult(OutResult.BlendedResult);

	BlendedResult.LerpAll(ChildResult, BlendFactor);

	OutResult.bIsBlendFull = BlendFactor >= 1.f;
	OutResult.bIsBlendFinished = bIsBlendFinished;
}

void FSimpleBlendCameraNodeEvaluator::OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar)
{
	Ar << BlendFactor;
	Ar << bIsBlendFinished;
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FSimpleBlendCameraNodeEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	FSimpleBlendCameraDebugBlock& DebugBlock = Builder.AttachDebugBlock<FSimpleBlendCameraDebugBlock>();
	DebugBlock.BlendFactor = BlendFactor;
}

void FSimpleBlendCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	Renderer.AddText(TEXT("blend %.2f%%"), BlendFactor * 100.f);
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

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
	if (BlendNode->BlendTime > 0.f)
	{
		return CurrentTime / BlendNode->BlendTime;
	}
	return 1.f;
}

}  // namespace UE::Cameras

