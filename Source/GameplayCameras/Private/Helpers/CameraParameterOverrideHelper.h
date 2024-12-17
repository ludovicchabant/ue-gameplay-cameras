// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StructUtils/PropertyBag.h"

class UCameraRigAsset;
class UCameraRigBlendableParameter;
class UCameraRigDataParameter;

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
	FCameraParameterOverrideHelper(FCameraVariableTable& OutVariableTable, FCameraContextDataTable& OutContextDataTable);

	void ApplyParameterOverride(
			const UCameraRigAsset* CameraRig,
			const FGuid& ParameterGuid,
			TValueOrError<FStructView, EPropertyBagResult> ParameterValueOrError,
			bool bDrivenOverridesOnly);

private:

	FCameraVariableTable& VariableTable;
	FCameraContextDataTable& ContextDataTable;
};

}  // namespace UE::Cameras

