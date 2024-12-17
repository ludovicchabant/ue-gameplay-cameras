// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraContextDataTable.h"

#include "Core/CameraContextDataAllocationInfo.h"

namespace UE::Cameras
{

FCameraContextDataTable::FCameraContextDataTable()
{
}

FCameraContextDataTable::~FCameraContextDataTable()
{
}

void FCameraContextDataTable::AddReferencedObjects(FReferenceCollector& ReferenceCollector)
{
	for (FEntry& Entry : Entries)
	{
		ReferenceCollector.AddReferencedObject(Entry.TypeObject);

		uint8* RawData = Memory + Entry.Offset;
		switch (Entry.Type)
		{
			case ECameraContextDataType::Struct:
				{
					const UScriptStruct* StructType = CastChecked<const UScriptStruct>(Entry.TypeObject);
					ReferenceCollector.AddPropertyReferencesWithStructARO(StructType, RawData);
				}
				break;
			case ECameraContextDataType::Object:
				{
					TObjectPtr<UObject>* TypedData = reinterpret_cast<TObjectPtr<UObject>*>(RawData);
					ReferenceCollector.AddReferencedObject(*TypedData);
				}
				break;
			case ECameraContextDataType::Class:
				{
					TObjectPtr<UClass>* TypedData = reinterpret_cast<TObjectPtr<UClass>*>(RawData);
					ReferenceCollector.AddReferencedObject(*TypedData);
				}
				break;
		}
	}
}

void FCameraContextDataTable::Initialize(const FCameraContextDataAllocationInfo& AllocationInfo)
{
	// Reset any previous state.
	DestroyBuffer();
	Entries.Reset();
	EntryLookup.Reset();

	// Compute the total buffer size we need, and create our entries as we go.
	const uint32 FirstAlignOf = 32u;
	uint32 TotalSizeOf = 0;
	uint32 CurSizeOf, CurAlignOf;

	for (const FCameraContextDataDefinition& DataDefinition : AllocationInfo.DataDefinitions)
	{
		GetDataTypeAllocationInfo(DataDefinition.DataType, DataDefinition.DataTypeObject, CurSizeOf, CurAlignOf);
		const uint32 NewEntryOffset = Align(TotalSizeOf, CurAlignOf);
		TotalSizeOf = NewEntryOffset + CurSizeOf;

		FEntry NewEntry;
		NewEntry.ID = DataDefinition.DataID;
		NewEntry.Type = DataDefinition.DataType;
		NewEntry.TypeObject = DataDefinition.DataTypeObject;
		NewEntry.Offset = NewEntryOffset;
		NewEntry.Flags = EEntryFlags::None;
#if WITH_EDITORONLY_DATA
		NewEntry.DebugName = DataDefinition.DataID.DataName;
#endif

		Entries.Add(NewEntry);
		EntryLookup.Add(NewEntry.ID, Entries.Num() - 1);
	}

	// Allocate the memory buffer.
	Memory = reinterpret_cast<uint8*>(FMemory::Malloc(TotalSizeOf, FirstAlignOf));
	Capacity = TotalSizeOf;
	Used = TotalSizeOf;

	// Go back to our entries and initialize each entry to the default value for that data type.
	for (const FEntry& Entry : Entries)
	{
		uint8* DataPtr = Memory + Entry.Offset;
		InitializeDefaultDataValue(Entry.Type, Entry.TypeObject, DataPtr);
	}
}

void FCameraContextDataTable::AddData(const FCameraContextDataDefinition& DataDefinition)
{
	if (!ensure(!EntryLookup.Contains(DataDefinition.DataID)))
	{
		return;
	}

	uint32 SizeOf, AlignOf;
	GetDataTypeAllocationInfo(DataDefinition.DataType, DataDefinition.DataTypeObject, SizeOf, AlignOf);

	uint8* DataPtr = Align(Memory + Used, AlignOf);
	uint32 NewUsed = (DataPtr + SizeOf) - Memory;

	if (NewUsed > Capacity)
	{
		ReallocateBuffer(NewUsed);

		DataPtr = Align(Memory + Used, AlignOf);
	}

	Used = NewUsed;

	FEntry NewEntry;
	NewEntry.ID = DataDefinition.DataID;
	NewEntry.Type = DataDefinition.DataType;
	NewEntry.TypeObject = DataDefinition.DataTypeObject;
	NewEntry.Offset = DataPtr - Memory;
	NewEntry.Flags = EEntryFlags::None;
#if WITH_EDITORONLY_DATA
	NewEntry.DebugName = DataDefinition.DataID.DataName;
#endif
	
	Entries.Add(NewEntry);
	EntryLookup.Add(DataDefinition.DataID, Entries.Num() - 1);

	InitializeDefaultDataValue(NewEntry.Type, NewEntry.TypeObject, Memory + NewEntry.Offset);
}

bool FCameraContextDataTable::GetDataTypeAllocationInfo(ECameraContextDataType DataType, const UObject* DataTypeObject, uint32& OutSizeOf, uint32& OutAlignOf)
{
	switch (DataType)
	{
		case ECameraContextDataType::Name:
			OutSizeOf = sizeof(FName);
			OutAlignOf = alignof(FName);
			break;
		case ECameraContextDataType::String:
			OutSizeOf = sizeof(FString);
			OutAlignOf = alignof(FString);
			break;
		case ECameraContextDataType::Enum:
			OutSizeOf = sizeof(uint8);
			OutAlignOf = alignof(uint8);
			break;
		case ECameraContextDataType::Struct:
			{
				const UScriptStruct* StructType = CastChecked<const UScriptStruct>(DataTypeObject);
				if (StructType)
				{
					OutSizeOf = StructType->PropertiesSize;
					OutAlignOf = StructType->MinAlignment;
				}
			}
			break;
		case ECameraContextDataType::Object:
			OutSizeOf = sizeof(TObjectPtr<UObject>);
			OutAlignOf = alignof(TObjectPtr<UObject>);
			break;
		case ECameraContextDataType::Class:
			OutSizeOf = sizeof(TObjectPtr<UClass>);
			OutAlignOf = alignof(TObjectPtr<UClass>);
			break;
		default:
			ensure(false);
			return false;
	}
	return true;
}

bool FCameraContextDataTable::InitializeDefaultDataValue(ECameraContextDataType DataType, const UObject* DataTypeObject, uint8* DataPtr)
{
	switch (DataType)
	{
		case ECameraContextDataType::Name:
			new (DataPtr) FName();
			break;
		case ECameraContextDataType::String:
			new (DataPtr) FString();
			break;
		case ECameraContextDataType::Enum:
			*reinterpret_cast<uint8*>(DataPtr) = (uint8)CastChecked<const UEnum>(DataTypeObject)->GetValueByIndex(0);
			break;
		case ECameraContextDataType::Struct:
			CastChecked<const UScriptStruct>(DataTypeObject)->InitializeDefaultValue(DataPtr);
			break;
		case ECameraContextDataType::Object:
			new (DataPtr) TObjectPtr<UObject>();
			break;
		case ECameraContextDataType::Class:
			new (DataPtr) TObjectPtr<UClass>();
			break;
		default:
			ensure(false);
			return false;
	}
	return true;
}

void FCameraContextDataTable::ReallocateBuffer(uint32 MinRequired)
{
	static const uint32 DefaultCapacity = 64u;
	static const uint32 DefaultAlignment = 32u;

	uint32 NewCapacity = Capacity <= 0 ? DefaultCapacity : Capacity * 2;
	if (MinRequired > 0)
	{
		NewCapacity = FMath::Max(NewCapacity, MinRequired);
	}

	uint8* OldMemory = Memory;
	uint8* NewMemory = reinterpret_cast<uint8*>(FMemory::Malloc(NewCapacity, DefaultAlignment));

	if (OldMemory)
	{
		// UE implements names, strings, and UStructs as bitwise relocatable so we can just
		// move the memory around.
		FMemory::Memmove(NewMemory, OldMemory, Capacity);
		FMemory::Free(OldMemory);
	}

	Memory = NewMemory;
	Capacity = NewCapacity;
}

void FCameraContextDataTable::DestroyBuffer()
{
	if (!Memory)
	{
		return;
	}

	for (const FEntry& Entry : Entries)
	{
		uint8* DataPtr = Memory + Entry.Offset;

		switch (Entry.Type)
		{
			case ECameraContextDataType::Name:
				reinterpret_cast<FName*>(DataPtr)->~FName();
				break;
			case ECameraContextDataType::String:
				reinterpret_cast<FString*>(DataPtr)->~FString();
				break;
			case ECameraContextDataType::Enum:
				// Nothing to do.
				break;
			case ECameraContextDataType::Struct:
				CastChecked<const UScriptStruct>(Entry.TypeObject)->DestroyStruct(DataPtr);
				break;
			case ECameraContextDataType::Object:
				reinterpret_cast<TObjectPtr<UObject>*>(DataPtr)->~TObjectPtr<UObject>();
				break;
			case ECameraContextDataType::Class:
				reinterpret_cast<TObjectPtr<UClass>*>(DataPtr)->~TObjectPtr<UClass>();
				break;
			default:
				ensure(false);
				break;
		}
	}

	FMemory::Free(Memory);

	Memory = nullptr;
	Capacity = 0;
	Used = 0;
}

void FCameraContextDataTable::SetData(FCameraContextDataID InID, const FStructView& InData)
{
	FEntry* Entry = FindEntry(InID);
	if (ensure(Entry && Entry->Type == ECameraContextDataType::Struct && InData.GetScriptStruct() == Entry->TypeObject))
	{
		uint8* DataPtr = Memory + Entry->Offset;
		const UScriptStruct* StructType = CastChecked<const UScriptStruct>(Entry->TypeObject);
		StructType->CopyScriptStruct(DataPtr, InData.GetMemory());
		EnumAddFlags(Entry->Flags, EEntryFlags::Written | EEntryFlags::WrittenThisFrame);
	}
}

void FCameraContextDataTable::SetData(FCameraContextDataID InID, const FInstancedStruct& InData)
{
	FEntry* Entry = FindEntry(InID);
	if (ensure(Entry && Entry->Type == ECameraContextDataType::Struct && InData.GetScriptStruct() == Entry->TypeObject))
	{
		uint8* DataPtr = Memory + Entry->Offset;
		const UScriptStruct* StructType = CastChecked<const UScriptStruct>(Entry->TypeObject);
		StructType->CopyScriptStruct(DataPtr, InData.GetMemory());
		EnumAddFlags(Entry->Flags, EEntryFlags::Written | EEntryFlags::WrittenThisFrame);
	}
}

const FCameraContextDataTable::FEntry* FCameraContextDataTable::FindEntry(FCameraContextDataID InID) const
{
	const uint32 EntryIndex = EntryLookup.FindRef(InID, INDEX_NONE);
	if (ensure(Entries.IsValidIndex(EntryIndex)))
	{
		return &Entries[EntryIndex];
	}
	return nullptr;
}

FCameraContextDataTable::FEntry* FCameraContextDataTable::FindEntry(FCameraContextDataID InID)
{
	const uint32 EntryIndex = EntryLookup.FindRef(InID, INDEX_NONE);
	if (ensure(Entries.IsValidIndex(EntryIndex)))
	{
		return &Entries[EntryIndex];
	}
	return nullptr;
}

const uint8* FCameraContextDataTable::GetData(FCameraContextDataID DataID, ECameraContextDataType ExpectedDataType, const UObject* ExpectedDataTypeObject) const
{
	const uint8* Data = TryGetData(DataID, ExpectedDataType, ExpectedDataTypeObject);
	ensureMsgf(
			Data, 
			TEXT("Can't get camera context data (ID '%s') because it doesn't exist in the table."), 
			*DataID.DataName.ToString());
	return Data;
}

const uint8* FCameraContextDataTable::TryGetData(FCameraContextDataID DataID, ECameraContextDataType ExpectedDataType, const UObject* ExpectedDataTypeObject) const
{
	const FEntry* Entry = FindEntry(DataID);
	if (Entry)
	{
		if (ensure(Entry->Type == ExpectedDataType && Entry->TypeObject == ExpectedDataTypeObject))
		{
			return Memory + Entry->Offset;
		}
	}

	return nullptr;
}

void FCameraContextDataTable::SetData(FCameraContextDataID DataID, ECameraContextDataType ExpectedDataType, const UObject* ExpectedDataTypeObject, const uint8* InRawDataPtr, bool bMarkAsWrittenThisFrame)
{
	const bool bDidSet = TrySetData(DataID, ExpectedDataType, ExpectedDataTypeObject, InRawDataPtr, bMarkAsWrittenThisFrame);
	ensureMsgf(bDidSet, TEXT("Can't set camera context data (ID '%s') beacuse it doesn't exist in the table."), *DataID.DataName.ToString());
}

bool FCameraContextDataTable::TrySetData(FCameraContextDataID DataID, ECameraContextDataType ExpectedDataType, const UObject* ExpectedDataTypeObject, const uint8* InRawDataPtr, bool bMarkAsWrittenThisFrame)
{
	FEntry* Entry = FindEntry(DataID);
	if (!Entry)
	{
		return false;
	}

	if (!ensure(ExpectedDataType == Entry->Type && ExpectedDataTypeObject == Entry->TypeObject))
	{
		return false;
	}

	uint8* DataPtr = Memory + Entry->Offset;
	SetDataValue(Entry->Type, Entry->TypeObject, DataPtr, InRawDataPtr);

	Entry->Flags |= EEntryFlags::Written;
	if (bMarkAsWrittenThisFrame)
	{
		Entry->Flags |= EEntryFlags::WrittenThisFrame;
	}
	
	return true;
}

bool FCameraContextDataTable::SetDataValue(ECameraContextDataType DataType, const UObject* DataTypeObject, uint8* DestDataPtr, const uint8* SrcDataPtr)
{
	switch (DataType)
	{
		case ECameraContextDataType::Name:
			*reinterpret_cast<FName*>(DestDataPtr) = *reinterpret_cast<const FName*>(SrcDataPtr);
			break;
		case ECameraContextDataType::String:
			*reinterpret_cast<FString*>(DestDataPtr) = *reinterpret_cast<const FString*>(SrcDataPtr);
			break;
		case ECameraContextDataType::Enum:
			*reinterpret_cast<uint8*>(DestDataPtr) = *reinterpret_cast<const uint8*>(SrcDataPtr);
			break;
		case ECameraContextDataType::Struct:
			CastChecked<const UScriptStruct>(DataTypeObject)->CopyScriptStruct(DestDataPtr, SrcDataPtr);
			break;
		case ECameraContextDataType::Object:
			*reinterpret_cast<TObjectPtr<UObject>*>(DestDataPtr) = *reinterpret_cast<const TObjectPtr<UObject>*>(SrcDataPtr);
			break;
		case ECameraContextDataType::Class:
			*reinterpret_cast<TObjectPtr<UClass>*>(DestDataPtr) = *reinterpret_cast<const TObjectPtr<UClass>*>(SrcDataPtr);
			break;
		default:
			ensure(false);
			return false;
	}
	return true;
}

bool FCameraContextDataTable::IsValueWritten(FCameraContextDataID InID) const
{
	if (const FEntry* Entry = FindEntry(InID))
	{
		return EnumHasAnyFlags(Entry->Flags, EEntryFlags::Written);
	}
	return false;
}

void FCameraContextDataTable::UnsetValue(FCameraContextDataID InID)
{
	if (FEntry* Entry = FindEntry(InID))
	{
		return EnumRemoveFlags(Entry->Flags, EEntryFlags::Written);
	}
}

void FCameraContextDataTable::UnsetAllValues()
{
	for (FEntry& Entry : Entries)
	{
		EnumRemoveFlags(Entry.Flags, EEntryFlags::Written);
	}
}

bool FCameraContextDataTable::IsValueWrittenThisFrame(FCameraContextDataID InID) const
{
	if (const FEntry* Entry = FindEntry(InID))
	{
		return EnumHasAnyFlags(Entry->Flags, EEntryFlags::WrittenThisFrame);
	}
	return false;
}

void FCameraContextDataTable::ClearAllWrittenThisFrameFlags()
{
	for (FEntry& Entry : Entries)
	{
		EnumRemoveFlags(Entry.Flags, EEntryFlags::WrittenThisFrame);
	}
}

void FCameraContextDataTable::OverrideAll(const FCameraContextDataTable& OtherTable)
{
	Override(OtherTable, false);
}

void FCameraContextDataTable::OverrideKnown(const FCameraContextDataTable& OtherTable)
{
	Override(OtherTable, true);
}

void FCameraContextDataTable::Override(const FCameraContextDataTable& OtherTable, bool bKnownOnly)
{
	for (const FEntry& OtherEntry : OtherTable.Entries)
	{
		const EEntryFlags OtherFlags = OtherEntry.Flags;
		if (EnumHasAnyFlags(OtherFlags, EEntryFlags::Written))
		{
			int32 ThisIndex = EntryLookup.FindRef(OtherEntry.ID, INDEX_NONE);
			if (ThisIndex == INDEX_NONE)
			{
				if (bKnownOnly)
				{
					continue;
				}

				FCameraContextDataDefinition OtherEntryDefinition;
				OtherEntryDefinition.DataID = OtherEntry.ID;
				OtherEntryDefinition.DataType = OtherEntry.Type;
				OtherEntryDefinition.DataTypeObject = OtherEntry.TypeObject;
				AddData(OtherEntryDefinition);
				ThisIndex = Entries.Num() - 1;
			}

			ensure(ThisIndex != INDEX_NONE);
			
			FEntry& ThisEntry = Entries[ThisIndex];
			if (!ensure(ThisEntry.Type == OtherEntry.Type && ThisEntry.TypeObject == OtherEntry.TypeObject))
			{
				continue;
			}

			uint8* ThisDataPtr = Memory + ThisEntry.Offset;
			uint8* OtherDataPtr = OtherTable.Memory + OtherEntry.Offset;
			SetDataValue(ThisEntry.Type, ThisEntry.TypeObject, ThisDataPtr, OtherDataPtr);

			EnumAddFlags(ThisEntry.Flags, EEntryFlags::Written | (OtherFlags & EEntryFlags::WrittenThisFrame));
		}
	}
}

}  // namespace UE::Cameras

