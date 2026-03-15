// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCamerasEditorSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCamerasEditorSettings)

UGameplayCamerasEditorSettings::UGameplayCamerasEditorSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CategoryName = TEXT("Plugins");

	CameraNodeTitleColor = FLinearColor(0.190525f, 0.583898f, 1.0f, 1.0f);           // Light blue
	CameraAssetTitleColor = FLinearColor(1.0f, 0.170000f, 0.0f, 1.0f);				 // Orange
	CameraRigAssetTitleColor = FLinearColor(1.0f, 0.170000f, 0.0f, 1.0f);            // Orange
	CameraShakeAssetTitleColor = FLinearColor(1.0f, 0.170000f, 0.0f, 1.0f);          // Orange
	CameraRigTransitionTitleColor = FLinearColor(1.0f, 0.65f, 0.4f, 1.0f);           // Beige
	CameraRigTransitionConditionTitleColor = FLinearColor(0.8f, 0.4f, 0.4f, 1.0f);   // Salmon
	CameraBlendNodeTitleColor = FLinearColor(0.6f, 0.0f, 1.0f, 1.0f);                // Purple
	CameraRigParameterGetterNodeTitleColor = FLinearColor(0.8784f, 1.0f, 0.4f, 1.0f);// Pale green
	CameraVariableGetterNodeTitleColor = FLinearColor(1.0f, 0.591255f, 0.016512f, 1.0f);// Yellow
}

