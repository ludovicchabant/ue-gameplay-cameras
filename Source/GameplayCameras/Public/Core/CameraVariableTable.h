// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "Containers/Set.h"
#include "Core/CameraVariableTableFwd.h"
#include "CoreTypes.h"
#include "GameplayCameras.h"
#include "Misc/EnumClassFlags.h"
#include "Templates/UnrealTypeTraits.h"
#include "UObject/NameTypes.h"

namespace UE::Cameras
{

template<typename ValueType>
struct TCameraVariableTraits;

template<typename ValueType>
struct TCameraVariableInterpolation;

/**
 * Filter for variable table operations.
 */
enum class ECameraVariableTableFilter
{
	None = 0,
	/** Only include input variables. */
	Input = 1 << 0,
	/** Only include output variables (i.e. anything not an input). */
	Output = 1 << 1,
	/** Only include changed variables. */
	ChangedOnly = 1 << 2,

	/** All variables. */
	All = Input | Output,
	/** All changed variables. */
	AllChanged = Input | Output | ChangedOnly
};
ENUM_CLASS_FLAGS(ECameraVariableTableFilter)

/**
 * A structure that keeps track of which variables have been processed in a
 * camera variable table.
 */
struct FCameraVariableTableFlags
{
	/** The list of processed variable IDs. */
	TSet<FCameraVariableID> VariableIDs;
};

/**
 * The camera variable table is a container for a collection of arbitrary values
 * of various types. Only certain basic types are supported (most primitive types).
 * 
 * This table serves both as an implementation of the usual "blackboard" design, where
 * gameplay systems can push any appropriate values into the camera system, and as a
 * place for camera node evaluators to stash various things.
 *
 * The main function of the variable table is that it is blended along with the camera
 * rig it belongs to. Any matching values between to blended tables with be themselves
 * blended, except for values flagged as "private".
 *
 * Internally, the variable table is allocated as one continuous block of memory, plus
 * a map of metadata keyed by variable ID. A variable ID can be anything, but will
 * generally be the hash of the variable name.
 */
class FCameraVariableTable
{
public:

	FCameraVariableTable();
	FCameraVariableTable(FCameraVariableTable&& Other);
	FCameraVariableTable& operator=(FCameraVariableTable&& Other);
	GAMEPLAYCAMERAS_API ~FCameraVariableTable();

	FCameraVariableTable(const FCameraVariableTable&) = delete;
	FCameraVariableTable& operator=(const FCameraVariableTable&) = delete;

	/** Initializes the variable table so that it fits the provided allocation info. */
	void Initialize(const FCameraVariableTableAllocationInfo& AllocationInfo);

	/** Adds a variable to the table.
	 *
	 * This may re-allocate the internal memory buffer. It's recommended to pre-compute
	 * the allocation information needed for a table, and initialize it once.
	 */
	GAMEPLAYCAMERAS_API void AddVariable(const FCameraVariableDefinition& VariableDefinition);

public:

	// Getter methods.

	template<typename ValueType>
	const ValueType* FindValue(FCameraVariableID VariableID) const;

	template<typename ValueType>
	const ValueType& GetValue(FCameraVariableID VariableID) const;

	template<typename ValueType>
	ValueType GetValue(FCameraVariableID VariableID, typename TCallTraits<ValueType>::ParamType DefaultValue) const;

	template<typename ValueType>
	bool TryGetValue(FCameraVariableID VariableID, ValueType& OutValue) const;

	bool ContainsValue(FCameraVariableID VariableID) const;

public:

	// Setter methods.
	
	template<typename ValueType>
	void SetValue(FCameraVariableID VariableID, typename TCallTraits<ValueType>::ParamType Value);

	template<typename ValueType>
	bool TrySetValue(FCameraVariableID VariableID, typename TCallTraits<ValueType>::ParamType Value);

	template<typename VariableAssetType>
	void SetValue(
			const VariableAssetType* VariableAsset, 
			typename TCallTraits<typename VariableAssetType::ValueType>::ParamType Value, 
			bool bCreateIfMissing = false);

public:

	// Interpolation.
	
	void OverrideAll(const FCameraVariableTable& OtherTable);
	void Override(const FCameraVariableTable& OtherTable, ECameraVariableTableFilter Filter);
	void Override(const FCameraVariableTable& OtherTable, ECameraVariableTableFilter Filter, const FCameraVariableTableFlags& InMask, bool bInvertMask, FCameraVariableTableFlags& OutMask);

	void LerpAll(const FCameraVariableTable& ToTable, float Factor);
	void Lerp(const FCameraVariableTable& ToTable, ECameraVariableTableFilter Filter, float Factor);
	void Lerp(const FCameraVariableTable& ToTable, ECameraVariableTableFilter Filter, float Factor, const FCameraVariableTableFlags& InMask, bool bInvertMask, FCameraVariableTableFlags& OutMask);

public:

	// Lower level API.

	void SetValue(FCameraVariableID VariableID, ECameraVariableType ExpectedVariableType, const uint8* InRawValuePtr);
	bool TrySetValue(FCameraVariableID VariableID, ECameraVariableType ExpectedVariableType, const uint8* InRawValuePtr);
	bool IsValueWritten(FCameraVariableID VariableID) const;
	void UnsetValue(FCameraVariableID VariableID);
	void UnsetAllValues();

	bool IsValueWrittenThisFrame(FCameraVariableID VariableID) const;
	void ClearAllWrittenThisFrameFlags();

	void Serialize(FArchive& Ar);

private:

	struct FEntry;

	static bool GetVariableTypeAllocationInfo(ECameraVariableType VariableType, uint32& OutSizeOf, uint32& OutAlignOf);

	template<typename ValueType>
	static bool CheckVariableType(ECameraVariableType InType)
	{
		return ensure(TCameraVariableTraits<ValueType>::Type == InType);
	}

	void ReallocateBuffer(uint32 MinRequired = 0);

	GAMEPLAYCAMERAS_API FEntry* FindEntry(FCameraVariableID VariableID);
	GAMEPLAYCAMERAS_API const FEntry* FindEntry(FCameraVariableID VariableID) const;

	void InternalOverride(const FCameraVariableTable& OtherTable, ECameraVariableTableFilter Filter, const FCameraVariableTableFlags* InMask, bool bInvertMask, FCameraVariableTableFlags* OutMask);
	void InternalLerp(const FCameraVariableTable& ToTable, ECameraVariableTableFilter Filter, float Factor, const FCameraVariableTableFlags* InMask, bool bInvertMask, FCameraVariableTableFlags* OutMask);

private:

	enum class EEntryFlags : uint8
	{
		None = 0,
		Private = 1 << 0,
		Input = 1 << 1,
		Written = 1 << 2,
		WrittenThisFrame = 1 << 3
	};
	FRIEND_ENUM_CLASS_FLAGS(EEntryFlags)

	struct FEntry
	{
		FCameraVariableID ID;
		ECameraVariableType Type;
		uint32 Offset;
		mutable EEntryFlags Flags;
#if WITH_EDITORONLY_DATA
		FString DebugName;
#endif
	};

	TArray<FEntry> Entries;
	TMap<FCameraVariableID, int32> EntryLookup;

	uint8* Memory = nullptr;
	uint32 Capacity = 0;
	uint32 Used = 0;

	template<typename T>
	friend struct TCameraVariableInterpolation;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	friend class FVariableTableDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

ENUM_CLASS_FLAGS(FCameraVariableTable::EEntryFlags)

template<typename ValueType>
const ValueType* FCameraVariableTable::FindValue(FCameraVariableID VariableID) const
{
	if (const FEntry* Entry = FindEntry(VariableID))
	{
		CheckVariableType<ValueType>(Entry->Type);
		if (EnumHasAnyFlags(Entry->Flags, EEntryFlags::Written))
		{
			return reinterpret_cast<ValueType*>(Memory + Entry->Offset);
		}
	}
	return nullptr;
}

template<typename ValueType>
const ValueType& FCameraVariableTable::GetValue(FCameraVariableID VariableID) const
{
	const FEntry* Entry = FindEntry(VariableID);
	if (ensureMsgf(Entry, TEXT("Can't get camera variable (ID '%d') because it doesn't exist in the table."), VariableID.GetValue()))
	{
		CheckVariableType<ValueType>(Entry->Type);
#if WITH_EDITORONLY_DATA
		checkf(
				EnumHasAnyFlags(Entry->Flags, EEntryFlags::Written),
				TEXT("Variable '%s' has never been written to. GetValue() will return uninitialized memory!"),
				*Entry->DebugName);
#else
		checkf(
				EnumHasAnyFlags(Entry->Flags, EEntryFlags::Written),
				TEXT("Variable '%s' has never been written to. GetValue() will return uninitialized memory!"),
				*LexToString(VariableID.GetValue()));
#endif
		return *reinterpret_cast<ValueType*>(Memory + Entry->Offset);
	}

	static ValueType DefaultValue = ValueType();
	return DefaultValue;
}

template<typename ValueType>
ValueType FCameraVariableTable::GetValue(FCameraVariableID VariableID, typename TCallTraits<ValueType>::ParamType DefaultValue) const
{
	if (const ValueType* Value = FindValue<ValueType>(VariableID))
	{
		return *Value;
	}
	return DefaultValue;
}

template<typename ValueType>
bool FCameraVariableTable::TryGetValue(FCameraVariableID VariableID, ValueType& OutValue) const
{
	if (const ValueType* Value = FindValue<ValueType>(VariableID))
	{
		OutValue = *Value;
		return true;
	}
	return false;
}

template<typename ValueType>
void FCameraVariableTable::SetValue(FCameraVariableID VariableID, typename TCallTraits<ValueType>::ParamType Value)
{
	FEntry* Entry = FindEntry(VariableID);
	if (ensureMsgf(Entry, TEXT("Can't set camera variable (ID '%d') because it doesn't exist in the table."), VariableID.GetValue()))
	{
		CheckVariableType<ValueType>(Entry->Type);
		ValueType* ValuePtr = reinterpret_cast<ValueType*>(Memory + Entry->Offset);
		*ValuePtr = Value;
		Entry->Flags |= EEntryFlags::Written | EEntryFlags::WrittenThisFrame;
	}
}

template<typename ValueType>
bool FCameraVariableTable::TrySetValue(FCameraVariableID VariableID, typename TCallTraits<ValueType>::ParamType Value)
{
	if (FEntry* Entry = FindEntry(VariableID))
	{
		CheckVariableType<ValueType>(Entry->Type);
		ValueType* ValuePtr = reinterpret_cast<ValueType*>(Memory + Entry->Offset);
		*ValuePtr = Value;
		Entry->Flags |= EEntryFlags::Written | EEntryFlags::WrittenThisFrame;
		return true;
	}
	return false;
}

template<typename VariableAssetType>
void FCameraVariableTable::SetValue(
		const VariableAssetType* VariableAsset, 
		typename TCallTraits<typename VariableAssetType::ValueType>::ParamType Value, 
		bool bCreateIfMissing)
{
	if (ensure(VariableAsset))
	{
		if (TrySetValue<typename VariableAssetType::ValueType>(VariableAsset->GetVariableID(), Value))
		{
			return;
		}

		if (bCreateIfMissing)
		{
			FCameraVariableDefinition VariableDefinition = VariableAsset->GetVariableDefinition();
			AddVariable(VariableDefinition);
			SetValue<typename VariableAssetType::ValueType>(VariableDefinition.VariableID, Value);
		}
	}
}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	template<>\
	struct TCameraVariableTraits<ValueType>\
	{\
		static const ECameraVariableType Type = ECameraVariableType::ValueName;\
	};
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

template<typename ValueType>
struct TCameraVariableInterpolation
{
	using ValueParam = typename TCallTraits<ValueType>::ParamType;
	static ValueType Interpolate(const FCameraVariableTable::FEntry& TableEntry, ValueParam From, ValueParam To, float Factor)
	{
		return FMath::LerpStable(From, To, Factor);
	}
};

template<typename T>
struct TCameraVariableInterpolation<UE::Math::TTransform<T>>
{
	using ValueType = UE::Math::TTransform<T>;
	using ValueParam = typename TCallTraits<ValueType>::ParamType;
	static ValueType Interpolate(const FCameraVariableTable::FEntry& TableEntry, ValueParam From, ValueParam To, float Factor)
	{
		ValueType Result(From);
		Result.BlendWith(To, Factor);
		return Result;
	}
};

}  // namespace UE::Cameras

