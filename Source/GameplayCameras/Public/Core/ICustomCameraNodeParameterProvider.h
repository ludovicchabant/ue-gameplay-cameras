// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraVariableTableFwd.h"
#include "Core/CameraContextDataAllocationInfo.h"
#include "UObject/Interface.h"
#include "UObject/ObjectPtr.h"

#include "ICustomCameraNodeParameterProvider.generated.h"

class UCameraVariableAsset;

namespace UE::Cameras
{
	class FCameraRigAssetBuilder;
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
	ECameraVariableType ParameterType;

	/** An optional camera variable that is dynamically driving the parameter's value. */
	UPROPERTY()
	TObjectPtr<UCameraVariableAsset> OverrideVariable;

	GAMEPLAYCAMERAS_API friend bool operator==(const FCustomCameraNodeBlendableParameter& A, const FCustomCameraNodeBlendableParameter& B);
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
	ECameraContextDataType ParameterType;

	/** An extra type object for the parameter. */
	UPROPERTY()
	TObjectPtr<const UObject> ParameterTypeObject;

	/** An optional context data ID for dynamically driving the parameter's value. */
	UPROPERTY()
	FCameraContextDataID OverrideDataID;

	GAMEPLAYCAMERAS_API friend bool operator==(const FCustomCameraNodeDataParameter& A, const FCustomCameraNodeDataParameter& B);
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

	GAMEPLAYCAMERAS_API friend bool operator==(const FCustomCameraNodeParameters& A, const FCustomCameraNodeParameters& B);
};

/**
 * A structure for providing custom camera rig parameter information.
 */
struct FCustomCameraNodeParameterInfos
{
	/** Returns whether there are any blendable or data parameters. */
	bool HasAnyParameters() const { return !BlendableParameters.IsEmpty() || !DataParameters.IsEmpty(); }

	/** Declares a blendable parameter. */
	template<typename VariableAssetType>
	void AddBlendableParameter(
			FName ParameterName, 
			ECameraVariableType ParameterType, 
			const uint8* DefaultValuePtr,
			TObjectPtr<VariableAssetType>* OverrideVariable)
	{
		FObjectPtr* OverrideVariablePtr = reinterpret_cast<FObjectPtr*>(OverrideVariable);
		BlendableParameters.Add({ 
				ParameterName, 
				ParameterType, 
				DefaultValuePtr,
				reinterpret_cast<TObjectPtr<UCameraVariableAsset>*>(OverrideVariable) });
	}

	/** Declares a blendable parameter. */
	GAMEPLAYCAMERAS_API void AddBlendableParameter(
			FName ParameterName, 
			ECameraVariableType ParameterType, 
			const uint8* DefaultValuePtr,
			TObjectPtr<UCameraVariableAsset>* OverrideVariable);

	/** Declares a data parameter. */
	GAMEPLAYCAMERAS_API void AddDataParameter(
			FName ParameterName, 
			ECameraContextDataType ParameterType,
			const UObject* ParameterTypeObject,
			FCameraContextDataID* OverrideDataID);

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
		const uint8* DefaultValuePtr = nullptr;
		TObjectPtr<UCameraVariableAsset>* OverrideVariable = nullptr;
	};

	struct FDataParameterInfo
	{
		FName ParameterName;
		ECameraContextDataType ParameterType;
		const UObject* ParameterTypeObject;
		FCameraContextDataID* OverrideDataID = nullptr;
	};

	TArray<FBlendableParameterInfo> BlendableParameters;
	TArray<FDataParameterInfo> DataParameters;

	friend class UE::Cameras::FCameraRigAssetBuilder;
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
};

