// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraShakeAssetReference.h"

#include "Core/CameraShakeAsset.h"
#include "Helpers/CameraObjectReferenceParameterOverrideEvaluator.h"

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

void FCameraShakeAssetReference::ApplyParameterOverrides(UE::Cameras::FCameraNodeEvaluationResult& OutResult, bool bDrivenOverridesOnly)
{
	using namespace UE::Cameras;
	FCameraObjectReferenceParameterOverrideEvaluator OverrideEvaluator(*this);
	OverrideEvaluator.ApplyParameterOverrides(OutResult.VariableTable, OutResult.ContextDataTable, bDrivenOverridesOnly);
}

