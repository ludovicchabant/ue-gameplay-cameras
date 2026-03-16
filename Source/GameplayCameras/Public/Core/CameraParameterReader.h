// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraVariableTable.h"
#include "CoreTypes.h"
#include "Templates/UnrealTypeTraits.h"

namespace UE::Cameras
{

namespace Internal
{

// Shared utility function for retrieving the value of a variable from a variable table, handling the case of having
// to convert from one type to another, such as when accessing a double-precision value but returning a single-precision
// conversion of it.
template<typename ValueType>
ValueType GetVariableTableValue(typename TCallTraits<ValueType>::ParamType DefaultValue, FCameraVariableID VariableID, const FCameraVariableTable& VariableTable);

}  // namespace Internal

/**
 * A utility class for reading the effective value of a camera parameter.
 */
template<typename ValueType>
class TCameraParameterReader
{
public:

	TCameraParameterReader() {}

	template<typename ParameterType>
	TCameraParameterReader(const ParameterType& Parameter)
	{
		Initialize(Parameter);
	}

	/**
	 * Initializes the reader around the given parameter.
	 */
	template<typename ParameterType>
	void Initialize(const ParameterType& Parameter)
	{
		static_assert(
				std::is_same<ValueType, typename ParameterType::ValueType>(),
				"The given parameter is of the wrong type for this reader! Value types must be the same.");

		DefaultValuePtr = &Parameter.Value;
		VariableID = Parameter.VariableID;

		ensureMsgf(DefaultValuePtr, TEXT("The given parameter doesn't have a value!"));
	}

	/**
	 * Gets the actual value for the parameter.
	 */
	ValueType Get(const FCameraVariableTable& VariableTable) const
	{
		checkf(DefaultValuePtr, TEXT("Parameter reader has no value pointer!"));
		return Internal::GetVariableTableValue<ValueType>(*DefaultValuePtr, VariableID, VariableTable);
	}

	/**
	 * Returns whether the parameter is driven by a variable.
	 */
	bool IsDriven() const
	{
		return VariableID.IsValid();
	}

private:

	/** Pointer to the value in the parameter. */
	const ValueType* DefaultValuePtr = nullptr;
	/** The ID of the variable driving the parameter, if any. */
	FCameraVariableID VariableID;
};

namespace Internal
{

template<typename ValueType>
ValueType GetVariableTableValue(typename TCallTraits<ValueType>::ParamType DefaultValue, FCameraVariableID VariableID, const FCameraVariableTable& VariableTable)
{
	if (!VariableID.IsValid())
	{
		// No variable is driving the parameter, just return the parameter value.
		return DefaultValue;
	}
	else
	{
		// The parameter is driven by a variable. Find it in the variable table.
		// Some types may have to be converted, such as a float parameter reader accessing a double value.
		FCameraVariableEntry Entry;
		if (VariableTable.FindEntry(VariableID, Entry))
		{
			if (Entry.IsA<ValueType>())
			{
				return *reinterpret_cast<const ValueType*>(Entry.RawValuePtr);
			}

			ValueType ReturnValue;
			const bool bConverted = TCameraVariableTraits<ValueType>::ConvertFrom(Entry.Type, Entry.RawValuePtr, ReturnValue);
			if (ensureMsgf(bConverted, TEXT("Found a valid entry for the requested variable, but couldn't convert it to the desired output type")))
			{
				return ReturnValue;
			}
		}

		return DefaultValue;
	}
}

}  // namespace Interal

}  // namespace UE::Cameras

