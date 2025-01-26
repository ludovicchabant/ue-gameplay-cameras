// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraContextDataTableAllocationInfo.h"

void FCameraContextDataTableAllocationInfo::Combine(const FCameraContextDataTableAllocationInfo& OtherInfo)
{
	TMap<FCameraContextDataID, int32> KnownNames;
	for (auto It = DataDefinitions.CreateConstIterator(); It; ++It)
	{
		const FCameraContextDataDefinition& DataDefinition(*It);
		KnownNames.Add(DataDefinition.DataID, It.GetIndex());
	}

	for (const FCameraContextDataDefinition& OtherDataDefinition : OtherInfo.DataDefinitions)
	{
		const int32 KnownIndex = KnownNames.FindRef(OtherDataDefinition.DataID, INDEX_NONE);
		if (KnownIndex == INDEX_NONE)
		{
			DataDefinitions.Add(OtherDataDefinition);
		}
		else
		{
			const FCameraContextDataDefinition& KnownDataDefinition(DataDefinitions[KnownIndex]);
			ensure(KnownDataDefinition == OtherDataDefinition);
		}
	}
}

