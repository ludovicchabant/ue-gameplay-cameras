// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraObjectInterfaceParameterDefinition.h"

#include "Core/CameraContextDataTableAllocationInfo.h"
#include "Core/CameraVariableTableAllocationInfo.h"

bool FCameraObjectInterfaceParameterDefinition::GetVariableDefinition(FCameraVariableDefinition& OutVariableDefinition) const
{
	if (ParameterType == ECameraObjectInterfaceParameterType::Blendable)
	{
		OutVariableDefinition.VariableID = VariableID;
		OutVariableDefinition.VariableType = VariableType;
		OutVariableDefinition.BlendableStructType = BlendableStructType;
	}
	return false;
}

bool FCameraObjectInterfaceParameterDefinition::GetContextDataDefinition(FCameraContextDataDefinition& OutDataDefinition) const
{
	if (ParameterType == ECameraObjectInterfaceParameterType::Data)
	{
		OutDataDefinition.DataID = DataID;
		OutDataDefinition.DataType = DataType;
		OutDataDefinition.DataTypeObject = DataTypeObject;
		OutDataDefinition.DataContainerType = DataContainerType;
	}
	return false;
}

