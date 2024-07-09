// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Input/CameraRigInput1DSlot.h"

#include "Core/CameraBuildLog.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigBuildContext.h"
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableTable.h"
#include "Nodes/Input/Input1DCameraNode.h"

#define LOCTEXT_NAMESPACE "CameraRigInputSlot"

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FCameraRigInput1DSlotEvaluator)

FCameraRigInput1DSlotEvaluator::FCameraRigInput1DSlotEvaluator()
{
	SetNodeEvaluatorFlags(ECameraNodeEvaluatorFlags::NeedsParameterUpdate | ECameraNodeEvaluatorFlags::NeedsEvaluationUpdate);
}

void FCameraRigInput1DSlotEvaluator::OnBuild(const FCameraNodeEvaluatorBuildParams& Params)
{
	const UCameraRigInput1DSlot* SlotNode = GetCameraNodeAs<UCameraRigInput1DSlot>();
	ChildEvaluator = Params.BuildEvaluatorAs<FInput1DCameraNodeEvaluator>(SlotNode->Child);
}

void FCameraRigInput1DSlotEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	TransientInputValue = 0.f;
	InputValue = 0.f;

	const UCameraRigInput1DSlot* SlotNode = GetCameraNodeAs<UCameraRigInput1DSlot>();
	if (SlotNode->GetVariableID().IsValid() && Params.LastActiveCameraRig.IsValid())
	{
		const FCameraVariableTable& LastActiveRigVariableTable = Params.LastActiveCameraRig.LastResult->VariableTable;
		LastActiveRigVariableTable.TryGetValue<double>(SlotNode->GetVariableID(), InputValue);
	}
}

FCameraNodeEvaluatorChildrenView FCameraRigInput1DSlotEvaluator::OnGetChildren()
{
	return FCameraNodeEvaluatorChildrenView{ ChildEvaluator };
}

void FCameraRigInput1DSlotEvaluator::OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult)
{
	if (ChildEvaluator)
	{
		ChildEvaluator->UpdateParameters(Params, OutResult);

		TransientInputValue = ChildEvaluator->GetInputValue();
	}

	const UCameraRigInput1DSlot* SlotNode = GetCameraNodeAs<UCameraRigInput1DSlot>();
	if (SlotNode->InputSlotParameters.bIsPreBlended)
	{
		OutResult.VariableTable.SetValue<double>(SlotNode->GetTransientVariableID(), TransientInputValue);
	}
}

void FCameraRigInput1DSlotEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UCameraRigInput1DSlot* SlotNode = GetCameraNodeAs<UCameraRigInput1DSlot>();

	if (SlotNode->InputSlotParameters.bIsPreBlended)
	{
		TransientInputValue = OutResult.VariableTable.GetValue<double>(SlotNode->GetTransientVariableID());
	}

	if (SlotNode->InputSlotParameters.bIsAccumulated)
	{
		InputValue += TransientInputValue;
	}
	else
	{
		InputValue = TransientInputValue;
	}

	InputValue = SlotNode->Normalize.NormalizeValue(InputValue);
	InputValue = SlotNode->Clamp.ClampValue(InputValue);

	OutResult.VariableTable.SetValue<double>(SlotNode->GetVariableID(), InputValue);
}

}  // namespace UE::Cameras

FCameraNodeChildrenView UCameraRigInput1DSlot::OnGetChildren()
{
	return FCameraNodeChildrenView{ Child };
}

void UCameraRigInput1DSlot::OnBuild(FCameraRigBuildContext& BuildContext) 
{
	using namespace UE::Cameras;

	FCameraVariableDefinition VariableDefinition;

	if (BuiltInVariable != EBuiltInDoubleCameraVariable::None)
	{
		VariableDefinition = FBuiltInCameraVariables::Get().GetDefinition(BuiltInVariable);
	}
	else if (Variable)
	{
		VariableDefinition = Variable->GetVariableDefinition();
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

FCameraNodeEvaluatorPtr UCameraRigInput1DSlot::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	return Builder.BuildEvaluator<UE::Cameras::FCameraRigInput1DSlotEvaluator>();
}

#undef LOCTEXT_NAMESPACE

