// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "StructUtils/PropertyBag.h"

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
class GAMEPLAYCAMERAS_API FCameraObjectInterfaceParameterBuilder
{
public:

	FCameraObjectInterfaceParameterBuilder();

	void BuildParameters(UBaseCameraObject* InCameraObject);

public:

	static void BuildDefaultParameters(const UBaseCameraObject* CameraObject, FInstancedPropertyBag& OutPropertyBag);
	static void AppendDefaultParameterProperties(const UBaseCameraObject* CameraObject, TArray<FPropertyBagPropertyDesc>& OutProperties);
	static void SetDefaultParameterValues(const UBaseCameraObject* CameraObject, FInstancedPropertyBag& PropertyBag);

private:

	static void SetDefaultParameterValue(const UCameraObjectInterfaceDataParameter* DataParameter, void* DestValuePtr, const void* SrcValuePtr);

	void BuildParametersImpl();

	void BuildParameterDefinitions();
	void BuildDefaultParameters();

private:

	UBaseCameraObject* CameraObject = nullptr;
};

}  // namespace UE::Cameras

