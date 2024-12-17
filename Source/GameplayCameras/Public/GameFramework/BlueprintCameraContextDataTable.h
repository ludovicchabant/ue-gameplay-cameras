// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraContextDataTableFwd.h"
#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/ObjectMacros.h"

#include "BlueprintCameraContextDataTable.generated.h"

namespace UE::Cameras
{

class FCameraContextDataTable;

}  // namespace UE::Cameras

USTRUCT(BlueprintType, DisplayName="Camera Context Data Table")
struct GAMEPLAYCAMERAS_API FBlueprintCameraContextDataTable
{
	GENERATED_BODY()

public:

	using FCameraContextDataTable = UE::Cameras::FCameraContextDataTable;

	FBlueprintCameraContextDataTable();
	FBlueprintCameraContextDataTable(FCameraContextDataTable* InContextDataTable);

	FCameraContextDataTable* GetContextDataTable() const { return PrivateContextDataTable; }


private:

	FCameraContextDataTable* PrivateContextDataTable = nullptr;
};

UCLASS()
class UBlueprintCameraContextDataTableFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category=Camera)
	static FInstancedStruct GetStructData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const UScriptStruct* DataStructType);

	UFUNCTION(BlueprintCallable, Category=Camera)
	static void SetStructData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const FInstancedStruct& Data);
};

