// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraVariableTableFwd.h"
#include "Core/CameraContextDataTableAllocationInfo.h"
#include "UObject/Interface.h"
#include "UObject/ObjectPtr.h"

#include "ICustomCameraNodeParameterProvider.generated.h"

class UCameraNode;

namespace UE::Cameras
{
	class FCameraNodeHierarchyBuilder;
	class FCameraObjectInterfaceBuilder;
	class FCameraObjectInterfaceParameterBuilder;
	namespace Internal { struct FInterfaceParameterBindingBuilder; }
}

/** Describes a custom camera blendable parameter. */
USTRUCT()
struct FCustomCameraNodeBlendableParameter
{
	GENERATED_BODY()

	/** The name of the parameter. */
	UPROPERTY()
	FName ParameterName;

	/** The type of the parameter. */
	UPROPERTY()
	ECameraVariableType ParameterType = ECameraVariableType::Boolean;

	/** The struct type of a blendable struct. */
	UPROPERTY()
	TObjectPtr<const UScriptStruct> BlendableStructType;

	/** An optional camera variable ID for dynamically driving the parameter's value. */
	UPROPERTY()
	FCameraVariableID OverrideVariableID;

	/** An optional user-defined camera variable for dynamically driving the parameter's value. */
	UPROPERTY()
	TObjectPtr<UCameraVariableAsset> OverrideVariable;

	bool operator==(const FCustomCameraNodeBlendableParameter& Other) const = default;
};

/** Describes a custom camera data parameter. */
USTRUCT()
struct FCustomCameraNodeDataParameter
{
	GENERATED_BODY()

	/** The name of the parameter. */
	UPROPERTY()
	FName ParameterName;

	/** The type of the parameter. */
	UPROPERTY()
	ECameraContextDataType ParameterType = ECameraContextDataType::Name;

	/** The type of the parameter container. */
	UPROPERTY()
	ECameraContextDataContainerType ParameterContainerType = ECameraContextDataContainerType::None;

	/** An extra type object for the parameter. */
	UPROPERTY()
	TObjectPtr<const UObject> ParameterTypeObject;

	/** An optional context data ID for dynamically driving the parameter's value. */
	UPROPERTY()
	FCameraContextDataID OverrideDataID;

	bool operator==(const FCustomCameraNodeDataParameter& Other) const = default;
};

/** Describes custom camera parameters. */
USTRUCT()
struct FCustomCameraNodeParameters
{
	GENERATED_BODY()

	/** The list of blendable parameters. */
	UPROPERTY()
	TArray<FCustomCameraNodeBlendableParameter> BlendableParameters;

	/** The list of data parameters. */
	UPROPERTY()
	TArray<FCustomCameraNodeDataParameter> DataParameters;

	/** Returns whether this struct has any blendable or data parameter. */
	bool HasAnyParameters() const { return !BlendableParameters.IsEmpty() || !DataParameters.IsEmpty(); }

	/** Removes all parameters from this structure. */
	void Reset() { BlendableParameters.Reset(); DataParameters.Reset(); }

	bool operator==(const FCustomCameraNodeParameters& Other) const = default;
};

/**
 * A structure for providing custom camera rig parameter information.
 */
struct FCustomCameraNodeParameterInfos
{
	/** Returns whether there are any blendable or data parameters. */
	bool HasAnyParameters() const { return !BlendableParameters.IsEmpty() || !DataParameters.IsEmpty(); }

	/** 
	 * Declares a blendable parameter. 
	 * Pass a null pointer for OverrideVariableID if this parameter should not be overridable
	 * by a camera rig parameter.
	 */
	GAMEPLAYCAMERAS_API void AddBlendableParameter(
			FName ParameterName, 
			ECameraVariableType ParameterType, 
			const UScriptStruct* BlendableStructType,
			const uint8* DefaultValue,
			FCameraVariableID* OverrideVariableID);

	GAMEPLAYCAMERAS_API void AddBlendableParameter(FCustomCameraNodeBlendableParameter& Parameter, const uint8* DefaultValue);

	/** 
	 * Declares a data parameter.
	 * Pass a null pointer for OverrideDataID if this parameter should not be overridable
	 * by a camera rig parameter.
	 */
	GAMEPLAYCAMERAS_API void AddDataParameter(
			FName ParameterName, 
			ECameraContextDataType ParameterType,
			ECameraContextDataContainerType ParameterContainerType,
			const UObject* ParameterTypeObject,
			const uint8* DefaultValue,
			FCameraContextDataID* OverrideDataID);

	GAMEPLAYCAMERAS_API void AddDataParameter(FCustomCameraNodeDataParameter& Parameter, const uint8* DefaultValue);

	/** Gets the list of blendable parameters. */
	GAMEPLAYCAMERAS_API void GetBlendableParameters(TArray<FCustomCameraNodeBlendableParameter>& OutBlendableParameters) const;
	/** Gets the list of data parameters. */
	GAMEPLAYCAMERAS_API void GetDataParameters(TArray<FCustomCameraNodeDataParameter>& OutDataParameters) const;

	/** Finds a blendable parameter of the given name. */
	GAMEPLAYCAMERAS_API bool FindBlendableParameter(FName ParameterName, FCustomCameraNodeBlendableParameter& OutParameter) const;
	/** Finds a data parameter of the given name. */
	GAMEPLAYCAMERAS_API bool FindDataParameter(FName ParameterName, FCustomCameraNodeDataParameter& OutParameter) const;

private:

	struct FBlendableParameterInfo
	{
		FName ParameterName;
		ECameraVariableType ParameterType;
		const UScriptStruct* BlendableStructType = nullptr;
		const uint8* DefaultValue = nullptr;
		FCameraVariableID* OverrideVariableID = nullptr;
		UCameraVariableAsset* OverrideVariable = nullptr;
	};

	struct FDataParameterInfo
	{
		FName ParameterName;
		ECameraContextDataType ParameterType;
		ECameraContextDataContainerType ParameterContainerType;
		const UObject* ParameterTypeObject;
		const uint8* DefaultValue = nullptr;
		FCameraContextDataID* OverrideDataID = nullptr;
	};

	TArray<FBlendableParameterInfo> BlendableParameters;
	TArray<FDataParameterInfo> DataParameters;

	friend class UE::Cameras::FCameraNodeHierarchyBuilder;
	friend class UE::Cameras::FCameraObjectInterfaceBuilder;
	friend class UE::Cameras::FCameraObjectInterfaceParameterBuilder;
	friend struct UE::Cameras::Internal::FInterfaceParameterBindingBuilder;
};

UINTERFACE(MinimalAPI)
class UCustomCameraNodeParameterProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for camera nodes that want to expose a custom interface of
 * blendable parameters and data parameters.
 */
class ICustomCameraNodeParameterProvider
{
	GENERATED_BODY()

public:

	/** Gathers the custom parameters on this node. */
	virtual void GetCustomCameraNodeParameters(FCustomCameraNodeParameterInfos& OutParameterInfos) {}

	/** Utility function for sub-classes to broadcast when the custom parameters have changed. */
	GAMEPLAYCAMERAS_API void OnCustomCameraNodeParametersChanged(const UCameraNode* ThisAsCameraNode) const;
};

