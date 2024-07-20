// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BuiltInCameraVariables.h"
#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Nodes/Input/CameraRigInputSlotTypes.h"
#include "UObject/ObjectPtr.h"

#include "CameraRigInput2DSlot.generated.h"

class UInput2DCameraNode;
class UVector2dCameraVariable;

/**
 * A node that can handle and accumulate a chain of player input nodes.
 */
UCLASS(MinimalAPI, meta=(
			CameraNodeCategories="Input", 
			ObjectTreeGraphSelfPinDirection="Output",
			ObjectTreeGraphDefaultPropertyPinDirection="Input"))
class UCameraRigInput2DSlot : public UCameraNode
{
	GENERATED_BODY()

public:

	/** Input processing parameters. */
	UPROPERTY(EditAnywhere, Category="Input", meta=(ShowOnlyInnerProperties))
	FCameraRigInputSlotParameters InputSlotParameters;

	/** Clamping of the final input value. */
	UPROPERTY(EditAnywhere, Category="Input")
	FCameraParameterClamping ClampX;

	/** Clamping of the final input value. */
	UPROPERTY(EditAnywhere, Category="Input")
	FCameraParameterClamping ClampY;

	/** Normalization of the final input value. */
	UPROPERTY(EditAnywhere, Category="Input")
	FCameraParameterNormalization NormalizeX;

	/** Normalization of the final input value. */
	UPROPERTY(EditAnywhere, Category="Input")
	FCameraParameterNormalization NormalizeY;

	/** The variable to use to blend with other input slots. */
	UPROPERTY(EditAnywhere, Category="Input")
	EBuiltInVector2dCameraVariable BuiltInVariable = EBuiltInVector2dCameraVariable::YawPitch;

	/** The variable to use to blend with other input slots. */
	UPROPERTY(EditAnywhere, Category="Input", meta=(EditCondition="BuiltInVariable == EBuiltInVector2dCameraVariable::None"))
	TObjectPtr<UVector2dCameraVariable> Variable;

	/** A node providing an incremental value. */
	UPROPERTY()
	TObjectPtr<UInput2DCameraNode> Child;

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

class FInput2DCameraNodeEvaluator;

class FCameraRigInput2DSlotEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FCameraRigInput2DSlotEvaluator)

public:

	FCameraRigInput2DSlotEvaluator();

	FVector2d GetInputValue() const { return InputValue; }

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

	FInput2DCameraNodeEvaluator* ChildEvaluator;

	FVector2d TransientInputValue = FVector2d::ZeroVector;
	FVector2d InputValue = FVector2d::ZeroVector;
};

}  // namespace UE::Cameras


