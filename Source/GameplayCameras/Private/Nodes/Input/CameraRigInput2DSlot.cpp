// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Input/CameraRigInput2DSlot.h"

#include "Core/CameraBuildLog.h"
#include "Core/CameraOperation.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigBuildContext.h"
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableTable.h"
#include "Nodes/Input/Input2DCameraNode.h"

#define LOCTEXT_NAMESPACE "CameraRigInputSlot"

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FCameraRigInput2DSlotEvaluator)

FCameraRigInput2DSlotEvaluator::FCameraRigInput2DSlotEvaluator()
{
	AddNodeEvaluatorFlags(ECameraNodeEvaluatorFlags::NeedsParameterUpdate | ECameraNodeEvaluatorFlags::SupportsOperations);
}

void FCameraRigInput2DSlotEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	TransientInputValue = FVector2d::ZeroVector;
	InputValue = FVector2d::ZeroVector;

	const UCameraRigInput2DSlot* SlotNode = GetCameraNodeAs<UCameraRigInput2DSlot>();
	if (SlotNode->GetVariableID().IsValid() && Params.LastActiveCameraRigInfo.LastResult)
	{
		const FCameraVariableTable& LastActiveRigVariableTable = Params.LastActiveCameraRigInfo.LastResult->VariableTable;
		LastActiveRigVariableTable.TryGetValue<FVector2d>(SlotNode->GetVariableID(), InputValue);
	}
}

void FCameraRigInput2DSlotEvaluator::OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult)
{
	const UCameraRigInput2DSlot* SlotNode = GetCameraNodeAs<UCameraRigInput2DSlot>();
	if (SlotNode->InputSlotParameters.bIsPreBlended)
	{
		OutResult.VariableTable.SetValue<FVector2d>(SlotNode->GetTransientVariableID(), TransientInputValue);
	}
}

void FCameraRigInput2DSlotEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UCameraRigInput2DSlot* SlotNode = GetCameraNodeAs<UCameraRigInput2DSlot>();

	if (SlotNode->InputSlotParameters.bIsPreBlended)
	{
		TransientInputValue = OutResult.VariableTable.GetValue<FVector2d>(SlotNode->GetTransientVariableID());
	}

	if (SlotNode->InputSlotParameters.bIsAccumulated)
	{
		InputValue += TransientInputValue;
	}
	else
	{
		InputValue = TransientInputValue;
	}

	InputValue.X = SlotNode->NormalizeX.NormalizeValue(InputValue.X);
	InputValue.Y = SlotNode->NormalizeY.NormalizeValue(InputValue.Y);

	InputValue.X = SlotNode->ClampX.ClampValue(InputValue.X);
	InputValue.Y = SlotNode->ClampY.ClampValue(InputValue.Y);

	OutResult.VariableTable.SetValue<FVector2d>(SlotNode->GetVariableID(), InputValue);
}

void FCameraRigInput2DSlotEvaluator::OnExecuteOperation(const FCameraOperationParams& Params, FCameraOperation& Operation)
{
	if (FYawPitchCameraOperation* Op = Operation.CastOperation<FYawPitchCameraOperation>())
	{
		const UCameraRigInput2DSlot* SlotNode = GetCameraNodeAs<UCameraRigInput2DSlot>();

		double MinValueX, MaxValueX;
		SlotNode->ClampX.GetEffectiveClamping(MinValueX, MaxValueX);

		double MinValueY, MaxValueY;
		SlotNode->ClampY.GetEffectiveClamping(MinValueY, MaxValueY);

		InputValue.X = Op->Yaw.Apply(InputValue.X, MinValueX, MaxValueX);
		InputValue.Y = Op->Pitch.Apply(InputValue.Y, MinValueY, MaxValueY);
	}
}

void FCameraRigInput2DSlotEvaluator::OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar)
{
	Super::OnSerialize(Params, Ar);

	Ar << TransientInputValue;
}

}  // namespace UE::Cameras

void UCameraRigInput2DSlot::OnBuild(FCameraRigBuildContext& BuildContext)
{
	using namespace UE::Cameras;

	FCameraVariableDefinition VariableDefinition;

	if (BuiltInVariable != EBuiltInVector2dCameraVariable::None)
	{
		VariableDefinition = FBuiltInCameraVariables::Get().GetDefinition(BuiltInVariable);
	}
	else if (CustomVariable.IsValid())
	{
		VariableDefinition = CustomVariable.Get()->GetVariableDefinition();
	}
	else if (InputSlotParameters.bIsPreBlended)
	{
		BuildContext.BuildLog.AddMessage(
				EMessageSeverity::Error,
				this,
				LOCTEXT("PreBlendedInputSlotRequiresVariable",
					"An input slot with pre-blend enabled must specify a variable (built-in or custom) "
					"to blend with other input slots"));
	}

	if (VariableDefinition.IsValid())
	{
		VariableDefinition.bIsInput = true;

		FCameraVariableTableAllocationInfo& VariableTableInfo = BuildContext.AllocationInfo.VariableTableInfo;
		VariableTableInfo.VariableDefinitions.Add(VariableDefinition);

		FCameraVariableDefinition TransientVariableDefinition = VariableDefinition.CreateVariant(TEXT("Transient"));
		VariableTableInfo.VariableDefinitions.Add(TransientVariableDefinition);

		VariableID = VariableDefinition.VariableID;
		TransientVariableID = TransientVariableDefinition.VariableID;
	}
}

FCameraNodeEvaluatorPtr UCameraRigInput2DSlot::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	return Builder.BuildEvaluator<UE::Cameras::FCameraRigInput2DSlotEvaluator>();
}

#undef LOCTEXT_NAMESPACE

