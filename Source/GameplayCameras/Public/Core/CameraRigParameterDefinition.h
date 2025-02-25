// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraContextDataTableFwd.h"
#include "Core/CameraVariableTableFwd.h"
#include "Misc/TVariant.h"

#include "CameraRigParameterDefinition.generated.h"

class UCameraRigAsset;
class UCameraRigDataParameter;
struct FInstancedPropertyBag;
struct FPropertyBagPropertyDesc;

/**
 * The type of a camera rig parameter.
 */
UENUM()
enum class ECameraRigInterfaceParameterType : uint8
{
	Blendable,
	Data
};

/**
 * Information about a parameter exposed on a camera asset.
 */
USTRUCT()
struct FCameraRigParameterDefinition
{
	GENERATED_BODY()

	/** The name of the parameter. */
	UPROPERTY()
	FName ParameterName;

	/**
	 * The GUID of the parameter.
	 * This matches the GUID on the corresponding UCameraRigBlendableParameter or
	 * UCameraRigDataParameter object.
	 */
	UPROPERTY()
	FGuid ParameterGuid;

	/** The type of this parameter. */
	UPROPERTY()
	ECameraRigInterfaceParameterType ParameterType = ECameraRigInterfaceParameterType::Blendable;


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

	bool operator==(const FCameraRigParameterDefinition& Other) const = default;
};

template<>
struct TStructOpsTypeTraits<FCameraRigParameterDefinition> : public TStructOpsTypeTraitsBase2<FCameraRigParameterDefinition>
{
	enum
	{
		WithIdenticalViaEquality = true
	};
};

namespace UE::Cameras
{

/**
 * A helper class for building an FInstancedPropertyBag from a list of camera rig
 * parameter definitions.
 */
class GAMEPLAYCAMERAS_API FCameraRigParameterBuilder
{
public:

	/**
	 * Builds a property bag that contains a property for each exposed parameter on the given camera rig.
	 * Each property's value is set to the default value of the corresponding parameter.
	 */
	static void BuildDefaultParameters(const UCameraRigAsset* CameraRig, FInstancedPropertyBag& OutPropertyBag);

public:

	/** Generates property bag property descriptors for the parameters exposed by the given camera rig. */
	static void AppendDefaultParameterProperties(const UCameraRigAsset* CameraRig, TArray<FPropertyBagPropertyDesc>& OutProperties);
	/** Sets the default value of the property bag properties that correspond to parameters on the given camera rig. */
	static void SetDefaultParameterValues(const UCameraRigAsset* CameraRig, FInstancedPropertyBag& PropertyBag);

private:

	static void SetDefaultParameterValue(const UCameraRigDataParameter* DataParameter, void* DestValuePtr, const void* SrcValuePtr);
};

}  // namespace UE::Cameras

