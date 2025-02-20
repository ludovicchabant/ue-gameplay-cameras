// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StructUtils/PropertyBag.h"

class UCameraRigAsset;
struct FCameraRigParameterDefinition;

namespace UE::Cameras
{

class FCameraContextDataTable;
class FCameraVariableTable;

/**
 * A helper class for applying camera rig parameter overrides from a property bag, such as with
 * camera asset references and camera rig asset references.
 */
struct FCameraParameterOverrideHelper
{
	FCameraParameterOverrideHelper(FCameraVariableTable* OutVariableTable, FCameraContextDataTable* OutContextDataTable);

	void ApplyParameterOverride(
			const UCameraRigAsset* CameraRig,
			const FCameraRigParameterDefinition& ParameterDefinition,
			const FInstancedPropertyBag& PropertyBag,
			const FPropertyBagPropertyDesc& PropertyBagPropertyDesc,
			bool bDrivenOverridesOnly);

	static void ApplyDefaultBlendableParameters(const UCameraRigAsset* CameraRig, FCameraVariableTable& OutVariableTable);

private:

	FCameraVariableTable* VariableTable;
	FCameraContextDataTable* ContextDataTable;
};

}  // namespace UE::Cameras

