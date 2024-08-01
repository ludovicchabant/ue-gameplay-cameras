// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Input/InputAxisBinding2DCameraNode.h"

#include "Components/InputComponent.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraParameterReader.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Actor.h"
#include "GameplayCameras.h"
#include "InputAction.h"
#include "UObject/Package.h"

namespace UE::Cameras
{

class FInputAxisBinding2DCameraNodeEvaluator : public FCameraRigInput2DSlotEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(GAMEPLAYCAMERAS_API, FInputAxisBinding2DCameraNodeEvaluator, FCameraRigInput2DSlotEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult) override;

private:

	TObjectPtr<UEnhancedInputComponent> InputComponent;

	TCameraParameterReader<bool> RevertAxisXReader;
	TCameraParameterReader<bool> RevertAxisYReader;
	TCameraParameterReader<FVector2d> MultiplierReader;

	TArray<FEnhancedInputActionValueBinding*> AxisValueBindings;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FInputAxisBinding2DCameraNodeEvaluator)

void FInputAxisBinding2DCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	UObject* ContextOwner = Params.EvaluationContext->GetOwner();
	if (ContextOwner)
	{
		if (AActor* ContextOwnerActor = Cast<AActor>(ContextOwner))
		{
			InputComponent = Cast<UEnhancedInputComponent>(ContextOwnerActor->InputComponent);
		}
		else if (AActor* OuterActor = ContextOwner->GetTypedOuter<AActor>())
		{
			InputComponent = Cast<UEnhancedInputComponent>(OuterActor->InputComponent);
		}
	}

	const UInputAxisBinding2DCameraNode* AxisBindingNode = GetCameraNodeAs<UInputAxisBinding2DCameraNode>();

	RevertAxisXReader.Initialize(AxisBindingNode->RevertAxisX);
	RevertAxisYReader.Initialize(AxisBindingNode->RevertAxisY);
	MultiplierReader.Initialize(AxisBindingNode->Multiplier);

	if (InputComponent)
	{
		for (TObjectPtr<UInputAction> AxisAction : AxisBindingNode->AxisActions)
		{
			FEnhancedInputActionValueBinding* AxisValueBinding = &InputComponent->BindActionValue(AxisAction);
			AxisValueBindings.Add(AxisValueBinding);
		}
	}
	else
	{
		UE_LOG(LogCameraSystem, Error, TEXT("No input component found on context owner '%s' for node '%s' in '%s'."),
				*GetNameSafe(ContextOwner), 
				*GetNameSafe(AxisBindingNode),
				*GetNameSafe(AxisBindingNode ? AxisBindingNode->GetOutermost() : nullptr));
	}

	Super::OnInitialize(Params, OutResult);
}

void FInputAxisBinding2DCameraNodeEvaluator::OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult)
{
	const UInputAxisBinding2DCameraNode* AxisBindingNode = GetCameraNodeAs<UInputAxisBinding2DCameraNode>();

	FVector2d HighestValue(FVector2d::ZeroVector);
	double HighestSquaredLenth = 0.f;

	for (FEnhancedInputActionValueBinding* AxisValueBinding : AxisValueBindings)
	{
		if (!AxisValueBinding)
		{
			continue;
		}

		const FVector2d Value = AxisValueBinding->GetValue().Get<FVector2D>();
		const double ValueSquaredLength = Value.SquaredLength();
		if (ValueSquaredLength > HighestSquaredLenth)
		{
			HighestValue = Value;
		}
	}

	const FVector2d Multiplier = MultiplierReader.Get(OutResult.VariableTable);
	TransientInputValue = FVector2d(HighestValue.X * Multiplier.X, HighestValue.Y * Multiplier.Y);

	if (RevertAxisXReader.Get(OutResult.VariableTable))
	{
		TransientInputValue.X = -TransientInputValue.X;
	}
	if (RevertAxisYReader.Get(OutResult.VariableTable))
	{
		TransientInputValue.Y = -TransientInputValue.Y;
	}

	Super::OnUpdateParameters(Params, OutResult);
}

}  // namespace UE::Cameras

UInputAxisBinding2DCameraNode::UInputAxisBinding2DCameraNode(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
	Multiplier = FVector2D(1, 1);
}

FCameraNodeEvaluatorPtr UInputAxisBinding2DCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FInputAxisBinding2DCameraNodeEvaluator>();
}

