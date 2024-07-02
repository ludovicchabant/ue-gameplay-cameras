// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraVariableTable.h"

#include "HAL/UnrealMemory.h"
#include "Math/UnrealMath.h"

namespace UE::Cameras
{

namespace Private
{

static const FString GUnavailableVariableDebugName(TEXT("<no debug info>"));

bool IsVariableInMask(FCameraVariableID VariableID, const FCameraVariableTableFlags* InMask, bool bInvertMask)
{
	if (InMask)
	{
		const bool bIsInMask = InMask->VariableIDs.Contains(VariableID);
		return bInvertMask ? !bIsInMask : bIsInMask;
	}
	return true;
}

}  // namespace Private

FCameraVariableTable::FCameraVariableTable()
{
}

FCameraVariableTable::FCameraVariableTable(FCameraVariableTable&& Other)
	: Entries(MoveTemp(Other.Entries))
	, Memory(Other.Memory)
	, Capacity(Other.Capacity)
	, Used(Other.Used)
{
	Other.Memory = nullptr;
	Other.Capacity = 0;
	Other.Used = 0;
}

FCameraVariableTable& FCameraVariableTable::operator=(FCameraVariableTable&& Other)
{
	Entries = MoveTemp(Other.Entries);
	Memory = Other.Memory;
	Capacity = Other.Capacity;
	Used = Other.Used;

	Other.Memory = nullptr;
	Other.Capacity = 0;
	Other.Used = 0;

	return *this;
}

FCameraVariableTable::~FCameraVariableTable()
{
	if (Memory)
	{
		FMemory::Free(Memory);
		Memory = nullptr;
		Capacity = Used = 0;
	}
}

void FCameraVariableTable::Initialize(const FCameraVariableTableAllocationInfo& AllocationInfo)
{
	// Reset any previous state.
	Entries.Reset();
	if (Memory)
	{
		FMemory::Free(Memory);
		Memory = nullptr;
	}

	// Compute the total buffer size we need, and create our entries as we go.
	uint32 TotalSizeOf = 0;
	uint32 MaxAlignOf = 0;
	uint32 CurSizeOf, CurAlignOf;
	for (const FCameraVariableDefinition& VariableDefinition : AllocationInfo.VariableDefinitions)
	{
		GetVariableTypeAllocationInfo(VariableDefinition.VariableType, CurSizeOf, CurAlignOf);
		const uint32 NewEntryOffset = Align(TotalSizeOf, CurAlignOf);
		TotalSizeOf = NewEntryOffset + CurSizeOf;
		MaxAlignOf = FMath::Max(MaxAlignOf, CurAlignOf);

		FEntry NewEntry;
		NewEntry.ID = VariableDefinition.VariableID;
		NewEntry.Type = VariableDefinition.VariableType;
		NewEntry.Offset = NewEntryOffset;
		NewEntry.Flags = EEntryFlags::None;
		if (VariableDefinition.bIsPrivate)
		{
			NewEntry.Flags |= EEntryFlags::Private;
		}
		if (VariableDefinition.bIsInput)
		{
			NewEntry.Flags |= EEntryFlags::Input;
		}
#if WITH_EDITORONLY_DATA
		NewEntry.DebugName = VariableDefinition.VariableName;
#endif
		Entries.Add(NewEntry.ID, NewEntry);
	}

	// Allocate the memory buffer.
	Memory = reinterpret_cast<uint8*>(FMemory::Malloc(TotalSizeOf, MaxAlignOf));
	Capacity = TotalSizeOf;
	Used = 0;

	// Go back to our entries and initialize each entry to the default value for that variable type.
	for (auto& Pair : Entries)
	{
		const FEntry& Entry = Pair.Value;
		uint8* ValuePtr = Memory + Entry.Offset;
		switch (Entry.Type)
		{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
			case ECameraVariableType::ValueName:\
				{\
					ValueType* TypedValuePtr = reinterpret_cast<ValueType*>(ValuePtr);\
					*TypedValuePtr = ValueType();\
				}\
				break;
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		}
	}
}

void FCameraVariableTable::AddVariable(const FCameraVariableDefinition& VariableDefinition)
{
	uint32 SizeOf, AlignOf;
	GetVariableTypeAllocationInfo(VariableDefinition.VariableType, SizeOf, AlignOf);

	uint8* VariablePtr = Align(Memory + Used, AlignOf);
	uint32 NewUsed = (VariablePtr + SizeOf) - Memory;

	if (NewUsed > Capacity)
	{
		ReallocateBuffer(NewUsed);

		VariablePtr = Align(Memory + Used, AlignOf);
	}

	Used = NewUsed;

	FEntry NewEntry;
	NewEntry.ID = VariableDefinition.VariableID;
	NewEntry.Type = VariableDefinition.VariableType;
	NewEntry.Offset = VariablePtr - Memory;
	NewEntry.Flags = VariableDefinition.bIsPrivate ? EEntryFlags::Private : EEntryFlags::None;
#if WITH_EDITORONLY_DATA
	NewEntry.DebugName = VariableDefinition.VariableName;
#endif

	Entries.Add(VariableDefinition.VariableID, NewEntry);
}

void FCameraVariableTable::ReallocateBuffer(uint32 MinRequired)
{
	static const uint32 DefaultCapacity = 64;
	static const uint32 DefaultAlignment = 32;

	uint32 NewCapacity = Capacity <= 0 ? DefaultCapacity : Capacity * 2;
	if (MinRequired > 0)
	{
		NewCapacity = FMath::Max(NewCapacity, MinRequired);
	}

	uint8* OldMemory = Memory;
	uint8* NewMemory = reinterpret_cast<uint8*>(FMemory::Malloc(NewCapacity, DefaultAlignment));

	if (OldMemory)
	{
		FMemory::Memmove(NewMemory, OldMemory, Capacity);
		FMemory::Free(OldMemory);
	}

	Memory = NewMemory;
	Capacity = NewCapacity;
}

bool FCameraVariableTable::GetVariableTypeAllocationInfo(ECameraVariableType VariableType, uint32& OutSizeOf, uint32& OutAlignOf)
{
	switch (VariableType)
	{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		case ECameraVariableType::ValueName:\
			OutSizeOf = sizeof(ValueType);\
			OutAlignOf = sizeof(ValueType);\
			return true;
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
	}
	return false;
}

bool FCameraVariableTable::ContainsValue(FCameraVariableID VariableID) const
{
	return Entries.Contains(VariableID);
}

bool FCameraVariableTable::IsValueWritten(FCameraVariableID VariableID) const
{
	if (const FEntry* Entry = Entries.Find(VariableID))
	{
		return EnumHasAnyFlags(Entry->Flags, EEntryFlags::Written);
	}
	return false;
}

void FCameraVariableTable::UnsetValue(FCameraVariableID VariableID)
{
	if (FEntry* Entry = Entries.Find(VariableID))
	{
		EnumRemoveFlags(Entry->Flags, EEntryFlags::Written | EEntryFlags::WrittenThisFrame);
	}
}

void FCameraVariableTable::UnsetAllValues()
{
	for (auto& Pair : Entries)
	{
		EnumRemoveFlags(Pair.Value.Flags, EEntryFlags::Written | EEntryFlags::WrittenThisFrame);
	}
}

bool FCameraVariableTable::IsValueWrittenThisFrame(FCameraVariableID VariableID) const
{
	if (const FEntry* Entry = Entries.Find(VariableID))
	{
		return EnumHasAnyFlags(Entry->Flags, EEntryFlags::WrittenThisFrame);
	}
	return false;
}

void FCameraVariableTable::ClearAllWrittenThisFrameFlags()
{
	for (auto& Pair : Entries)
	{
		EnumRemoveFlags(Pair.Value.Flags, EEntryFlags::WrittenThisFrame);
	}
}

void FCameraVariableTable::OverrideAll(const FCameraVariableTable& OtherTable)
{
	const ECameraVariableTableFilter Filter = ECameraVariableTableFilter::All;
	InternalOverride(OtherTable, Filter, nullptr, false, nullptr);
}

void FCameraVariableTable::Override(const FCameraVariableTable& OtherTable, ECameraVariableTableFilter Filter)
{
	InternalOverride(OtherTable, Filter, nullptr, false, nullptr);
}

void FCameraVariableTable::Override(const FCameraVariableTable& OtherTable, ECameraVariableTableFilter Filter, const FCameraVariableTableFlags& InMask, bool bInvertMask, FCameraVariableTableFlags& OutMask)
{
	InternalOverride(OtherTable, Filter, &InMask, bInvertMask, &OutMask);
}

void FCameraVariableTable::InternalOverride(const FCameraVariableTable& OtherTable, ECameraVariableTableFilter Filter, const FCameraVariableTableFlags* InMask, bool bInvertMask, FCameraVariableTableFlags* OutMask)
{
	using namespace UE::Cameras::Private;

	const bool bChangedOnly = EnumHasAnyFlags(Filter, ECameraVariableTableFilter::ChangedOnly);
	const bool bInputs = EnumHasAnyFlags(Filter, ECameraVariableTableFilter::Input);
	const bool bOutputs = EnumHasAnyFlags(Filter, ECameraVariableTableFilter::Output);

	for (auto& OtherPair : OtherTable.Entries)
	{
		// Look for entries in the other table that have been written to, and aren't private.
		const FEntry& OtherEntry  = OtherPair.Value;
		const EEntryFlags OtherFlags = OtherEntry.Flags;
		const bool bOtherEntryIsInput = EnumHasAnyFlags(OtherFlags, EEntryFlags::Input);
		if (EnumHasAnyFlags(OtherFlags, EEntryFlags::Written)
				&& (!bChangedOnly || EnumHasAnyFlags(OtherFlags, EEntryFlags::WrittenThisFrame))
				&& ((bInputs && bOtherEntryIsInput) || (bOutputs && !bOtherEntryIsInput))
				&& !EnumHasAnyFlags(OtherFlags, EEntryFlags::Private)
				&& IsVariableInMask(OtherEntry.ID, InMask, bInvertMask))
		{
			// See if we know this variable.
			FEntry* ThisEntry = Entries.Find(OtherEntry.ID);
			if (ThisEntry)
			{
				// We already have the other table's variable in our table. Let's check
				// that the types match, and then copy the memory.
#if WITH_EDITORONLY_DATA
				checkf(ThisEntry->DebugName == OtherEntry.DebugName,
						TEXT("Camera variable name collision! Expected variable '%d' to be named '%s', but other table has '%s'!"),
						ThisEntry->ID.GetValue(), *ThisEntry->DebugName, *OtherEntry.DebugName);
#endif

#if WITH_EDITORONLY_DATA
				const FString& DebugName = ThisEntry->DebugName;
#else
				const FString& DebugName = GUnavailableVariableDebugName;
#endif
				checkf(ThisEntry->Type == OtherEntry.Type, 
						TEXT("Camera variable name collision! Expected '%d' (%s) to be of type '%s' but other table has type '%s'!"),
						ThisEntry->ID.GetValue(), *DebugName,
						*UEnum::GetValueAsString(ThisEntry->Type), *UEnum::GetValueAsString(OtherEntry.Type));
			}
			else
			{
				// We don't have this variable in our table. Let's add it.
				FCameraVariableDefinition NewVariableDefinition;
				NewVariableDefinition.VariableID = OtherEntry.ID;
				NewVariableDefinition.VariableType = OtherEntry.Type;
#if WITH_EDITORONLY_DATA
				NewVariableDefinition.VariableName = OtherEntry.DebugName;
#endif
				AddVariable(NewVariableDefinition);

				ThisEntry = Entries.Find(OtherEntry.ID);
			}

			if (ensure(ThisEntry))
			{
				uint32 ValueSize = 0, ValueAlignment = 0;
				GetVariableTypeAllocationInfo(ThisEntry->Type, ValueSize, ValueAlignment);
				check(ValueSize != 0);

				uint8* ThisValuePtr = Memory + ThisEntry->Offset;
				const uint8* OtherValuePtr = OtherTable.Memory + OtherEntry.Offset;
				FMemory::Memcpy(ThisValuePtr, OtherValuePtr, ValueSize);
				EnumAddFlags(ThisEntry->Flags, EEntryFlags::Written);

				if (OutMask)
				{
					OutMask->VariableIDs.Add(ThisEntry->ID);
				}
			}
		}
	}
}

void FCameraVariableTable::LerpAll(const FCameraVariableTable& ToTable, float Factor)
{
	const ECameraVariableTableFilter Filter = ECameraVariableTableFilter::All;
	InternalLerp(ToTable, Filter, Factor, nullptr, false, nullptr);
}

void FCameraVariableTable::Lerp(const FCameraVariableTable& ToTable, ECameraVariableTableFilter Filter, float Factor)
{
	InternalLerp(ToTable, Filter, Factor, nullptr, false, nullptr);
}

void FCameraVariableTable::Lerp(const FCameraVariableTable& ToTable, ECameraVariableTableFilter Filter, float Factor, const FCameraVariableTableFlags& InMask, bool bInvertMask, FCameraVariableTableFlags& OutMask)
{
	InternalLerp(ToTable, Filter, Factor, &InMask, false, &OutMask);
}

void FCameraVariableTable::InternalLerp(const FCameraVariableTable& ToTable, ECameraVariableTableFilter Filter, float Factor, const FCameraVariableTableFlags* InMask, bool bInvertMask, FCameraVariableTableFlags* OutMask)
{
	using namespace UE::Cameras::Private;

	const bool bChangedOnly = EnumHasAnyFlags(Filter, ECameraVariableTableFilter::ChangedOnly);
	const bool bInputs = EnumHasAnyFlags(Filter, ECameraVariableTableFilter::Input);
	const bool bOutputs = EnumHasAnyFlags(Filter, ECameraVariableTableFilter::Output);

	for (auto& ToPair : ToTable.Entries)
	{
		// Look for entries in the other table that have been written to, and aren't private.
		const FEntry& ToEntry  = ToPair.Value;
		const EEntryFlags ToFlags = ToEntry.Flags;
		const bool bToEntryIsInput = EnumHasAnyFlags(ToFlags, EEntryFlags::Input);
		if (EnumHasAnyFlags(ToFlags, EEntryFlags::Written)
				&& (!bChangedOnly || EnumHasAnyFlags(ToFlags, EEntryFlags::WrittenThisFrame))
				&& ((bInputs && bToEntryIsInput) || (bOutputs && !bToEntryIsInput))
				&& !EnumHasAnyFlags(ToFlags, EEntryFlags::Private)
				&& IsVariableInMask(ToEntry.ID, InMask, bInvertMask))
		{
			// See if we know this variable.
			FEntry* FromEntry = Entries.Find(ToEntry.ID);
			if (FromEntry)
			{
				// We already have the other table's variable in our table. Let's check
				// that the types match, and then interpolate the values.
#if WITH_EDITORONLY_DATA
				checkf(FromEntry->DebugName == ToEntry.DebugName,
						TEXT("Camera variable name collision! Expected variable '%d' to be named '%s', but other table has '%s'!"),
						FromEntry->ID.GetValue(), *FromEntry->DebugName, *ToEntry.DebugName);
#endif

#if WITH_EDITORONLY_DATA
				const FString& DebugName = FromEntry->DebugName;
#else
				const FString& DebugName = GUnavailableVariableDebugName;
#endif
				checkf(FromEntry->Type == ToEntry.Type, 
						TEXT("Camera variable name collision! Expected '%d' (%s) to be of type '%s' but other table has type '%s'!"),
						FromEntry->ID.GetValue(), *DebugName,
						*UEnum::GetValueAsString(FromEntry->Type), *UEnum::GetValueAsString(ToEntry.Type));

				uint8* FromValuePtr = Memory + FromEntry->Offset;
				const uint8* ToValuePtr = ToTable.Memory + ToEntry.Offset;
				switch (FromEntry->Type)
				{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
					case ECameraVariableType::ValueName:\
						{\
							ValueType& FromValue = *reinterpret_cast<ValueType*>(FromValuePtr);\
							const ValueType& ToValue = *reinterpret_cast<const ValueType*>(ToValuePtr);\
							ValueType InterpValue = TCameraVariableInterpolation<ValueType>::Interpolate(*FromEntry, FromValue, ToValue, Factor);\
							FromValue = InterpValue;\
						}\
						break;
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
				}
				EnumAddFlags(FromEntry->Flags, EEntryFlags::Written);
			}
			else
			{
				// We don't have this variable in our table. Let's add it.
				FCameraVariableDefinition NewVariableDefinition;
				NewVariableDefinition.VariableID = ToEntry.ID;
				NewVariableDefinition.VariableType = ToEntry.Type;
#if WITH_EDITORONLY_DATA
				NewVariableDefinition.VariableName = ToEntry.DebugName;
#endif
				AddVariable(NewVariableDefinition);

				FromEntry = Entries.Find(ToEntry.ID);
				check(FromEntry);

				uint32 ValueSize = 0, ValueAlignment = 0;
				GetVariableTypeAllocationInfo(FromEntry->Type, ValueSize, ValueAlignment);
				check(ValueSize != 0);

				uint8* FromValuePtr = Memory + FromEntry->Offset;
				const uint8* ToValuePtr = ToTable.Memory + ToEntry.Offset;
				FMemory::Memcpy(FromValuePtr, ToValuePtr, ValueSize);
				EnumAddFlags(FromEntry->Flags, EEntryFlags::Written);
			}

			if (OutMask)
			{
				OutMask->VariableIDs.Add(FromEntry->ID);
			}
		}
	}
}

}  // namespace UE::Cameras

