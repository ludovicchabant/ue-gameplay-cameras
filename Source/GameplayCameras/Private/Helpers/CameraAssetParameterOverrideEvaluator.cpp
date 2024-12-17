// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/CameraAssetParameterOverrideEvaluator.h"

#include "Core/CameraAsset.h"
#include "Core/CameraAssetReference.h"
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

	TConstArrayView<FPropertyBagPropertyDesc> PropertyDescs = CameraParametersStruct->GetPropertyDescs();
	const TArray<TWeakObjectPtr<const UCameraRigAsset>>& ParameterOwners = CameraAsset->ParameterOwners;
	ensure(PropertyDescs.Num() == ParameterOwners.Num());

	FCameraParameterOverrideHelper Helper(*OutVariableTable, *OutContextDataTable);

	for (int32 Index = 0, MaxIndex = FMath::Min(PropertyDescs.Num(), ParameterOwners.Num()); Index < MaxIndex; ++Index)
	{
		const FPropertyBagPropertyDesc& PropertyDesc(PropertyDescs[Index]);
		if (!CameraReference.IsParameterOverriden(PropertyDesc.ID))
		{
			continue;
		}

		const UCameraRigAsset* CameraRig = ParameterOwners[Index].Get();
		if (!ensure(CameraRig))
		{
			continue;
		}

		TValueOrError<FStructView, EPropertyBagResult> ParameterValueOrError = CameraParameters.GetValueStruct(PropertyDesc);
		Helper.ApplyParameterOverride(CameraRig, PropertyDesc.ID, ParameterValueOrError, bDrivenOverridesOnly);
	}
}

}  // namespace UE::Cameras

