// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraContextDataAllocationInfo.h"

#include "Core/CameraNode.h"

void FCameraContextDataAllocationInfo::Add(const UCameraNode* Owner, FName DataName, ECameraContextDataType DataType, const UObject* DataTypeObject)
{
	FName FullDataName(FString::Format(TEXT("{0}_{1}"), { Owner->GetName(), *DataName.ToString() }));
	FCameraContextDataID DataID = FCameraContextDataID::FromName(FullDataName);

	FCameraContextDataDefinition NewDefinition;
	NewDefinition.DataID = DataID;
	NewDefinition.DataType = DataType;
	NewDefinition.DataTypeObject = DataTypeObject;
	DataDefinitions.Add(NewDefinition);
}

void FCameraContextDataAllocationInfo::Combine(const FCameraContextDataAllocationInfo& OtherInfo)
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

bool operator==(const FCameraContextDataDefinition& A, const FCameraContextDataDefinition& B)
{
	return A.DataID == B.DataID &&
		A.DataType == B.DataType &&
		A.DataTypeObject == B.DataTypeObject;
}

bool operator==(const FCameraContextDataAllocationInfo& A, const FCameraContextDataAllocationInfo& B)
{
	return A.DataDefinitions == B.DataDefinitions;
}

