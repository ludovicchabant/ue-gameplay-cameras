// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraParameters.h"
#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectPtr.h"

#include "CameraRigParameterInterop.generated.h"

class UCameraRigAsset;
class UCameraEvaluationResultInterop;
class UCameraVariableAsset;

/**
 * Blueprint internal methods to set values on a camera rig's exposed parameters.
 *
 * These functions are internal because users are supposed to use the K2Node_SetCameraRigParameters node instead. That node then
 * gets compiled into one or more of these internal functions.
 */
UCLASS(MinimalAPI)
class UCameraRigParameterInterop : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UCameraRigParameterInterop(const FObjectInitializer& ObjectInit);

public:

	UFUNCTION(BlueprintCallable, Category="Camera", meta=(BlueprintInternalUseOnly="true"))
	static void SetBooleanParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, bool bParameterValue);

	UFUNCTION(BlueprintCallable, Category="Camera", meta=(BlueprintInternalUseOnly="true"))
	static void SetIntegerParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, int32 ParameterValue);

	UFUNCTION(BlueprintCallable, Category="Camera", meta=(BlueprintInternalUseOnly="true"))
	static void SetFloatParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, double ParameterValue);

	UFUNCTION(BlueprintCallable, Category="Camera", meta=(BlueprintInternalUseOnly="true"))
	static void SetDoubleParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, double ParameterValue);

	UFUNCTION(BlueprintCallable, Category="Camera", meta=(BlueprintInternalUseOnly="true"))
	static void SetVector2Parameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector2D ParameterValue);

	UFUNCTION(BlueprintCallable, Category="Camera", meta=(BlueprintInternalUseOnly="true"))
	static void SetVector3Parameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector ParameterValue);

	UFUNCTION(BlueprintCallable, Category="Camera", meta=(BlueprintInternalUseOnly="true"))
	static void SetVector4Parameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector4 ParameterValue);

	UFUNCTION(BlueprintCallable, Category="Camera", meta=(BlueprintInternalUseOnly="true"))
	static void SetRotatorParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, FRotator ParameterValue);

	UFUNCTION(BlueprintCallable, Category="Camera", meta=(BlueprintInternalUseOnly="true"))
	static void SetTransformParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, FTransform ParameterValue);

private:

	static UCameraVariableAsset* GetParameterPrivateVariable(UCameraRigAsset* CameraRig, const FString& ParameterName);
};

