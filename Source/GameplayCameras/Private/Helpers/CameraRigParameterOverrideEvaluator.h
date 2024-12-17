// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

struct FCameraRigAssetReference;

namespace UE::Cameras
{

class FCameraContextDataTable;
class FCameraVariableTable;

/**
 * Utility class for applying interface parameter overrides to a camera rig via a
 * given variable table.
 */
class FCameraRigParameterOverrideEvaluator
{
public:

	/** Creates a new parameter override evaluator. */
	FCameraRigParameterOverrideEvaluator(const FCameraRigAssetReference& InCameraRigReference);

	/** 
	 * Applies override values to the given variable table.
	 *
	 * @param OutVariableTable      The variable table in which to set the override values.
	 * @param bDrivenOverridesOnly  Whether only overrides driven by variables should be applied.
	 */
	void ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, bool bDrivenOverridesOnly = false);

	/** 
	 * Applies override values to the given variable and context data tables.
	 *
	 * @param OutVariableTable      The variable table in which to set the override values.
	 * @param OutContextDataTable   The context data table in which to set the override values.
	 * @param bDrivenOverridesOnly  Whether only overrides driven by variables should be applied.
	 */
	void ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, FCameraContextDataTable& OutContextDataTable, bool bDrivenOverridesOnly = false);

private:

	void ApplyParameterOverrides(FCameraVariableTable* OutVariableTable, FCameraContextDataTable* OutContextDataTable, bool bDrivenOverridesOnly);

private:

	const FCameraRigAssetReference& CameraRigReference;
};

}  // namespace UE::Cameras

