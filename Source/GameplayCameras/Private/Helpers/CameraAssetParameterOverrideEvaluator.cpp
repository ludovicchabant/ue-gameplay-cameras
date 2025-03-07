// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/CameraAssetParameterOverrideEvaluator.h"

#include "Core/CameraAsset.h"
#include "Core/CameraAssetReference.h"
#include "Core/CameraObjectInterfaceParameterDefinition.h"
#include "Core/CameraRigAsset.h"
#include "Helpers/CameraObjectInterfaceParameterOverrideHelper.h"

namespace UE::Cameras
{

FCameraAssetParameterOverrideEvaluator::FCameraAssetParameterOverrideEvaluator(const FCameraAssetReference& InCameraReference)
	: CameraReference(InCameraReference)
{
}

void FCameraAssetParameterOverrideEvaluator::ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, bool bDrivenOverridesOnly)
{
	ApplyParameterOverrides(&OutVariableTable, nullptr, bDrivenOverridesOnly);
}

void FCameraAssetParameterOverrideEvaluator::ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, FCameraContextDataTable& OutContextDataTable, bool bDrivenOverridesOnly)
{
	ApplyParameterOverrides(&OutVariableTable, &OutContextDataTable, bDrivenOverridesOnly);
}

void FCameraAssetParameterOverrideEvaluator::ApplyParameterOverrides(FCameraVariableTable* OutVariableTable, FCameraContextDataTable* OutContextDataTable, bool bDrivenOverridesOnly)
{
	check(OutVariableTable);

	const UCameraAsset* CameraAsset = CameraReference.GetCameraAsset();
	const FInstancedPropertyBag& CameraParameters = CameraReference.GetParameters();
	const UPropertyBag* CameraParametersStruct = CameraParameters.GetPropertyBagStruct();
	if (!CameraAsset || !CameraParametersStruct)
	{
		return;
	}

	TConstArrayView<FCameraObjectInterfaceParameterDefinition> ParameterDefinitions = CameraAsset->GetParameterDefinitions();
	const TArray<TObjectPtr<const UCameraRigAsset>>& ParameterOwners = CameraAsset->ParameterOwners;
	ensure(ParameterDefinitions.Num() == ParameterOwners.Num());

	FCameraObjectInterfaceParameterOverrideHelper Helper(OutVariableTable, OutContextDataTable);

	for (int32 Index = 0, MaxIndex = FMath::Min(ParameterDefinitions.Num(), ParameterOwners.Num()); Index < MaxIndex; ++Index)
	{
		const FCameraObjectInterfaceParameterDefinition& Definition(ParameterDefinitions[Index]);

		if (!OutContextDataTable && Definition.ParameterType == ECameraObjectInterfaceParameterType::Data)
		{
			continue;
		}

		const bool bIsAnimated = CameraReference.IsParameterAnimated(Definition.ParameterGuid);
		if (!CameraReference.IsParameterOverridden(Definition.ParameterGuid) && !bIsAnimated)
		{
			continue;
		}

		const FPropertyBagPropertyDesc* PropertyDesc = CameraParametersStruct->FindPropertyDescByID(Definition.ParameterGuid);
		if (!ensure(PropertyDesc))
		{
			continue;
		}

		const UCameraRigAsset* CameraRig = ParameterOwners[Index];
		if (!ensure(CameraRig))
		{
			continue;
		}

		const bool bThisDrivenOnly = bDrivenOverridesOnly && !bIsAnimated;
		Helper.ApplyParameterOverride(CameraRig, Definition, CameraParameters, *PropertyDesc, bThisDrivenOnly);
	}
}

}  // namespace UE::Cameras

