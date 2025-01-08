// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/BlueprintCameraContextDataTable.h"

#include "Core/CameraContextDataTable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlueprintCameraContextDataTable)

#define UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(ErrorResult)\
	using namespace UE::Cameras;\
	if (!DataID.IsValid())\
	{\
		FFrame::KismetExecutionMessage(TEXT("Invalid camera context data ID"), ELogVerbosity::Error);\
		return ErrorResult;\
	}\
	if (!ContextDataTable.IsValid())\
	{\
		FFrame::KismetExecutionMessage(TEXT("No camera context data table has been set"), ELogVerbosity::Error);\
		return ErrorResult;\
	}\
	FCameraContextDataTable* ActualTable = ContextDataTable.GetContextDataTable();

FBlueprintCameraContextDataTable::FBlueprintCameraContextDataTable()
	: PrivateContextDataTable(nullptr)
{
}

FBlueprintCameraContextDataTable::FBlueprintCameraContextDataTable(FCameraContextDataTable* InContextDataTable)
	: PrivateContextDataTable(InContextDataTable)
{
}

FName UBlueprintCameraContextDataTableFunctionLibrary::GetNameData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(NAME_None);
	return ActualTable->GetNameData(DataID);
}

FString UBlueprintCameraContextDataTableFunctionLibrary::GetStringData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(FString());
	return ActualTable->GetStringData(DataID);
}

uint8 UBlueprintCameraContextDataTableFunctionLibrary::GetEnumData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const UEnum* EnumType)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(0);
	return ActualTable->GetEnumData(DataID, EnumType);
}

FInstancedStruct UBlueprintCameraContextDataTableFunctionLibrary::GetStructData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const UScriptStruct* DataStructType)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(FInstancedStruct());
	return ActualTable->GetInstancedStructData(DataID, DataStructType);
}

UObject* UBlueprintCameraContextDataTableFunctionLibrary::GetObjectData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(nullptr);
	return ActualTable->GetObjectData(DataID);
}

UClass* UBlueprintCameraContextDataTableFunctionLibrary::GetClassData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(nullptr);
	return ActualTable->GetClassData(DataID);
}

bool UBlueprintCameraContextDataTableFunctionLibrary::SetNameData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const FName& Data)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(false);
	ActualTable->SetNameData(DataID, Data);
	return true;
}

bool UBlueprintCameraContextDataTableFunctionLibrary::SetStringData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const FString& Data)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(false);
	ActualTable->SetStringData(DataID, Data);
	return true;
}

bool UBlueprintCameraContextDataTableFunctionLibrary::SetEnumData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const UEnum* EnumType, uint8 Data)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(false);
	ActualTable->SetEnumData(DataID, EnumType, Data);
	return true;
}

bool UBlueprintCameraContextDataTableFunctionLibrary::SetStructData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const FInstancedStruct& Data)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(false);
	ActualTable->SetInstancedStructData(DataID, Data);
	return true;
}

bool UBlueprintCameraContextDataTableFunctionLibrary::SetObjectData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, UObject* Data)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(false);
	ActualTable->SetObjectData(DataID, Data);
	return true;
}

bool UBlueprintCameraContextDataTableFunctionLibrary::SetClassData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, UClass* Data)
{
	UE_PRIVATE_BLUEPRINT_CAMERA_CONTEXT_DATA_TABLE_VALIDATE(false);
	ActualTable->SetClassData(DataID, Data);
	return true;
}

