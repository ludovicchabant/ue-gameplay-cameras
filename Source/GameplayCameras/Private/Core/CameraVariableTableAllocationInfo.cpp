// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraVariableTableAllocationInfo.h"

void FCameraVariableTableAllocationInfo::Combine(const FCameraVariableTableAllocationInfo& OtherInfo)
{
	TMap<FCameraVariableID, int32> KnownIDs;
	for (auto It = VariableDefinitions.CreateConstIterator(); It; ++It)
	{
		const FCameraVariableDefinition& VariableDefinition(*It);
		KnownIDs.Add(VariableDefinition.VariableID, It.GetIndex());
	}

	for (const FCameraVariableDefinition& OtherVariableDefinition : OtherInfo.VariableDefinitions)
	{
		const int32 KnownIndex = KnownIDs.FindRef(OtherVariableDefinition.VariableID, INDEX_NONE);
		if (KnownIndex == INDEX_NONE)
		{
			VariableDefinitions.Add(OtherVariableDefinition);
		}
		else
		{
			const FCameraVariableDefinition& KnownVariableDefinition(VariableDefinitions[KnownIndex]);
			ensure(KnownVariableDefinition == OtherVariableDefinition);
		}
	}
}

