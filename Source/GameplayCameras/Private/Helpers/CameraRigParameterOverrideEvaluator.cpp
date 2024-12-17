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

	FCameraParameterOverrideHelper Helper(*OutVariableTable, *OutContextDataTable);

	for (const FPropertyBagPropertyDesc& PropertyDesc : CameraRigParametersStruct->GetPropertyDescs())
	{
		if (!CameraRigReference.IsParameterOverriden(PropertyDesc.ID))
		{
			continue;
		}

		TValueOrError<FStructView, EPropertyBagResult> ParameterValueOrError = CameraRigParameters.GetValueStruct(PropertyDesc);
		Helper.ApplyParameterOverride(CameraRig, PropertyDesc.ID, ParameterValueOrError, bDrivenOverridesOnly);
	}
}

}  // namespace UE::Cameras

