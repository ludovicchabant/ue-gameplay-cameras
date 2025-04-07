// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StructUtils/PropertyBag.h"

class UBaseCameraObject;
struct FCameraObjectInterfaceParameterDefinition;

namespace UE::Cameras
{

class FCameraContextDataTable;
class FCameraVariableTable;

/**
 * A helper class for applying camera object interface parameter overrides from a property bag, 
 * such as with camera asset references, camera rig asset references, and camera shake asset
 * references.
 */
struct FCameraObjectInterfaceParameterOverrideHelper
{
public:

	/** Sets default values of blendable interface parameters in the given variable table. */
	static void ApplyDefaultBlendableParameters(const UBaseCameraObject* CameraObject, FCameraVariableTable& OutVariableTable);

	/** Sets default values of interface parameters in the given variable and context data tables. */
	static void ApplyDefaultParameters(const UBaseCameraObject* CameraObject, FCameraVariableTable& OutVariableTable, FCameraContextDataTable& OutContextDataTable);

private:

	static void ApplyDefaultParametersImpl(const UBaseCameraObject* CameraObject, FCameraVariableTable* OutVariableTable, FCameraContextDataTable* OutContextDataTable);

public:

	/** 
	 * Creates a new helper instance.
	 *
	 * The given variable or context data tables can be null, in which case blendable or data
	 * interface parameters will be skipped.
	 */
	FCameraObjectInterfaceParameterOverrideHelper(FCameraVariableTable* OutVariableTable, FCameraContextDataTable* OutContextDataTable);

	/** Sets overriden values of interface parameters in the given variable and context data tables. */
	void ApplyParameterOverrides(
			const UBaseCameraObject* CameraObject,
			const FInstancedPropertyBag& ParameterOverrides,
			const TSet<FGuid>& OverridenParameterGuids,
			const TSet<FGuid>& AnimatedParameterGuids,
			FCameraVariableTable* OutVariableTable,
			FCameraContextDataTable* OutContextDataTable,
			bool bDrivenOverridesOnly);

	/** Sets a given override value for an interface parameter. */
	void ApplyParameterOverride(
			const UBaseCameraObject* CameraObject,
			const FCameraObjectInterfaceParameterDefinition& ParameterDefinition,
			const FInstancedPropertyBag& PropertyBag,
			const FPropertyBagPropertyDesc& PropertyBagPropertyDesc,
			bool bDrivenOverridesOnly);

private:

	FCameraVariableTable* VariableTable;
	FCameraContextDataTable* ContextDataTable;
};

}  // namespace UE::Cameras

