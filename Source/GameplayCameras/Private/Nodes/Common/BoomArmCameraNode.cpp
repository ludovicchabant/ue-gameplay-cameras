// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/BoomArmCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraParameterReader.h"
#include "Nodes/Input/CameraRigInput2DSlot.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameplayCameras.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BoomArmCameraNode)

namespace UE::Cameras
{

class FBoomArmCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FBoomArmCameraNodeEvaluator)

protected:

	virtual void OnBuild(const FCameraNodeEvaluatorBuildParams& Params) override;
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) override;
	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	TCameraParameterReader<FVector3d> BoomOffsetReader;
	FCameraRigInput2DSlotEvaluator* InputSlotEvaluator = nullptr;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBoomArmCameraNodeEvaluator)

void FBoomArmCameraNodeEvaluator::OnBuild(const FCameraNodeEvaluatorBuildParams& Params)
{
	const UBoomArmCameraNode* BoomArmNode = GetCameraNodeAs<UBoomArmCameraNode>();
	InputSlotEvaluator = Params.BuildEvaluatorAs<FCameraRigInput2DSlotEvaluator>(BoomArmNode->InputSlot);
}

void FBoomArmCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	const UBoomArmCameraNode* BoomArmNode = GetCameraNodeAs<UBoomArmCameraNode>();
	BoomOffsetReader.Initialize(BoomArmNode->BoomOffset);
}

FCameraNodeEvaluatorChildrenView FBoomArmCameraNodeEvaluator::OnGetChildren()
{
	return FCameraNodeEvaluatorChildrenView({ InputSlotEvaluator });
}

void FBoomArmCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	FRotator3d BoomRotation = FRotator3d::ZeroRotator;
	if (InputSlotEvaluator)
	{
		InputSlotEvaluator->Run(Params, OutResult);
		const FVector2d YawPitch = InputSlotEvaluator->GetInputValue();
		BoomRotation = FRotator3d(YawPitch.Y, YawPitch.X, 0);
	}
	else if (Params.EvaluationContext)
	{
		if (APlayerController* PlayerController = Params.EvaluationContext->GetPlayerController())
		{
			if (APawn* Pawn = PlayerController->GetPawn())
			{
				const FRotator3d PawnViewRotation = Pawn->GetViewRotation();
				BoomRotation = PawnViewRotation;
			}
		}
	}

	const UBoomArmCameraNode* BoomArmNode = GetCameraNodeAs<UBoomArmCameraNode>();

	// Here we want to logically apply transform in this order:
	//
	// FinalTransform = BoomOffset * BoomRotation * CameraPose.Location
	//
	// Since FTransform3d applies rotation first and translation second, we can save one multiplication
	// by using the fact that BoomRotation is, well, just a rotation, and CameraPose.Location is of
	// course just a translation. So we can put them both in the same transform:
	const FTransform3d BoomPivot(BoomRotation, OutResult.CameraPose.GetLocation());
	const FTransform3d BoomOffset(BoomOffsetReader.Get(OutResult.VariableTable));

	const FTransform3d FinalTransform(BoomOffset * BoomPivot);

	OutResult.CameraPose.SetTransform(FinalTransform);
}

}  // namespace UE::Cameras

FCameraNodeChildrenView UBoomArmCameraNode::OnGetChildren()
{
	return FCameraNodeChildrenView({ InputSlot });
}

FCameraNodeEvaluatorPtr UBoomArmCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FBoomArmCameraNodeEvaluator>();
}

