// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "StructUtils/PropertyBag.h"

#define UE_API GAMEPLAYCAMERAS_API

class UBaseCameraObject;
class UCameraObjectInterfaceDataParameter;

namespace UE::Cameras
{

/**
 * A helper class for building an FInstancedPropertyBag from a list of camera rig
 * parameter definitions.
 */
/** Builds the parameter definitions for the given camera object. */
/**
 * Builds the property bag that contains a property for each exposed parameter on the given camera object.
 * Each property's value is set to the default value of the corresponding parameter.
 */
class FCameraObjectInterfaceParameterBuilder
{
public:

	UE_API FCameraObjectInterfaceParameterBuilder();

	UE_API void BuildParameters(UBaseCameraObject* InCameraObject);

public:

	static UE_API void BuildDefaultParameters(const UBaseCameraObject* CameraObject, FInstancedPropertyBag& OutPropertyBag);
	static UE_API void AppendDefaultParameterProperties(const UBaseCameraObject* CameraObject, TArray<FPropertyBagPropertyDesc>& OutProperties);
	static UE_API void SetDefaultParameterValues(const UBaseCameraObject* CameraObject, FInstancedPropertyBag& PropertyBag);

private:

	static void SetDefaultParameterValue(const UCameraObjectInterfaceDataParameter* DataParameter, void* DestValuePtr, const void* SrcValuePtr);

	void BuildParametersImpl();

	void BuildParameterDefinitions();
	void BuildDefaultParameters();

private:

	UBaseCameraObject* CameraObject = nullptr;
};

}  // namespace UE::Cameras

#undef UE_API
