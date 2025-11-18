// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BaseCameraObject.h"

#include "Build/CameraObjectInterfaceParameterBuilder.h"
#include "Misc/EngineVersionComparison.h"

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

void UBaseCameraObject::PostLoad()
{
	Super::PostLoad();

	if (!Guid.IsValid())
	{
		Guid = FGuid::NewGuid();
	}

#if UE_VERSION_OLDER_THAN(5,8,0)
	using namespace UE::Cameras;
	FCameraObjectInterfaceParameterBuilder::FixUpDefaultParameterProperties(ParameterDefinitions, DefaultParameters);
#endif
}

void UBaseCameraObject::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject | RF_NeedLoad | RF_WasLoaded) && 
			!Guid.IsValid())
	{
		Guid = FGuid::NewGuid();
	}
}

void UBaseCameraObject::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

	if (DuplicateMode == EDuplicateMode::Normal)
	{
		Guid = FGuid::NewGuid();
	}
}

