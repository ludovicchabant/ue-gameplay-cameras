// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/ICustomCameraNodeParameterProvider.h"

#include "GameplayCamerasDelegates.h"

void FCustomCameraNodeParameterInfos::AddBlendableParameter(
		FName ParameterName, 
		ECameraVariableType ParameterType, 
		const UScriptStruct* BlendableStructType,
		const uint8* DefaultValue,
		FCameraVariableID* OverrideVariableID)
{
	BlendableParameters.Add({ ParameterName, ParameterType, BlendableStructType, DefaultValue, OverrideVariableID });
}

void FCustomCameraNodeParameterInfos::AddBlendableParameter(FCustomCameraNodeBlendableParameter& Parameter, const uint8* DefaultValue)
{
	AddBlendableParameter(
			Parameter.ParameterName,
			Parameter.ParameterType,
			Parameter.BlendableStructType,
			DefaultValue,
			Parameter.OverrideVariable ? nullptr : &Parameter.OverrideVariableID);
}

void FCustomCameraNodeParameterInfos::AddDataParameter(
		FName ParameterName, 
		ECameraContextDataType ParameterType,
		ECameraContextDataContainerType ParameterContainerType,
		const UObject* ParameterTypeObject,
		const uint8* DefaultValue,
		FCameraContextDataID* OverrideDataID)
{
	DataParameters.Add({ ParameterName, ParameterType, ParameterContainerType, ParameterTypeObject, DefaultValue, OverrideDataID });
}

void FCustomCameraNodeParameterInfos::AddDataParameter(FCustomCameraNodeDataParameter& Parameter, const uint8* DefaultValue)
{
	AddDataParameter(
			Parameter.ParameterName,
			Parameter.ParameterType,
			Parameter.ParameterContainerType,
			Parameter.ParameterTypeObject,
			DefaultValue,
			&Parameter.OverrideDataID);
}

void FCustomCameraNodeParameterInfos::GetBlendableParameters(TArray<FCustomCameraNodeBlendableParameter>& OutBlendableParameters) const
{
	for (const FBlendableParameterInfo& BlendableParameter : BlendableParameters)
	{
		FCustomCameraNodeBlendableParameter& OutParameter = OutBlendableParameters.Emplace_GetRef();
		OutParameter.ParameterName = BlendableParameter.ParameterName;
		OutParameter.ParameterType = BlendableParameter.ParameterType;
		OutParameter.BlendableStructType = BlendableParameter.BlendableStructType;
		OutParameter.OverrideVariable = BlendableParameter.OverrideVariable;
		if (BlendableParameter.OverrideVariableID)
		{
			OutParameter.OverrideVariableID = *BlendableParameter.OverrideVariableID;
		}
	}
}

void FCustomCameraNodeParameterInfos::GetDataParameters(TArray<FCustomCameraNodeDataParameter>& OutDataParameters) const
{
	for (const FDataParameterInfo& DataParameter : DataParameters)
	{
		FCustomCameraNodeDataParameter& OutParameter = OutDataParameters.Emplace_GetRef();
		OutParameter.ParameterName = DataParameter.ParameterName;
		OutParameter.ParameterType = DataParameter.ParameterType;
		OutParameter.ParameterContainerType = DataParameter.ParameterContainerType;
		OutParameter.ParameterTypeObject = DataParameter.ParameterTypeObject;
		if (DataParameter.OverrideDataID)
		{
			OutParameter.OverrideDataID = *DataParameter.OverrideDataID;
		}
	}
}

bool FCustomCameraNodeParameterInfos::FindBlendableParameter(FName ParameterName, FCustomCameraNodeBlendableParameter& OutParameter) const
{
	for (const FBlendableParameterInfo& BlendableParameter : BlendableParameters)
	{
		if (BlendableParameter.ParameterName == ParameterName)
		{
			OutParameter.ParameterName = BlendableParameter.ParameterName;
			OutParameter.ParameterType = BlendableParameter.ParameterType;
			OutParameter.BlendableStructType = BlendableParameter.BlendableStructType;
			if (BlendableParameter.OverrideVariableID)
			{
				OutParameter.OverrideVariableID = *BlendableParameter.OverrideVariableID;
			}
			return true;
		}
	}
	return false;
}

bool FCustomCameraNodeParameterInfos::FindDataParameter(FName ParameterName, FCustomCameraNodeDataParameter& OutParameter) const
{
	for (const FDataParameterInfo& DataParameter : DataParameters)
	{
		if (DataParameter.ParameterName == ParameterName)
		{
			OutParameter.ParameterName = DataParameter.ParameterName;
			OutParameter.ParameterType = DataParameter.ParameterType;
			OutParameter.ParameterContainerType = DataParameter.ParameterContainerType;
			OutParameter.ParameterTypeObject = DataParameter.ParameterTypeObject;
			if (DataParameter.OverrideDataID)
			{
				OutParameter.OverrideDataID = *DataParameter.OverrideDataID;
			}
			return true;
		}
	}
	return false;
}

void ICustomCameraNodeParameterProvider::OnCustomCameraNodeParametersChanged(const UCameraNode* ThisAsCameraNode) const
{
	using namespace UE::Cameras;

	FGameplayCamerasDelegates::OnCustomCameraNodeParametersChanged().Broadcast(ThisAsCameraNode);
}

