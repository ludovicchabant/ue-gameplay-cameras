// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigParameterOverrideEvaluator.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetReference.h"
#include "Core/CameraVariableTable.h"

namespace UE::Cameras
{

namespace Internal
{

template<typename ParameterOverrideType>
void ApplyParameterOverrides(
		const UCameraRigAsset* CameraRig, 
		TArrayView<const ParameterOverrideType> ParameterOverrides, 
		FCameraVariableTable& OutVariableTable,
		bool bDrivenOverridesOnly)
{
	for (const ParameterOverrideType& ParameterOverride : ParameterOverrides)
	{
		using ParameterType = decltype(ParameterOverrideType::Value);
		using ValueType = typename ParameterType::ValueType;

		if (!ParameterOverride.PrivateVariableGuid.IsValid())
		{
			// Ignore un-built parameter overrides in the editor since the user could have just added
			// an override while PIE is running. They need to hit the Build button for the override
			// to apply.
			// Outside of the editor, report this as an error.
#if !WITH_EDITOR
			UE_LOG(LogCameraSystem, Error, 
					TEXT("Invalid parameter override '%s' in camera rig '%s'. Was it built/cooked?"),
					*ParameterOverride.InterfaceParameterName,
					*GetPathNameSafe(CameraRig));
#endif
			continue;
		}

		FCameraVariableID InterfaceParameterID(FCameraVariableID::FromHashValue(GetTypeHash(ParameterOverride.PrivateVariableGuid)));

		if (ParameterOverride.Value.Variable != nullptr)
		{
			// The override is driven by a variable... read its value and set it as the value for the
			// prefab's variable. Basically, we forward the value from one variable to the next.
			FCameraVariableDefinition OverrideDefinition(ParameterOverride.Value.Variable->GetVariableDefinition());

			const ValueType OverrideValue = OutVariableTable.GetValue<ValueType>(
					OverrideDefinition.VariableID, ParameterOverride.Value.Variable->GetDefaultValue());
			OutVariableTable.SetValue<ValueType>(InterfaceParameterID, OverrideValue);
		}
		else if (!bDrivenOverridesOnly)
		{
			// The override is a fixed value. Just set that on the prefab's variable.
			OutVariableTable.SetValue<ValueType>(InterfaceParameterID, ParameterOverride.Value.Value);
		}
	}
}

}  // namespace Internal

FCameraRigParameterOverrideEvaluator::FCameraRigParameterOverrideEvaluator(const FCameraRigAssetReference& InCameraRigReference)
	: CameraRigReference(InCameraRigReference)
{
}

void FCameraRigParameterOverrideEvaluator::ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, bool bDrivenOverridesOnly)
{
	const UCameraRigAsset* CameraRig = CameraRigReference.GetCameraRig();
	const FCameraRigParameterOverrides& ParameterOverrides = CameraRigReference.GetParameterOverrides();

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	Internal::ApplyParameterOverrides(\
			CameraRig,\
			ParameterOverrides.Get##ValueName##Overrides(),\
			OutVariableTable,\
			bDrivenOverridesOnly);
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
}

}  // namespace UE::Cameras

