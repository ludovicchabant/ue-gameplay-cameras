// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraShakeCameraNodeGraphSchema.h"

#include "Core/CameraShakeAsset.h"
#include "Core/ShakeCameraNode.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "GameplayCamerasEditorSettings.h"
#include "Nodes/Blends/SimpleBlendCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraShakeCameraNodeGraphSchema)

#define LOCTEXT_NAMESPACE "CameraShakeCameraNodeGraphSchema"

UCameraShakeCameraNodeGraphSchema::UCameraShakeCameraNodeGraphSchema(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
}

FObjectTreeGraphConfig UCameraShakeCameraNodeGraphSchema::BuildGraphConfig() const
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	FObjectTreeGraphConfig GraphConfig;

	BuildBaseGraphConfig(GraphConfig);

	GraphConfig.ConnectableObjectClasses.Add(UShakeCameraNode::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(USimpleFixedTimeBlendCameraNode::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraShakeAsset::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UCameraShakeAsset::StaticClass())
		.OnlyAsRoot()
		.HasSelfPin(false)
		.NodeTitleUsesObjectName(true)
		.NodeTitleColor(Settings->CameraShakeAssetTitleColor);

	return GraphConfig;
}

#undef LOCTEXT_NAMESPACE

