// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/CameraAssetParameterOverrideEvaluator.h"

#include "Core/CameraAsset.h"
#include "Core/CameraAssetReference.h"
#include "Core/CameraRigAsset.h"
#include "Helpers/CameraParameterOverrideHelper.h"

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
	using namespace Internal;

	check(OutVariableTable);

	const UCameraAsset* CameraAsset = CameraReference.GetCameraAsset();
	const FInstancedPropertyBag& CameraParameters = CameraReference.GetParameters();
	const UPropertyBag* CameraParametersStruct = CameraParameters.GetPropertyBagStruct();
	if (!CameraParametersStruct)
	{
		return;
	}

	TConstArrayView<FCameraRigParameterDefinition> ParameterDefinitions = CameraAsset->GetParameterDefinitions();
	const TArray<TObjectPtr<const UCameraRigAsset>>& ParameterOwners = CameraAsset->ParameterOwners;
	ensure(ParameterDefinitions.Num() == ParameterOwners.Num());

	FCameraParameterOverrideHelper Helper(*OutVariableTable, *OutContextDataTable);

	for (int32 Index = 0, MaxIndex = FMath::Min(ParameterDefinitions.Num(), ParameterOwners.Num()); Index < MaxIndex; ++Index)
	{
		const FCameraRigParameterDefinition& Definition(ParameterDefinitions[Index]);
		if (!CameraReference.IsParameterOverridden(Definition.ParameterGuid))
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

		Helper.ApplyParameterOverride(CameraRig, Definition, CameraParameters, *PropertyDesc, bDrivenOverridesOnly);
	}
}

}  // namespace UE::Cameras

