// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/CameraParameterOverrideHelper.h"

#include "Core/CameraContextDataTable.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableTable.h"

namespace UE::Cameras
{

namespace Internal
{

template<typename ParameterType>
void ApplyBlendableParameterOverride(
		const UCameraRigAsset* CameraRig,
		const UCameraRigBlendableParameter* BlendableParameter,
		const ParameterType& ParameterValue,
		FCameraVariableTable& VariableTable,
		bool bDrivenOverridesOnly)
{
	using ValueType = typename ParameterType::ValueType;

	if (!ensure(BlendableParameter))
	{
		return;
	}

	if (!BlendableParameter->PrivateVariable)
	{
		// Ignore un-built parameter overrides in the editor since the user could have just added
		// an override while PIE is running. They need to hit the Build button for the override
		// to apply.
		// Outside of the editor, report this as an error.
#if !WITH_EDITOR
		UE_LOG(LogCameraSystem, Error,
				TEXT("Invalid blendable parameter override '%s' in camera rig '%s'. Was it built/cooked?"),
				*BlendableParameter->InterfaceParameterName,
				*GetPathNameSafe(CameraRig));
#endif
		return;
	}

	FCameraVariableID ParameterVariableID(BlendableParameter->PrivateVariable->GetVariableID());

	if (ParameterValue.Variable != nullptr)
	{
		// The override is driven by a variable... read its value and set it as the value for the
		// prefab's variable. Basically, we forward the value from one variable to the next.
		FCameraVariableDefinition OverrideDefinition(ParameterValue.Variable->GetVariableDefinition());

		const ValueType OverrideValue = VariableTable.GetValue<ValueType>(
				OverrideDefinition.VariableID, ParameterValue.Variable->GetDefaultValue());
		VariableTable.SetValue<ValueType>(ParameterVariableID, OverrideValue);
	}
	else if (!bDrivenOverridesOnly)
	{
		// The override is a fixed value. Just set that on the prefab's variable.
		VariableTable.SetValue<ValueType>(ParameterVariableID, ParameterValue.Value);
	}
}

void ApplyDataParameterOverride(
		const UCameraRigAsset* CameraRig,
		const UCameraRigDataParameter* DataParameter,
		const FStructView& ParameterValue,
		FCameraContextDataTable& ContextDataTable)
{
	if (!ensure(DataParameter))
	{
		return;
	}

	if (!DataParameter->PrivateDataID)
	{
#if !WITH_EDITOR
		UE_LOG(LogCameraSystem, Error,
				TEXT("Invalid data parameter override '%s' in camera rig '%s'. Was it built/cooked?"),
				*DataParameter->InterfaceParameterName,
				*GetPathNameSafe(CameraRig));
		return;
#endif
	}

	// Write the override value into the context data table.
	FCameraContextDataID ParameterDataID = DataParameter->PrivateDataID;
	ContextDataTable.SetStructViewData(ParameterDataID, ParameterValue);
}

}  // namespace Internal

FCameraParameterOverrideHelper::FCameraParameterOverrideHelper(FCameraVariableTable& OutVariableTable, FCameraContextDataTable& OutContextDataTable)
	: VariableTable(OutVariableTable)
	, ContextDataTable(OutContextDataTable)
{
}

void FCameraParameterOverrideHelper::ApplyParameterOverride(
		const UCameraRigAsset* CameraRig,
		const FGuid& ParameterGuid,
		TValueOrError<FStructView, EPropertyBagResult> ParameterValueOrError,
		bool bDrivenOverridesOnly)
{
	using namespace Internal;

	if (ensureMsgf(
				ParameterValueOrError.HasValue() && !ParameterValueOrError.HasError(),
				TEXT("Camera parameter has no valid value! Error: %s"),
				*UEnum::GetValueAsString(ParameterValueOrError.GetError())))
	{
		const FStructView& ParameterValue = ParameterValueOrError.GetValue();
		const UScriptStruct* ParameterType = ParameterValue.GetScriptStruct();

		// Check if this is a blendable parameter or a data parameter.
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		if (ParameterType == F##ValueName##CameraParameter::StaticStruct())\
		{\
			const UCameraRigBlendableParameter* BlendableParameter = CameraRig->Interface.FindBlendableParameterByGuid(ParameterGuid);\
			const F##ValueName##CameraParameter& TypedParameterValue = ParameterValue.Get<F##ValueName##CameraParameter>();\
			ApplyBlendableParameterOverride(CameraRig, BlendableParameter, TypedParameterValue, VariableTable, bDrivenOverridesOnly);\
		}\
		else
		UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		{
			const UCameraRigDataParameter* DataParameter = CameraRig->Interface.FindDataParameterByGuid(ParameterGuid);
			ApplyDataParameterOverride(CameraRig, DataParameter, ParameterValue, ContextDataTable);
		}
	}
}

}  // namespace UE::Cameras

