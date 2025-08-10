// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraShakeAssetReference.h"

#include "Core/CameraShakeAsset.h"
#include "Core/CameraVariableTable.h"
#include "Helpers/CameraObjectInterfaceParameterOverrideHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraShakeAssetReference)

FCameraShakeAssetReference::FCameraShakeAssetReference()
{
}

FCameraShakeAssetReference::FCameraShakeAssetReference(UCameraShakeAsset* InCameraShake)
	: CameraShake(InCameraShake)
{
}

const UBaseCameraObject* FCameraShakeAssetReference::GetCameraObject() const
{
	return CameraShake;
}

void FCameraShakeAssetReference::ApplyParameterOverrides(UE::Cameras::FCameraVariableTable& OutVariableTable, bool bDrivenOnly) const
{
	ApplyParameterOverridesImpl(&OutVariableTable, nullptr, bDrivenOnly);
}

void FCameraShakeAssetReference::ApplyParameterOverrides(UE::Cameras::FCameraVariableTable& OutVariableTable, UE::Cameras::FCameraContextDataTable& OutContextDataTable, bool bDrivenOnly) const
{
	ApplyParameterOverridesImpl(&OutVariableTable, &OutContextDataTable, bDrivenOnly);
}

void FCameraShakeAssetReference::ApplyParameterOverrides(UE::Cameras::FCameraNodeEvaluationResult& OutResult, bool bDrivenOnly) const
{
	ApplyParameterOverridesImpl(&OutResult.VariableTable, &OutResult.ContextDataTable, bDrivenOnly);
}

void FCameraShakeAssetReference::ApplyParameterOverridesImpl(UE::Cameras::FCameraVariableTable* OutVariableTable, UE::Cameras::FCameraContextDataTable* OutContextDataTable, bool bDrivenOnly) const
{
	using namespace UE::Cameras;
	
	if (CameraShake)
	{
		FCameraObjectInterfaceParameterOverrideHelper Helper(OutVariableTable, OutContextDataTable);
		Helper.ApplyParameterOverrides(CameraShake, CameraShake->GetParameterDefinitions(), Parameters, bDrivenOnly);
	}
}

