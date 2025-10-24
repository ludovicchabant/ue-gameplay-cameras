// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/ArrayView.h"
#include "Misc/Guid.h"

class UBaseCameraObject;
class UCameraAsset;
class UObject;
struct FCameraObjectInterfaceParameterDefinition;
struct FInstancedOverridablePropertyBag;
struct FInstancedPropertyBag;
struct FPropertyBagPropertyDesc;

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
			const UObject* CameraObject,
			TConstArrayView<FCameraObjectInterfaceParameterDefinition> ParameterDefinitions,
			const FInstancedOverridablePropertyBag& ParameterOverrides);

private:

	void ApplyParameterOverride(
			const UObject* CameraObject,
			const FCameraObjectInterfaceParameterDefinition& ParameterDefinition,
			const FInstancedOverridablePropertyBag& PropertyBag,
			const FPropertyBagPropertyDesc& PropertyBagPropertyDesc);

public:

	/** Only parameters flagged as "overriden" should be applied. */
	bool bOverridenOnly = true;
	/** Only parameters that are dynamicaly driven should be applied. */
	bool bDrivenOnly = false;
	/** If not empty, the list of parameters we should only apply. */
	TArray<FGuid> OnlyParameterGuids;

private:

	FCameraVariableTable* VariableTable;
	FCameraContextDataTable* ContextDataTable;
};

}  // namespace UE::Cameras

