// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BaseCameraObject.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseCameraObject)

void FCameraObjectAllocationInfo::Append(const FCameraObjectAllocationInfo& OtherAllocationInfo)
{
	const FCameraNodeEvaluatorAllocationInfo& OtherEvaluatorInfo(OtherAllocationInfo.EvaluatorInfo);
	EvaluatorInfo.MaxAlignof = FMath::Max(EvaluatorInfo.MaxAlignof, OtherEvaluatorInfo.MaxAlignof);
	EvaluatorInfo.TotalSizeof = Align(EvaluatorInfo.TotalSizeof, OtherEvaluatorInfo.MaxAlignof) + OtherEvaluatorInfo.TotalSizeof;

	const FCameraVariableTableAllocationInfo& OtherVariableTableInfo(OtherAllocationInfo.VariableTableInfo);
	VariableTableInfo.VariableDefinitions.Append(OtherVariableTableInfo.VariableDefinitions);

	const FCameraContextDataTableAllocationInfo& OtherContextDataTableInfo(OtherAllocationInfo.ContextDataTableInfo);
	ContextDataTableInfo.DataDefinitions.Append(OtherContextDataTableInfo.DataDefinitions);
}

