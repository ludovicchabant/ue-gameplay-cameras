// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BuiltInCameraVariables.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraVariableReferences.h"
#include "Nodes/Input/CameraRigInputSlotTypes.h"
#include "Nodes/Input/Input1DCameraNode.h"
#include "UObject/ObjectPtr.h"

#include "CameraRigInput1DSlot.generated.h"

class UDoubleCameraVariable;

/**
 * The base class for a node that can handle and accumulate raw player input values.
 */
UCLASS(Abstract)
class UCameraRigInput1DSlot : public UInput1DCameraNode
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
	FDoubleCameraVariableReference CustomVariable;

public:

	FCameraVariableID GetVariableID() const { return VariableID; }
	FCameraVariableID GetTransientVariableID() const { return TransientVariableID; }

protected:

	// UCameraNode interface.
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

class FCameraRigInput1DSlotEvaluator : public FInput1DCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(GAMEPLAYCAMERAS_API, FCameraRigInput1DSlotEvaluator, FInput1DCameraNodeEvaluator)

public:

	FCameraRigInput1DSlotEvaluator();

protected:

	// FCameraNodeEvaluator interface.
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnExecuteOperation(const FCameraOperationParams& Params, FCameraOperation& Operation) override;
	virtual void OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar) override;

protected:

	double TransientInputValue = 0.f;
};

}  // namespace UE::Cameras

