// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/CameraRigParameterOverrideEvaluator.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetReference.h"
#include "Helpers/CameraParameterOverrideHelper.h"

namespace UE::Cameras
{

FCameraRigParameterOverrideEvaluator::FCameraRigParameterOverrideEvaluator(const FCameraRigAssetReference& InCameraRigReference)
	: CameraRigReference(InCameraRigReference)
{
}

void FCameraRigParameterOverrideEvaluator::ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, bool bDrivenOverridesOnly)
{
	ApplyParameterOverrides(&OutVariableTable, nullptr, bDrivenOverridesOnly);
}

void FCameraRigParameterOverrideEvaluator::ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, FCameraContextDataTable& OutContextDataTable, bool bDrivenOverridesOnly)
{
	ApplyParameterOverrides(&OutVariableTable, &OutContextDataTable, bDrivenOverridesOnly);
}

void FCameraRigParameterOverrideEvaluator::ApplyParameterOverrides(FCameraVariableTable* OutVariableTable, FCameraContextDataTable* OutContextDataTable, bool bDrivenOverridesOnly)
{
	using namespace Internal;

	check(OutVariableTable);

	const UCameraRigAsset* CameraRig = CameraRigReference.GetCameraRig();
	const FInstancedPropertyBag& CameraRigParameters = CameraRigReference.GetParameters();
	const UPropertyBag* CameraRigParametersStruct = CameraRigParameters.GetPropertyBagStruct();
	if (!CameraRig || !CameraRigParametersStruct)
	{
		return;
	}

	TConstArrayView<FCameraRigParameterDefinition> ParameterDefinitions = CameraRig->GetParameterDefinitions();

	FCameraParameterOverrideHelper Helper(OutVariableTable, OutContextDataTable);

	for (const FCameraRigParameterDefinition& Definition : ParameterDefinitions)
	{
		if (!OutContextDataTable && Definition.ParameterType == ECameraRigInterfaceParameterType::Data)
		{
			continue;
		}

		const bool bIsAnimated = CameraRigReference.IsParameterAnimated(Definition.ParameterGuid);
		if (!CameraRigReference.IsParameterOverridden(Definition.ParameterGuid) && !bIsAnimated)
		{
			continue;
		}

		const FPropertyBagPropertyDesc* PropertyDesc = CameraRigParametersStruct->FindPropertyDescByID(Definition.ParameterGuid);
		if (!ensure(PropertyDesc))
		{
			continue;
		}

		const bool bThisDrivenOnly = bDrivenOverridesOnly && !bIsAnimated;
		Helper.ApplyParameterOverride(CameraRig, Definition, CameraRigParameters, *PropertyDesc, bThisDrivenOnly);
	}
}

}  // namespace UE::Cameras

