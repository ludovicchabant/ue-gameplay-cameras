// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/OffsetCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraParameterReader.h"
#include "GameplayCameras.h"
#include "Math/Axis.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OffsetCameraNode)

namespace UE::Cameras
{

class FOffsetCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FOffsetCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	TCameraParameterReader<FVector3d> TranslationReader;
	TCameraParameterReader<FRotator3d> RotationReader;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FOffsetCameraNodeEvaluator)

void FOffsetCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UOffsetCameraNode* OffsetNode = GetCameraNodeAs<UOffsetCameraNode>();
	TranslationReader.Initialize(OffsetNode->TranslationOffset);
	RotationReader.Initialize(OffsetNode->RotationOffset);
}

void FOffsetCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const FVector3d TranslationOffset = TranslationReader.Get(OutResult.VariableTable);
	const FRotator3d RotationOffset = RotationReader.Get(OutResult.VariableTable);

	const UOffsetCameraNode* OffsetNode = GetCameraNodeAs<UOffsetCameraNode>();
	switch (OffsetNode->OffsetSpace)
	{
		case ECameraNodeSpace::CameraPose:
		default:
			{
				FTransform3d Transform = OutResult.CameraPose.GetTransform();
				Transform = FTransform3d(RotationOffset, TranslationOffset) * Transform;
				OutResult.CameraPose.SetTransform(Transform);
			}
			break;
		case ECameraNodeSpace::OwningContext:
			if (Params.EvaluationContext)
			{ 
				// The offsets are meant to be treated as context-local. Let's get the context transform
				// and apply the offsets using that transform's axes.
				const FCameraNodeEvaluationResult& InitialResult = Params.EvaluationContext->GetInitialResult();
				const FTransform3d ContextTransform = InitialResult.CameraPose.GetTransform();

				const FVector3d WorldTranslationOffset = ContextTransform.TransformVector(TranslationOffset);

				const FVector3d ContextForward = ContextTransform.GetUnitAxis(EAxis::X);
				const FVector3d ContextRight = ContextTransform.GetUnitAxis(EAxis::Y);
				const FVector3d ContextUp = ContextTransform.GetUnitAxis(EAxis::Z);
				const FQuat WorldRotationOffset = 
					FQuat(ContextUp, FMath::DegreesToRadians(RotationOffset.Yaw)) * 
					FQuat(ContextRight, -FMath::DegreesToRadians(RotationOffset.Pitch)) *
					FQuat(ContextForward, -FMath::DegreesToRadians(RotationOffset.Roll));

				FTransform3d Transform = OutResult.CameraPose.GetTransform();
				Transform.SetTranslation(Transform.GetTranslation() + WorldTranslationOffset);
				Transform.SetRotation(WorldRotationOffset * Transform.GetRotation());
				OutResult.CameraPose.SetTransform(Transform);
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
			{
				FTransform3d Transform = OutResult.CameraPose.GetTransform();
				Transform.SetTranslation(Transform.GetTranslation() + TranslationOffset);
				Transform.SetRotation(RotationOffset.Quaternion() * Transform.GetRotation());
				OutResult.CameraPose.SetTransform(Transform);
			}
			break;
	}
}

}  // namespace UE::Cameras

FCameraNodeEvaluatorPtr UOffsetCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FOffsetCameraNodeEvaluator>();
}

TTuple<int32, int32> UOffsetCameraNode::GetEvaluatorAllocationInfo()
{
	using namespace UE::Cameras;
	return { sizeof(FOffsetCameraNodeEvaluator), alignof(FOffsetCameraNodeEvaluator) };
}

