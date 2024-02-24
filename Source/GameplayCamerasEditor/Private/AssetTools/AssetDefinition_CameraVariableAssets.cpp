// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTools/AssetDefinition_CameraVariableAssets.h"

#include "Core/CameraVariableAssets.h"
#include "Toolkits/SimpleAssetEditor.h"

#define LOCTEXT_NAMESPACE "AssetDefinition_CameraVariableAssets"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_CameraVariableAssets)

TConstArrayView<FAssetCategoryPath> UAssetDefinition_CameraVariableAsset::StaticMenuCategories()
{
	static const auto Categories = { FAssetCategoryPath(EAssetCategoryPaths::Gameplay) };
	return Categories;
}

FLinearColor UAssetDefinition_CameraVariableAsset::GetAssetColor() const
{
	return FLinearColor(FColor(200, 80, 80));
}

TSoftClassPtr<UObject> UAssetDefinition_CameraVariableAsset::GetAssetClass() const
{
	return UCameraVariableAsset::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_CameraVariableAsset::GetAssetCategories() const
{
	return StaticMenuCategories();
}

EAssetCommandResult UAssetDefinition_CameraVariableAsset::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	TArray<UObject*> Objects = OpenArgs.LoadObjects<UObject>();
	FSimpleAssetEditor::CreateEditor(OpenArgs.GetToolkitMode(), OpenArgs.ToolkitHost, Objects);

	return EAssetCommandResult::Handled;
}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	FText UAssetDefinition_##ValueName##CameraVariable::GetAssetDisplayName() const\
	{\
		return LOCTEXT("AssetDisplayName", #ValueName " Camera Variable");\
	}\
	TSoftClassPtr<UObject> UAssetDefinition_##ValueName##CameraVariable::GetAssetClass() const\
	{\
		return U##ValueName##CameraVariable::StaticClass();\
	}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

#undef LOCTEXT_NAMESPACE

