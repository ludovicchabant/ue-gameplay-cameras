// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraContextDataTableFwd.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UnrealNames.h"

#include "CameraContextDataAllocationInfo.generated.h"

class UCameraNode;

/**
 * Definition for one entry in a camera rig's context data registry.
 */
USTRUCT()
struct FCameraContextDataDefinition
{
	GENERATED_BODY()

	/** The ID of data. */
	UPROPERTY()
	FCameraContextDataID DataID;

	/** The type of the data. */
	UPROPERTY()
	ECameraContextDataType DataType;

	/** An extra type object for the data. */
	UPROPERTY()
	TObjectPtr<const UObject> DataTypeObject;

	GAMEPLAYCAMERAS_API friend bool operator==(const FCameraContextDataDefinition& A, const FCameraContextDataDefinition& B);
};

/**
 * Collection of context data entries for a camera rig.
 */
USTRUCT()
struct FCameraContextDataAllocationInfo
{
	GENERATED_BODY()

	/** The list of context data definitions. */
	UPROPERTY()
	TArray<FCameraContextDataDefinition> DataDefinitions;

	/** Adds a new context data definition. */
	GAMEPLAYCAMERAS_API void Add(const UCameraNode* Owner, FName DataName, ECameraContextDataType DataType, const UObject* DataTypeObject);

	/**Combines the given allocation info with this one. */
	GAMEPLAYCAMERAS_API void Combine(const FCameraContextDataAllocationInfo& OtherInfo);

	GAMEPLAYCAMERAS_API friend bool operator==(const FCameraContextDataAllocationInfo& A, const FCameraContextDataAllocationInfo& B);
};

