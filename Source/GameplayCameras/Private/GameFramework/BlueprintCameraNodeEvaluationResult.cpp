// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/BlueprintCameraNodeEvaluationResult.h"

FBlueprintCameraNodeEvaluationResult::FBlueprintCameraNodeEvaluationResult()
{
}

FBlueprintCameraNodeEvaluationResult::FBlueprintCameraNodeEvaluationResult(FCameraNodeEvaluationResult* InResult)
	: Result(InResult)
{
}

FBlueprintCameraPose FBlueprintCameraNodeEvaluationResult::GetCameraPose() const
{
	if (Result)
	{
		return FBlueprintCameraPose::FromCameraPose(Result->CameraPose);
	}
	return FBlueprintCameraPose();
}

FBlueprintCameraVariableTable FBlueprintCameraNodeEvaluationResult::GetVariableTable() const
{
	return FBlueprintCameraVariableTable(Result ? &Result->VariableTable : nullptr);
}

FBlueprintCameraContextDataTable FBlueprintCameraNodeEvaluationResult::GetContextDataTable() const
{
	return FBlueprintCameraContextDataTable(Result ? &Result->ContextDataTable : nullptr);
}

void FBlueprintCameraNodeEvaluationResult::SetCameraPose(const FBlueprintCameraPose& CameraPose)
{
	if (Result)
	{
		CameraPose.ApplyTo(Result->CameraPose);
	}
}

FBlueprintCameraPose UBlueprintCameraNodeEvaluationResultFunctionLibrary::GetCameraPose(const FBlueprintCameraNodeEvaluationResult& Data)
{
	return Data.GetCameraPose();
}

FBlueprintCameraVariableTable UBlueprintCameraNodeEvaluationResultFunctionLibrary::GetVariableTable(const FBlueprintCameraNodeEvaluationResult& Data)
{
	return Data.GetVariableTable();
}

FBlueprintCameraContextDataTable UBlueprintCameraNodeEvaluationResultFunctionLibrary::GetContextDataTable(const FBlueprintCameraNodeEvaluationResult& Data)
{
	return Data.GetContextDataTable();
}

void UBlueprintCameraNodeEvaluationResultFunctionLibrary::SetCameraPose(FBlueprintCameraNodeEvaluationResult& Data, const FBlueprintCameraPose& CameraPose)
{
	Data.SetCameraPose(CameraPose);
}

