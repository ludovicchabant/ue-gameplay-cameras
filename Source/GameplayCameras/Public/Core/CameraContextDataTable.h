// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraContextDataTableFwd.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "GameplayCameras.h"
#include "Math/NumericLimits.h"
#include "StructUtils/InstancedStruct.h"
#include "StructUtils/StructView.h"

struct FCameraContextDataAllocationInfo;
struct FCameraContextDataDefinition;

namespace UE::Cameras
{

class FCameraContextDataTable;

#if UE_GAMEPLAY_CAMERAS_DEBUG
class FContextDataTableDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

template<typename DataType>
struct TCameraContextDataReader
{
	void Initialize(FCameraContextDataID InID, const FCameraContextDataTable& InContextDataTable);
};

/**
 * The camera context data table is a container for a collection of arbitrary values
 * of various types. It is the companion of the camera variable table (see FCameraVariableTable)
 * but for non-blendable values.
 */
class FCameraContextDataTable
{
public:

	FCameraContextDataTable();
	~FCameraContextDataTable();

	/** Initializes the context data table so that it fits the provided allocation info. */
	void Initialize(const FCameraContextDataAllocationInfo& AllocationInfo);

	/** Adds a data entry to the table. */
	void AddData(const FCameraContextDataDefinition& DataDefinition);

public:

	// Getter methods.

	const FName& GetNameData(FCameraContextDataID InID) const;
	const FString& GetStringData(FCameraContextDataID InID) const;
	uint8 GetEnumData(FCameraContextDataID InID, const UEnum* EnumType) const;
	FConstStructView GetStructViewData(FCameraContextDataID InID, const UScriptStruct* StructType) const;
	FInstancedStruct GetInstancedStructData(FCameraContextDataID InID, const UScriptStruct* StructType) const;
	UObject* GetObjectData(FCameraContextDataID InID) const;
	UClass* GetClassData(FCameraContextDataID InID) const;
	
	template<typename EnumType>
	EnumType GetEnumData(FCameraContextDataID InID) const;

	template<typename StructType>
	const StructType& GetStructData(FCameraContextDataID InID) const;

	template<typename ObjectClass>
	ObjectClass* GetObjectData(FCameraContextDataID InID) const;

	template<typename BaseClass>
	TSubclassOf<BaseClass> GetClassData(FCameraContextDataID InID) const;

	// Setter methods.

	void SetNameData(FCameraContextDataID InID, const FName& InData);
	void SetStringData(FCameraContextDataID InID, const FString& InData);
	void SetEnumData(FCameraContextDataID InID, const UEnum* EnumType, uint8 InData);
	void SetObjectData(FCameraContextDataID InID, UObject* InData);
	void SetClassData(FCameraContextDataID InID, UClass* InData);

	template<typename EnumType>
	void SetEnumData(FCameraContextDataID InID, EnumType InData);

	template<typename StructType>
	void SetStructData(FCameraContextDataID InID, const StructType& InData);

	void SetStructViewData(FCameraContextDataID InID, const FStructView& InData);
	void SetInstancedStructData(FCameraContextDataID InID, const FInstancedStruct& InData);

public:

	// Overriding.

	void OverrideAll(const FCameraContextDataTable& OtherTable);
	void OverrideKnown(const FCameraContextDataTable& OtherTable);

public:

	/** Collects referenced objects. */
	void AddReferencedObjects(FReferenceCollector& ReferenceCollector);

public:

	// Low-level API.
	const uint8* GetData(FCameraContextDataID DataID, ECameraContextDataType ExpectedDataType, const UObject* ExpectedDataTypeObject) const;
	const uint8* TryGetData(FCameraContextDataID DataID, ECameraContextDataType ExpectedDataType, const UObject* ExpectedDataTypeObject) const;

	void SetData(FCameraContextDataID DataID, ECameraContextDataType ExpectedDataType, const UObject* ExpectedDataTypeObject, const uint8* InRawDataPtr, bool bMarkAsWrittenThisFrame = true);
	bool TrySetData(FCameraContextDataID DataID, ECameraContextDataType ExpectedDataType, const UObject* ExpectedDataTypeObject, const uint8* InRawDataPtr, bool bMarkAsWrittenThisFrame = true);

	bool IsValueWritten(FCameraContextDataID InID) const;
	void UnsetValue(FCameraContextDataID InID);
	void UnsetAllValues();

	bool IsValueWrittenThisFrame(FCameraContextDataID InID) const;
	void ClearAllWrittenThisFrameFlags();

private:

	enum class EEntryFlags : uint8
	{
		None = 0,
		Written = 1 << 2,
		WrittenThisFrame = 1 << 3
	};
	FRIEND_ENUM_CLASS_FLAGS(EEntryFlags)

	struct FEntry
	{
		FCameraContextDataID ID;
		ECameraContextDataType Type;
		TObjectPtr<const UObject> TypeObject;
		uint32 Offset;
		EEntryFlags Flags;
#if WITH_EDITORONLY_DATA
		FName DebugName;
#endif
	};

	static bool GetDataTypeAllocationInfo(ECameraContextDataType DataType, const UObject* DataTypeObject, uint32& OutSizeOf, uint32& OutAlignOf);
	static bool InitializeDefaultDataValue(ECameraContextDataType DataType, const UObject* DataTypeObject, uint8* DataPtr);
	static bool SetDataValue(ECameraContextDataType DataType, const UObject* DataTypeObject, uint8* DestDataPtr, const uint8* SrcDataPtr);

	const FEntry* FindEntry(FCameraContextDataID InID) const;
	FEntry* FindEntry(FCameraContextDataID InID);

	void Override(const FCameraContextDataTable& OtherTable, bool bKnownOnly);

	void ReallocateBuffer(uint32 MinRequired = 0);
	void DestroyBuffer();

	template<typename StorageType>
	const StorageType* GetDataImpl(FCameraContextDataID InID, ECameraContextDataType DataType, const UObject* DataTypeObject) const;

	template<typename StorageType>
	bool SetDataImpl(FCameraContextDataID InID, ECameraContextDataType DataType, const UObject* DataTypeObject, const StorageType& InData);

private:

	TArray<FEntry> Entries;
	TMap<FCameraContextDataID, int32> EntryLookup;

	uint8* Memory = nullptr;
	uint32 Capacity = 0;
	uint32 Used = 0;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	friend class FContextDataTableDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

ENUM_CLASS_FLAGS(FCameraContextDataTable::EEntryFlags)

template<typename EnumType>
EnumType FCameraContextDataTable::GetEnumData(FCameraContextDataID InID) const
{
	if (const uint8* Value = GetDataImpl<uint8>(InID, ECameraContextDataType::Enum, StaticEnum<EnumType>()))
	{
		return EnumType(*Value);
	}
	return EnumType();
}

template<typename StructType>
const StructType& FCameraContextDataTable::GetStructData(FCameraContextDataID InID) const
{
	if (const StructType* Value = GetDataImpl<StructType>(InID, ECameraContextDataType::Struct, StructType::StaticStruct()))
	{
		return *Value;
	}

	static const StructType DefaultValue;
	return DefaultValue;
}

template<typename ObjectClass>
ObjectClass* FCameraContextDataTable::GetObjectData(FCameraContextDataID InID) const
{
	if (const TObjectPtr<UObject>* Value = GetDataImpl<TObjectPtr<UObject>>(InID, ECameraContextDataType::Object, nullptr))
	{
		return Cast<ObjectClass>(Value->Get());
	}
	return nullptr;
}

template<typename BaseClass>
TSubclassOf<BaseClass> FCameraContextDataTable::GetClassData(FCameraContextDataID InID) const
{
	if (const TObjectPtr<UClass>* Value = GetDataImpl<TObjectPtr<UClass>>(InID, ECameraContextDataType::Class, nullptr))
	{
		return TSubclassOf<BaseClass>(Value->Get());
	}
	return nullptr;
}

template<typename EnumType>
void FCameraContextDataTable::SetEnumData(FCameraContextDataID InID, EnumType InData)
{
	SetDataImpl(InID, ECameraContextDataType::Enum, StaticEnum<EnumType>(), InData);
}

template<typename StructType>
void FCameraContextDataTable::SetStructData(FCameraContextDataID InID, const StructType& InData)
{
	SetDataImpl(InID, ECameraContextDataType::Struct, StructType::StaticStruct(), InData);
}

template<typename StorageType>
const StorageType* FCameraContextDataTable::GetDataImpl(FCameraContextDataID InID, ECameraContextDataType DataType, const UObject* DataTypeObject) const
{
	const FEntry* Entry = FindEntry(InID);
	if (Entry && Entry->Type == DataType && Entry->TypeObject == DataTypeObject)
	{
		const uint8* RawData = Memory + Entry->Offset;
		return reinterpret_cast<const StorageType*>(RawData);
	}
	return nullptr;
}

template<typename StorageType>
bool FCameraContextDataTable::SetDataImpl(FCameraContextDataID InID, ECameraContextDataType DataType, const UObject* DataTypeObject, const StorageType& InData)
{
	FEntry* Entry = FindEntry(InID);
	if (Entry && Entry->Type == DataType && Entry->TypeObject == DataTypeObject)
	{
		uint8* RawData = Memory + Entry->Offset;
		*reinterpret_cast<StorageType*>(RawData) = InData;
		EnumAddFlags(Entry->Flags, EEntryFlags::Written | EEntryFlags::WrittenThisFrame);
		return true;
	}
	return false;
}

}  // namespace UE::Cameras

