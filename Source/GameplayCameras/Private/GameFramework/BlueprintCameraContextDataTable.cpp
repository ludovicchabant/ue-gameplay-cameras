// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/BlueprintCameraContextDataTable.h"

#include "Core/CameraContextDataTable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlueprintCameraContextDataTable)

FBlueprintCameraContextDataTable::FBlueprintCameraContextDataTable()
	: PrivateContextDataTable(nullptr)
{
}

FBlueprintCameraContextDataTable::FBlueprintCameraContextDataTable(FCameraContextDataTable* InContextDataTable)
	: PrivateContextDataTable(InContextDataTable)
{
}

FInstancedStruct UBlueprintCameraContextDataTableFunctionLibrary::GetStructData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const UScriptStruct* DataStructType)
{
	using namespace UE::Cameras;

	if (FCameraContextDataTable* ActualTable = ContextDataTable.GetContextDataTable())
	{
		if (DataID.IsValid())
		{
			const uint8* RawData = ActualTable->GetData(DataID, ECameraContextDataType::Struct, DataStructType);
			FInstancedStruct ReturnValue;
			ReturnValue.InitializeAs(DataStructType, RawData);
			return ReturnValue;
		}
	}

	return FInstancedStruct();
}

void UBlueprintCameraContextDataTableFunctionLibrary::SetStructData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const FInstancedStruct& Data)
{
	using namespace UE::Cameras;

	if (FCameraContextDataTable* ActualTable = ContextDataTable.GetContextDataTable())
	{
		if (DataID.IsValid())
		{
			ActualTable->SetData(DataID, Data);
		}
	}
}

