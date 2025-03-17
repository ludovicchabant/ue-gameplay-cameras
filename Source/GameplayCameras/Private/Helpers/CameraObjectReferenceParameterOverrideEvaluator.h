// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BaseCameraObjectReference.h"
#include "Helpers/CameraObjectInterfaceParameterOverrideHelper.h"

namespace UE::Cameras
{

class FCameraContextDataTable;
class FCameraVariableTable;

/**
 * Utility class for applying interface parameter overrides to a camera object via a
 * given variable table and/or context data table.
 */
class FCameraObjectReferenceParameterOverrideEvaluator
{
public:

	/** Creates a new parameter override evaluator. */
	FCameraObjectReferenceParameterOverrideEvaluator(const FBaseCameraObjectReference& InObjectReference)
		: ObjectReference(InObjectReference)
	{}

	/** 
	 * Applies override values to the given variable table.
	 *
	 * @param OutVariableTable      The variable table in which to set the override values.
	 * @param bDrivenOverridesOnly  Whether only overrides driven by variables should be applied.
	 */
	void ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, bool bDrivenOverridesOnly = false)
	{
		TSet<FGuid> OverridenParameterGuids;
		ObjectReference.GetOverriddenParameterGuids(OverridenParameterGuids);

		TSet<FGuid> AnimatedParameterGuids;
		ObjectReference.GetAnimatedParameterGuids(AnimatedParameterGuids);

		FCameraObjectInterfaceParameterOverrideHelper Helper(&OutVariableTable, nullptr);
		Helper.ApplyParameterOverrides(
				ObjectReference.GetCameraObject(),
				ObjectReference.GetParameters(),
				OverridenParameterGuids,
				AnimatedParameterGuids,
				&OutVariableTable, 
				nullptr, 
				bDrivenOverridesOnly);
	}

	/** 
	 * Applies override values to the given variable and context data tables.
	 *
	 * @param OutVariableTable      The variable table in which to set the override values.
	 * @param OutContextDataTable   The context data table in which to set the override values.
	 * @param bDrivenOverridesOnly  Whether only overrides driven by variables should be applied.
	 */
	void ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, FCameraContextDataTable& OutContextDataTable, bool bDrivenOverridesOnly = false)
	{
		TSet<FGuid> OverridenParameterGuids;
		ObjectReference.GetOverriddenParameterGuids(OverridenParameterGuids);

		TSet<FGuid> AnimatedParameterGuids;
		ObjectReference.GetAnimatedParameterGuids(AnimatedParameterGuids);

		FCameraObjectInterfaceParameterOverrideHelper Helper(&OutVariableTable, &OutContextDataTable);
		Helper.ApplyParameterOverrides(
				ObjectReference.GetCameraObject(),
				ObjectReference.GetParameters(),
				OverridenParameterGuids,
				AnimatedParameterGuids,
				&OutVariableTable, 
				&OutContextDataTable, 
				bDrivenOverridesOnly);
	}

private:

	const FBaseCameraObjectReference& ObjectReference;
};

}  // namespace UE::Cameras

