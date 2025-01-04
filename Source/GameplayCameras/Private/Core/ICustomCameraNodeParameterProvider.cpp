// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/ICustomCameraNodeParameterProvider.h"

#include "GameplayCamerasDelegates.h"

bool operator==(const FCustomCameraNodeBlendableParameter& A, const FCustomCameraNodeBlendableParameter& B)
{
	return A.ParameterName == B.ParameterName &&
		A.ParameterType == B.ParameterType &&
		A.OverrideVariable == B.OverrideVariable;
}

bool operator==(const FCustomCameraNodeDataParameter& A, const FCustomCameraNodeDataParameter& B)
{
	return A.ParameterName == B.ParameterName &&
		A.ParameterType == B.ParameterType &&
		A.ParameterTypeObject == B.ParameterTypeObject &&
		A.OverrideDataID == B.OverrideDataID;
}

bool operator==(const FCustomCameraNodeParameters& A, const FCustomCameraNodeParameters& B)
{
	return A.BlendableParameters == B.BlendableParameters &&
		A.DataParameters == B.DataParameters;
}

void FCustomCameraNodeParameterInfos::AddBlendableParameter(
		FName ParameterName, 
		ECameraVariableType ParameterType, 
		const uint8* DefaultValuePtr,
		TObjectPtr<UCameraVariableAsset>* OverrideVariable)
{
	BlendableParameters.Add({ ParameterName, ParameterType, DefaultValuePtr, OverrideVariable });
}

void FCustomCameraNodeParameterInfos::AddDataParameter(
		FName ParameterName, 
		ECameraContextDataType ParameterType,
		const UObject* ParameterTypeObject,
		FCameraContextDataID* OverrideDataID)
{
	DataParameters.Add({ ParameterName, ParameterType, ParameterTypeObject, OverrideDataID });
}

void FCustomCameraNodeParameterInfos::GetBlendableParameters(TArray<FCustomCameraNodeBlendableParameter>& OutBlendableParameters) const
{
	for (const FBlendableParameterInfo& BlendableParameter : BlendableParameters)
	{
		FCustomCameraNodeBlendableParameter& OutParameter = OutBlendableParameters.Emplace_GetRef();
		OutParameter.ParameterName = BlendableParameter.ParameterName;
		OutParameter.ParameterType = BlendableParameter.ParameterType;
		if (BlendableParameter.OverrideVariable)
		{
			OutParameter.OverrideVariable = *BlendableParameter.OverrideVariable;
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
			OutParameter.OverrideVariable = nullptr;
			if (BlendableParameter.OverrideVariable)
			{
				OutParameter.OverrideVariable = *BlendableParameter.OverrideVariable;
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

