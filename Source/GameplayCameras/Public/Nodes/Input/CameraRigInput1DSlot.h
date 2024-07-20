// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BuiltInCameraVariables.h"
#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Nodes/Input/CameraRigInputSlotTypes.h"
#include "UObject/ObjectPtr.h"

#include "CameraRigInput1DSlot.generated.h"

class UDoubleCameraVariable;
class UInput1DCameraNode;

/**
 * A node that can handle and accumulate a chain of player input nodes.
 */
UCLASS(MinimalAPI, meta=(
			CameraNodeCategories="Input", 
			ObjectTreeGraphSelfPinDirection="Output", 
			ObjectTreeGraphDefaultPropertyPinDirection="Input"))
class UCameraRigInput1DSlot : public UCameraNode
{
	GENERATED_BODY()

public:

	/** Input processing parameters. */
	UPROPERTY(EditAnywhere, Category="Input", meta=(ShowOnlyInnerProperties))
	FCameraRigInputSlotParameters InputSlotParameters;

	/** Clamping of the final input value. */
	UPROPERTY(EditAnywhere, Category="Input")
	FCameraParameterClamping Clamp;

	/** Normalization of the final input value. */
	UPROPERTY(EditAnywhere, Category="Input")
	FCameraParameterNormalization Normalize;

	/** The variable to use to blend with other input slots. */
	UPROPERTY(EditAnywhere, Category="Input")
	EBuiltInDoubleCameraVariable BuiltInVariable = EBuiltInDoubleCameraVariable::Yaw;

	/** The variable to use to blend with other input slots. */
	UPROPERTY(EditAnywhere, Category="Input", meta=(EditCondition="BuiltInVariable == EBuiltInDoubleCameraVariable::None"))
	TObjectPtr<UDoubleCameraVariable> Variable;

	/** A node providing an incremental value. */
	UPROPERTY()
	TObjectPtr<UInput1DCameraNode> Child;

public:

	FCameraVariableID GetVariableID() const { return VariableID; }
	FCameraVariableID GetTransientVariableID() const { return TransientVariableID; }

protected:

	// UCameraNode interface.
	virtual FCameraNodeChildrenView OnGetChildren() override;
	virtual void OnBuild(FCameraRigBuildContext& BuildContext) override;
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

private:

	UPROPERTY()
	FCameraVariableID TransientVariableID;
	UPROPERTY()
	FCameraVariableID VariableID;
};

namespace UE::Cameras
{

class FInput1DCameraNodeEvaluator;

class FCameraRigInput1DSlotEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FCameraRigInput1DSlotEvaluator)

public:

	FCameraRigInput1DSlotEvaluator();

	double GetInputValue() const { return InputValue; }

protected:

	// FCameraNodeEvaluator interface.
	virtual void OnBuild(const FCameraNodeEvaluatorBuildParams& Params) override;
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) override;
	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnExecuteOperation(const FCameraOperationParams& Params, FCameraOperation& Operation) override;
	virtual void OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar) override;

protected:

	FInput1DCameraNodeEvaluator* ChildEvaluator;

	double TransientInputValue = 0.f;
	double InputValue = 0.f;
};

}  // namespace UE::Cameras

