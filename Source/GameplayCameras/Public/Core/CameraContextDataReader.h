// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraContextDataTable.h"
#include "CoreTypes.h"

namespace UE::Cameras
{

template<typename DataType>
struct TCameraContextDataReader
{
	TCameraContextDataReader() {}

	template<typename ParameterType>
	TCameraContextDataReader(const ParameterType& Parameter)
	{
		Initialize(Parameter);
	}

	template<typename ParameterType>
	void Initialize(const ParameterType& Parameter)
	{
		static_assert(
				std::is_same<DataType, typename ParameterType::DataType>(),
				"The given parameter is of the wrong type for this reader! Data types must be the same.");

		DefaultValuePtr = &Parameter.Value;
		DataID = Parameter.DataID;

		ensureMsgf(DefaultValuePtr, TEXT("The given parameter doesn't have a value!"));
	}

	DataType Get(const FCameraContextDataTable& ContextDataTable) const
	{
		if (!DataID.IsValid())
		{
			return *DefaultValuePtr;
		}
		else
		{
			if (const DataType* ActualValue = ContextDataTable.TryGetData<DataType>(DataID))
			{
				return *ActualValue;
			}
			return *DefaultValuePtr;
		}
	}

private:

	const DataType* DefaultValuePtr = nullptr;
	FCameraContextDataID DataID;
};

}  // namespace UE::Cameras

