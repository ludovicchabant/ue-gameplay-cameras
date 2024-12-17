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

	template<typename DataType>
	const DataType* GetData(FCameraContextDataID InID) const;

	// Setter methods.

	template<typename DataType>
	void SetData(FCameraContextDataID InID, const DataType& InData);

	void SetData(FCameraContextDataID InID, const FStructView& InData);
	void SetData(FCameraContextDataID InID, const FInstancedStruct& InData);

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

template<typename DataType>
const DataType* FCameraContextDataTable::GetData(FCameraContextDataID InID) const
{
	const FEntry* Entry = FindEntry(InID);
	if (Entry)
	{
		ensureMsgf(Entry->Type == DataType::StaticStruct(), TEXT("Data type mismatch!"));
		const uint8* RawData = Memory + Entry->Offset;
		return reinterpret_cast<const DataType*>(RawData);
	}
	return nullptr;
}

template<typename DataType>
void FCameraContextDataTable::SetData(FCameraContextDataID InID, const DataType& InData)
{
	const FEntry* Entry = FindEntry(InID);
	if (ensure(Entry))
	{
		ensureMsgf(Entry->Type == DataType::StaticStruct(), TEXT("Data type mismatch!"));
		uint8* RawData = Memory + Entry->Offset;
		*reinterpret_cast<DataType*>(RawData) = InData;
	}
}

}  // namespace UE::Cameras

