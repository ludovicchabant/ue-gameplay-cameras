// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/Package.h"
#include "FrameNumberDisplayFormat.h"

#include "GameplayCamerasEditorSettings.generated.h"

UCLASS(config=EditorPerProjectUserSettings)
class GAMEPLAYCAMERASEDITOR_API UGameplayCamerasEditorSettings : public UObject
{
	GENERATED_BODY()

public:

	UGameplayCamerasEditorSettings(const FObjectInitializer& ObjectInitializer);	

public:

	UPROPERTY(EditAnywhere, Config, Category=NodeTitleColors)
	FLinearColor CameraNodeTitleColor;

	UPROPERTY(EditAnywhere, Config, Category=NodeTitleColors)
	FLinearColor CameraAssetTitleColor;

	UPROPERTY(EditAnywhere, Config, Category=NodeTitleColors)
	FLinearColor CameraRigAssetTitleColor;

	UPROPERTY(EditAnywhere, Config, Category=NodeTitleColors)
	FLinearColor CameraRigTransitionTitleColor;

	UPROPERTY(EditAnywhere, Config, Category=NodeTitleColors)
	FLinearColor CameraRigTransitionConditionTitleColor;

	UPROPERTY(EditAnywhere, Config, Category=NodeTitleColors)
	FLinearColor CameraBlendNodeTitleColor;

public:

	UPROPERTY()
	FName LastCameraAssetToolkitModeName;
};

