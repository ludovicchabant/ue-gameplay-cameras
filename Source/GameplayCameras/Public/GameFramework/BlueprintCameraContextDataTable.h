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

/** Provides access to a camera context data table. */
USTRUCT(BlueprintType, DisplayName="Camera Context Data Table")
struct GAMEPLAYCAMERAS_API FBlueprintCameraContextDataTable
{
	GENERATED_BODY()

public:

	using FCameraContextDataTable = UE::Cameras::FCameraContextDataTable;

	FBlueprintCameraContextDataTable();
	FBlueprintCameraContextDataTable(FCameraContextDataTable* InContextDataTable);

	/** Gets the underlying context data table. */
	FCameraContextDataTable* GetContextDataTable() const { return PrivateContextDataTable; }

	/** Returns whether this context data table is valid. */
	bool IsValid() const { return PrivateContextDataTable != nullptr; }

private:

	FCameraContextDataTable* PrivateContextDataTable = nullptr;
};

/**
 * Utility Blueprint functions for camera context data tables.
 */
UCLASS()
class UBlueprintCameraContextDataTableFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Gets a value from the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static FName GetNameData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID);

	/** Gets a value from the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static FString GetStringData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID);

	/** Gets a value from the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera, meta=(DeterminesOutputType="EnumType"))
	static uint8 GetEnumData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const UEnum* EnumType);

	/** Gets a value from the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static FInstancedStruct GetStructData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const UScriptStruct* DataStructType);

	/** Gets a value from the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static UObject* GetObjectData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID);

	/** Gets a value from the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static UClass* GetClassData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID);

public:

	/** Sets a value in the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static bool SetNameData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const FName& Data);

	/** Sets a value in the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static bool SetStringData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const FString& Data);

	/** Sets a value in the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static bool SetEnumData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const UEnum* EnumType, uint8 Data);

	/** Sets a value in the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static bool SetStructData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, const FInstancedStruct& Data);

	/** Sets a value in the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static bool SetObjectData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, UObject* Data);

	/** Sets a value in the given camera context data table. */
	UFUNCTION(BlueprintCallable, Category=Camera)
	static bool SetClassData(const FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID, UClass* Data);
};

