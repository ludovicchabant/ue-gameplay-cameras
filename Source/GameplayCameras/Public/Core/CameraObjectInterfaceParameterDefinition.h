// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraContextDataTableFwd.h"
#include "Core/CameraVariableTableFwd.h"
#include "Misc/TVariant.h"

#include "CameraObjectInterfaceParameterDefinition.generated.h"

class UBaseCameraObject;
struct FInstancedPropertyBag;
struct FPropertyBagPropertyDesc;

/**
 * The type of a camera rig parameter.
 */
UENUM()
enum class ECameraObjectInterfaceParameterType : uint8
{
	Blendable,
	Data
};

/**
 * Information about a parameter exposed on a camera asset.
 */
USTRUCT()
struct FCameraObjectInterfaceParameterDefinition
{
	GENERATED_BODY()

	/** The name of the parameter. */
	UPROPERTY()
	FName ParameterName;

	/**
	 * The GUID of the parameter.
	 * This matches the GUID on the corresponding UCameraObjectInterfaceBlendableParameter
	 * or UCameraObjectInterfaceDataParameter object.
	 */
	UPROPERTY()
	FGuid ParameterGuid;

	/** The type of this parameter. */
	UPROPERTY()
	ECameraObjectInterfaceParameterType ParameterType = ECameraObjectInterfaceParameterType::Blendable;


	// Blendable parameter properties.
	// (only valid when ParameterType == Blendable)

	/** The ID of the variable that drives this blendable parameter. */
	UPROPERTY()
	FCameraVariableID VariableID;

	/** The type of the variable that drives this blendable parameter. */
	UPROPERTY()
	ECameraVariableType VariableType = ECameraVariableType::Boolean;

	/** The type of the structure if VariableType is BlendableStruct. */
	UPROPERTY()
	TObjectPtr<const UScriptStruct> BlendableStructType;


	// Data parameter properties.
	// (only valid when ParameterType == Data)

	/** The ID of the data that drives this blendable parameter. */
	UPROPERTY()
	FCameraContextDataID DataID;

	/** The type of the data that drives this blendable parameter. */
	UPROPERTY()
	ECameraContextDataType DataType = ECameraContextDataType::Name;

	/** The type of container that drives this blendable parameter. */
	UPROPERTY()
	ECameraContextDataContainerType DataContainerType = ECameraContextDataContainerType::None;

	/** The type object of the data that drives this blendable parameter. */
	UPROPERTY()
	TObjectPtr<const UObject> DataTypeObject;

public:

	bool operator==(const FCameraObjectInterfaceParameterDefinition& Other) const = default;
};

template<>
struct TStructOpsTypeTraits<FCameraObjectInterfaceParameterDefinition> : public TStructOpsTypeTraitsBase2<FCameraObjectInterfaceParameterDefinition>
{
	enum
	{
		WithIdenticalViaEquality = true
	};
};

