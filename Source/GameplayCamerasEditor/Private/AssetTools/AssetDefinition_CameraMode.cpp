// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTools/AssetDefinition_CameraMode.h"

#include "IGameplayCamerasEditorModule.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Toolkits/CameraModeAssetEditorToolkit.h"
#include "Toolkits/IToolkit.h"

#define LOCTEXT_NAMESPACE "AssetDefinition_CameraMode"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_CameraMode)

FText UAssetDefinition_CameraMode::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDisplayName", "Camera Mode");
}

FLinearColor UAssetDefinition_CameraMode::GetAssetColor() const
{
	return FLinearColor(FColor(200, 80, 80));
}

TSoftClassPtr<UObject> UAssetDefinition_CameraMode::GetAssetClass() const
{
	return UCameraMode::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_CameraMode::GetAssetCategories() const
{
	static const auto Categories = { FAssetCategoryPath(EAssetCategoryPaths::Gameplay, LOCTEXT("AssetSubCategory", "Cameras")) };
	return Categories;
}

FAssetOpenSupport UAssetDefinition_CameraMode::GetAssetOpenSupport(const FAssetOpenSupportArgs& OpenSupportArgs) const
{
	return FAssetOpenSupport(
			OpenSupportArgs.OpenMethod,
			OpenSupportArgs.OpenMethod == EAssetOpenMethod::Edit,
			EToolkitMode::Standalone); 
}

EAssetCommandResult UAssetDefinition_CameraMode::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	IGameplayCamerasEditorModule& GameplayCamerasEditorModule = FModuleManager::LoadModuleChecked<IGameplayCamerasEditorModule>("GameplayCamerasEditor");
	for (UCameraMode* CameraMode : OpenArgs.LoadObjects<UCameraMode>())
	{
		GameplayCamerasEditorModule.CreateCameraModeEditor(OpenArgs.GetToolkitMode(), OpenArgs.ToolkitHost, CameraMode);
	}	

	return EAssetCommandResult::Handled;
}

#undef LOCTEXT_NAMESPACE

