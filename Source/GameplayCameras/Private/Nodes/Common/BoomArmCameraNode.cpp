// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/BoomArmCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraOperation.h"
#include "Core/CameraParameterReader.h"
#include "Core/CameraRigJoints.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Nodes/Input/Input2DCameraNode.h"

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
	virtual void OnExecuteOperation(const FCameraOperationParams& Params, FCameraOperation& Operation) override;

private:

	APlayerController* GetPlayerController(TSharedPtr<const FCameraEvaluationContext> EvaluationContext) const;

private:

	TCameraParameterReader<FVector3d> BoomOffsetReader;
	FInput2DCameraNodeEvaluator* InputSlotEvaluator = nullptr;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBoomArmCameraNodeEvaluator)

void FBoomArmCameraNodeEvaluator::OnBuild(const FCameraNodeEvaluatorBuildParams& Params)
{
	const UBoomArmCameraNode* BoomArmNode = GetCameraNodeAs<UBoomArmCameraNode>();
	InputSlotEvaluator = Params.BuildEvaluatorAs<FInput2DCameraNodeEvaluator>(BoomArmNode->InputSlot);
}

void FBoomArmCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	SetNodeEvaluatorFlags(
			ECameraNodeEvaluatorFlags::NeedsEvaluationUpdate |
			ECameraNodeEvaluatorFlags::SupportsOperations);

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
	else if (APlayerController* PlayerController = GetPlayerController(Params.EvaluationContext))
	{
		const FRotator3d ControlRotation = PlayerController->GetControlRotation();
		BoomRotation = ControlRotation;
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
	
	OutResult.CameraRigJoints.AddYawPitchJoint(BoomPivot);
}

void FBoomArmCameraNodeEvaluator::OnExecuteOperation(const FCameraOperationParams& Params, FCameraOperation& Operation)
{
	if (InputSlotEvaluator)
	{
		InputSlotEvaluator->ExecuteOperation(Params, Operation);
	}
	else
	{
		// If we don't have an input slot, we use the pawn rotation directly in OnRun. So let's handle
		// some operations by affecting that pawn rotation ourselves.
		if (FYawPitchCameraOperation* Op = Operation.CastOperation<FYawPitchCameraOperation>())
		{
			if (APlayerController* PlayerController = GetPlayerController(Params.EvaluationContext))
			{
				FRotator3d ControlRotation = PlayerController->GetControlRotation();
				ControlRotation.Yaw = Op->Yaw.Apply(ControlRotation.Yaw);
				ControlRotation.Pitch = Op->Pitch.Apply(ControlRotation.Pitch);
				PlayerController->SetControlRotation(ControlRotation);
			}
		}
	}
}

APlayerController* FBoomArmCameraNodeEvaluator::GetPlayerController(TSharedPtr<const FCameraEvaluationContext> EvaluationContext) const
{
	if (EvaluationContext)
	{
		return EvaluationContext->GetPlayerController();
	}
	return nullptr;
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

