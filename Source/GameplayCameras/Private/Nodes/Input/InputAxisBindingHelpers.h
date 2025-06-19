// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/Array.h"

class UCameraNode;
class UEnhancedInputComponent;
class UInputAction;
struct FEnhancedInputActionValueBinding;

namespace UE::Cameras { struct FCameraNodeEvaluatorInitializeParams; }

namespace UE::Cameras
{

class FInputAxisBindingHelpers
{
public:

	static UEnhancedInputComponent* FindInputComponent(const UE::Cameras::FCameraNodeEvaluatorInitializeParams& Params);

	static void BindActionValues(
			const UE::Cameras::FCameraNodeEvaluatorInitializeParams& Params,
			const UCameraNode* CameraNode,
			UEnhancedInputComponent* InputComponent,
			const TArray<TObjectPtr<UInputAction>>& AxisActions,
			TArray<FEnhancedInputActionValueBinding*>& OutAxisValueBindings);

	static FVector2d GetHighestValue(const TArray<FEnhancedInputActionValueBinding*>& AxisValueBindings);
};

}  // namespace UE::Cameras

