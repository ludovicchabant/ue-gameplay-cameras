// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/CameraEvaluationResultInterop.h"

#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraPose.h"
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableTable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraEvaluationResultInterop)

#define UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_VALIDATE_RESULT(ErrorResult)\
	if (!Result)\
	{\
		FFrame::KismetExecutionMessage(TEXT("No camera evaluation result has been set"), ELogVerbosity::Error);\
		return ErrorResult;\
	}

#define UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_VALIDATE_VARIABLE_ASSET_PARAM(ErrorResult)\
	if (!InVariableAsset)\
	{\
		FFrame::KismetExecutionMessage(TEXT("No camera variable asset was given"), ELogVerbosity::Error);\
		return ErrorResult;\
	}\

#define UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_GET_VARIABLE(VariableType)\
	static VariableType ErrorResult {};\
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_VALIDATE_RESULT(ErrorResult)\
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_VALIDATE_VARIABLE_ASSET_PARAM(ErrorResult)\
	return Result->VariableTable.GetValue<VariableType>(InVariableAsset->GetVariableID());

#define UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_SET_VARIABLE(VariableType)\
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_VALIDATE_RESULT()\
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_VALIDATE_VARIABLE_ASSET_PARAM()\
	Result->VariableTable.SetValue(InVariableAsset, InValue, true);

UCameraEvaluationResultInterop::UCameraEvaluationResultInterop(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UCameraEvaluationResultInterop::Setup(FCameraNodeEvaluationResult* InResult)
{
	Result = InResult;
}

void UCameraEvaluationResultInterop::Teardown()
{
	Result = nullptr;
}

const FCameraPose& UCameraEvaluationResultInterop::GetCameraPose() const
{
	static FCameraPose ErrorResult;
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_VALIDATE_RESULT(ErrorResult);
	return Result->CameraPose;
}

bool UCameraEvaluationResultInterop::GetBooleanCameraVariable(UBooleanCameraVariable* InVariableAsset) const
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_GET_VARIABLE(bool);
}

int32 UCameraEvaluationResultInterop::GetInteger32CameraVariable(UInteger32CameraVariable* InVariableAsset) const
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_GET_VARIABLE(int32);
}

float UCameraEvaluationResultInterop::GetFloatCameraVariable(UFloatCameraVariable* InVariableAsset) const
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_GET_VARIABLE(float);
}

double UCameraEvaluationResultInterop::GetDoubleCameraVariable(UDoubleCameraVariable* InVariableAsset) const
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_GET_VARIABLE(double);
}

FVector2D UCameraEvaluationResultInterop::GetVector2CameraVariable(UVector2dCameraVariable* InVariableAsset) const
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_GET_VARIABLE(FVector2d);
}

FVector UCameraEvaluationResultInterop::GetVector3CameraVariable(UVector3dCameraVariable* InVariableAsset) const
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_GET_VARIABLE(FVector3d);
}

FVector4 UCameraEvaluationResultInterop::GetVector4CameraVariable(UVector4dCameraVariable* InVariableAsset) const
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_GET_VARIABLE(FVector4d);
}

FRotator UCameraEvaluationResultInterop::GetRotatorCameraVariable(URotator3dCameraVariable* InVariableAsset) const
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_GET_VARIABLE(FRotator3d);
}

FTransform UCameraEvaluationResultInterop::GetTransformCameraVariable(UTransform3dCameraVariable* InVariableAsset) const
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_GET_VARIABLE(FTransform3d);
}

void UCameraEvaluationResultInterop::SetCameraPose(const FCameraPose& InCameraPose)
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_VALIDATE_RESULT();
	Result->CameraPose = InCameraPose;
	// TODO: auto-set flags based on differences
}

void UCameraEvaluationResultInterop::SetBooleanCameraVariable(UBooleanCameraVariable* InVariableAsset, bool InValue)
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_SET_VARIABLE(bool);
}

void UCameraEvaluationResultInterop::SetInteger32CameraVariable(UInteger32CameraVariable* InVariableAsset, int32 InValue)
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_SET_VARIABLE(int32);
}

void UCameraEvaluationResultInterop::SetFloatCameraVariable(UFloatCameraVariable* InVariableAsset, float InValue)
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_SET_VARIABLE(float);
}

void UCameraEvaluationResultInterop::SetDoubleCameraVariable(UDoubleCameraVariable* InVariableAsset, double InValue)
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_SET_VARIABLE(double);
}

void UCameraEvaluationResultInterop::SetVector2CameraVariable(UVector2dCameraVariable* InVariableAsset, const FVector2D& InValue)
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_SET_VARIABLE(FVector2d);
}

void UCameraEvaluationResultInterop::SetVector3CameraVariable(UVector3dCameraVariable* InVariableAsset, const FVector& InValue)
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_SET_VARIABLE(FVector3d);
}

void UCameraEvaluationResultInterop::SetVector4CameraVariable(UVector4dCameraVariable* InVariableAsset, const FVector4& InValue)
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_SET_VARIABLE(FVector4d);
}

void UCameraEvaluationResultInterop::SetRotatorCameraVariable(URotator3dCameraVariable* InVariableAsset, const FRotator& InValue)
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_SET_VARIABLE(FRotator3d);
}

void UCameraEvaluationResultInterop::SetTransformCameraVariable(UTransform3dCameraVariable* InVariableAsset, const FTransform& InValue)
{
	UE_PRIVATE_CAMERA_EVALUATION_RESULT_BLUEPRINT_INTEROP_SET_VARIABLE(FTransform3d);
}

