// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Input/InputAxisBinding2DCameraNode.h"

#include "Components/InputComponent.h"
#include "Core/CameraEvaluationContext.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Actor.h"
#include "InputAction.h"

namespace UE::Cameras
{

class FInputAxisBinding2DCameraNodeEvaluator : public FInput2DCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(GAMEPLAYCAMERAS_API, FInputAxisBinding2DCameraNodeEvaluator, FInput2DCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) override;
	virtual void OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult) override;

private:

	TObjectPtr<UEnhancedInputComponent> InputComponent;
	FEnhancedInputActionValueBinding* AxisValueBinding = nullptr;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FInputAxisBinding2DCameraNodeEvaluator)

void FInputAxisBinding2DCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	if (UObject* ContextOwner = Params.EvaluationContext->GetOwner())
	{
		if (AActor* OuterActor = ContextOwner->GetTypedOuter<AActor>())
		{
			InputComponent = Cast<UEnhancedInputComponent>(OuterActor->InputComponent);
		}
	}

	const UInputAxisBinding2DCameraNode* AxisBindingNode = GetCameraNodeAs<UInputAxisBinding2DCameraNode>();
	if (InputComponent && AxisBindingNode->AxisAction)
	{
		AxisValueBinding = &InputComponent->BindActionValue(AxisBindingNode->AxisAction);
	}
}

void FInputAxisBinding2DCameraNodeEvaluator::OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult)
{
	const UInputAxisBinding2DCameraNode* AxisBindingNode = GetCameraNodeAs<UInputAxisBinding2DCameraNode>();
	if (AxisValueBinding)
	{
		const FVector2d Value = AxisValueBinding->GetValue().Get<FVector2D>();

		InputValue = FVector2d(
				Value.X * AxisBindingNode->Multiplier.X, 
				Value.Y * AxisBindingNode->Multiplier.Y);

		if (AxisBindingNode->RevertAxisX)
		{
			InputValue.X = -InputValue.X;
		}
		if (AxisBindingNode->RevertAxisY)
		{
			InputValue.Y = -InputValue.Y;
		}
	}
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

