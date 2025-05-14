// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraContextDataTableFwd.h"
#include "Core/CameraVariableTableFwd.h"
#include "StructUtils/PropertyBag.h"

#include "BaseCameraObjectReference.generated.h"

class UBaseCameraObject;
struct FCustomCameraNodeParameterInfos;

namespace UE::Cameras
{
	struct FCameraNodeEvaluationResult;
}

/**
 * Metadata for a referenced camera object's interface parameters.
 */
USTRUCT()
struct FCameraObjectInterfaceParameterMetaData
{
	GENERATED_BODY()

	/** The GUID of the parameter. */
	UPROPERTY()
	FGuid ParameterGuid;

	/** The ID to use for overriding a blendable parameter. */
	UPROPERTY()
	FCameraVariableID OverrideVariableID;

	/** The ID to use for overriding a data parameter. */
	UPROPERTY()
	FCameraContextDataID OverrideDataID;

	/** Whether this parameter has an override value. */
	UPROPERTY()
	bool bIsOverridden = false;

	/** Whether this parameter is being animated. */
	UPROPERTY()
	bool bIsAnimated = false;
};

USTRUCT(BlueprintType)
struct FBaseCameraObjectReference
{
	GENERATED_BODY()

public:

	virtual ~FBaseCameraObjectReference() {}

	/** Gets the parameters for this camera rig, some of which containing overrides. */
	const FInstancedPropertyBag& GetParameters() const
	{
		return Parameters;
	}

	/** Gets the parameters for this camera rig, some of which containing overrides. */
	FInstancedPropertyBag& GetParameters()
	{
		return Parameters;
	}

public:

	/** Gets the camera object that this reference points to. */
	virtual const UBaseCameraObject* GetCameraObject() const { return nullptr; }

	/** Returns whether the override parameters structure needs to be rebuilt. */
	GAMEPLAYCAMERAS_API bool NeedsRebuildParameters() const;
	/** Rebuilds the override parameters structure, if needed. */
	GAMEPLAYCAMERAS_API bool RebuildParametersIfNeeded();
	/** Rebuilds the override parameters structure. */
	GAMEPLAYCAMERAS_API void RebuildParameters();

public:

	// Internal API.

	GAMEPLAYCAMERAS_API bool IsParameterOverridden(const FGuid& PropertyID) const;
	GAMEPLAYCAMERAS_API void SetParameterOverridden(const FGuid& PropertyID, bool bIsOverridden);

	GAMEPLAYCAMERAS_API bool IsParameterAnimated(const FGuid& PropertyID) const;
	GAMEPLAYCAMERAS_API void SetParameterAnimated(const FGuid& PropertyID, bool bIsAnimated);

	GAMEPLAYCAMERAS_API void GetCustomCameraNodeParameters(FCustomCameraNodeParameterInfos& OutParameterInfos);
	
	template<typename ContainerType>
	void GetOverriddenParameterGuids(ContainerType& OutOverriddenIDs) const;

	template<typename ContainerType>
	void GetAnimatedParameterGuids(ContainerType& OutAnimatedIDs) const;

private:

	const FCameraObjectInterfaceParameterMetaData* FindMetaData(const FGuid& PropertyID) const;
	FCameraObjectInterfaceParameterMetaData& FindOrAddMetaData(const FGuid& PropertyID);

protected:

	/** The camera rig's parameters. */
	UPROPERTY(EditAnywhere, Category="", meta=(FixedLayout))
	FInstancedPropertyBag Parameters;

	/** Metadata for the parameters. */
	UPROPERTY()
	TArray<FCameraObjectInterfaceParameterMetaData> ParameterMetaData;
};

template<typename ContainerType>
void FBaseCameraObjectReference::GetOverriddenParameterGuids(ContainerType& OutOverriddenIDs) const
{
	for (const FCameraObjectInterfaceParameterMetaData& MetaData : ParameterMetaData)
	{
		if (MetaData.bIsOverridden)
		{
			OutOverriddenIDs.Add(MetaData.ParameterGuid);
		}
	}
}

template<typename ContainerType>
void FBaseCameraObjectReference::GetAnimatedParameterGuids(ContainerType& OutAnimatedIDs) const
{
	for (const FCameraObjectInterfaceParameterMetaData& MetaData : ParameterMetaData)
	{
		if (MetaData.bIsAnimated)
		{
			OutAnimatedIDs.Add(MetaData.ParameterGuid);
		}
	}
}

