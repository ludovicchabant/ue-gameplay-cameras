// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/BlueprintCameraVariableTable.h"

#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableTable.h"
#include "Services/AutoResetCameraVariableService.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlueprintCameraVariableTable)

#define UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_VALIDATE(ErrorResult)\
	if (!VariableTable.IsValid())\
	{\
		FFrame::KismetExecutionMessage(TEXT("No camera variable table has been set"), ELogVerbosity::Error);\
		return ErrorResult;\
	}

#define UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_VALIDATE_PARAM(ErrorResult)\
	if (!Variable)\
	{\
		FFrame::KismetExecutionMessage(TEXT("No camera variable asset was given"), ELogVerbosity::Error);\
		return ErrorResult;\
	}\

#define UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_GET_VARIABLE(VariableType)\
	static VariableType ErrorResult {};\
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_VALIDATE(ErrorResult)\
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_VALIDATE_PARAM(ErrorResult)\
	return VariableTable.GetVariableTable()->GetValue<VariableType>(Variable->GetVariableID(), Variable->GetDefaultValue());

#define UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_SET_VARIABLE(VariableType)\
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_VALIDATE()\
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_VALIDATE_PARAM()\
	VariableTable.GetVariableTable()->SetValue(Variable, Value, true);\
	if (Variable->bAutoReset && VariableTable.VariableAutoResetService)\
	{\
		VariableTable.VariableAutoResetService->RegisterVariableUseFromScripting(Variable);\
	}

FBlueprintCameraVariableTable::FBlueprintCameraVariableTable()
	: PrivateVariableTable(nullptr)
{
}

FBlueprintCameraVariableTable::FBlueprintCameraVariableTable(FCameraVariableTable* InVariableTable, TSharedPtr<UE::Cameras::FAutoResetCameraVariableService> InVariableAutoResetService)
	: PrivateVariableTable(InVariableTable)
	, VariableAutoResetService(InVariableAutoResetService)
{
}

bool UBlueprintCameraVariableTableFunctionLibrary::GetBooleanCameraVariable(const FBlueprintCameraVariableTable& VariableTable, UBooleanCameraVariable* Variable)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_GET_VARIABLE(bool);
}

int32 UBlueprintCameraVariableTableFunctionLibrary::GetInteger32CameraVariable(const FBlueprintCameraVariableTable& VariableTable, UInteger32CameraVariable* Variable)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_GET_VARIABLE(int32);
}

float UBlueprintCameraVariableTableFunctionLibrary::GetFloatCameraVariable(const FBlueprintCameraVariableTable& VariableTable, UFloatCameraVariable* Variable)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_GET_VARIABLE(float);
}

double UBlueprintCameraVariableTableFunctionLibrary::GetDoubleCameraVariable(const FBlueprintCameraVariableTable& VariableTable, UDoubleCameraVariable* Variable)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_GET_VARIABLE(double);
}

FVector2D UBlueprintCameraVariableTableFunctionLibrary::GetVector2CameraVariable(const FBlueprintCameraVariableTable& VariableTable, UVector2dCameraVariable* Variable)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_GET_VARIABLE(FVector2d);
}

FVector UBlueprintCameraVariableTableFunctionLibrary::GetVector3CameraVariable(const FBlueprintCameraVariableTable& VariableTable, UVector3dCameraVariable* Variable)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_GET_VARIABLE(FVector3d);
}

FVector4 UBlueprintCameraVariableTableFunctionLibrary::GetVector4CameraVariable(const FBlueprintCameraVariableTable& VariableTable, UVector4dCameraVariable* Variable)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_GET_VARIABLE(FVector4d);
}

FRotator UBlueprintCameraVariableTableFunctionLibrary::GetRotatorCameraVariable(const FBlueprintCameraVariableTable& VariableTable, URotator3dCameraVariable* Variable)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_GET_VARIABLE(FRotator3d);
}

FTransform UBlueprintCameraVariableTableFunctionLibrary::GetTransformCameraVariable(const FBlueprintCameraVariableTable& VariableTable, UTransform3dCameraVariable* Variable)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_GET_VARIABLE(FTransform3d);
}

void UBlueprintCameraVariableTableFunctionLibrary::SetBooleanCameraVariable(const FBlueprintCameraVariableTable& VariableTable, UBooleanCameraVariable* Variable, bool Value)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_SET_VARIABLE(bool);
}

void UBlueprintCameraVariableTableFunctionLibrary::SetInteger32CameraVariable(const FBlueprintCameraVariableTable& VariableTable, UInteger32CameraVariable* Variable, int32 Value)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_SET_VARIABLE(int32);
}

void UBlueprintCameraVariableTableFunctionLibrary::SetFloatCameraVariable(const FBlueprintCameraVariableTable& VariableTable, UFloatCameraVariable* Variable, float Value)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_SET_VARIABLE(float);
}

void UBlueprintCameraVariableTableFunctionLibrary::SetDoubleCameraVariable(const FBlueprintCameraVariableTable& VariableTable, UDoubleCameraVariable* Variable, double Value)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_SET_VARIABLE(double);
}

void UBlueprintCameraVariableTableFunctionLibrary::SetVector2CameraVariable(const FBlueprintCameraVariableTable& VariableTable, UVector2dCameraVariable* Variable, const FVector2D& Value)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_SET_VARIABLE(FVector2d);
}

void UBlueprintCameraVariableTableFunctionLibrary::SetVector3CameraVariable(const FBlueprintCameraVariableTable& VariableTable, UVector3dCameraVariable* Variable, const FVector& Value)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_SET_VARIABLE(FVector3d);
}

void UBlueprintCameraVariableTableFunctionLibrary::SetVector4CameraVariable(const FBlueprintCameraVariableTable& VariableTable, UVector4dCameraVariable* Variable, const FVector4& Value)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_SET_VARIABLE(FVector4d);
}

void UBlueprintCameraVariableTableFunctionLibrary::SetRotatorCameraVariable(const FBlueprintCameraVariableTable& VariableTable, URotator3dCameraVariable* Variable, const FRotator& Value)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_SET_VARIABLE(FRotator3d);
}

void UBlueprintCameraVariableTableFunctionLibrary::SetTransformCameraVariable(const FBlueprintCameraVariableTable& VariableTable, UTransform3dCameraVariable* Variable, const FTransform& Value)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_VARIABLE_TABLE_SET_VARIABLE(FTransform3d);
}

