// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTools/CameraModeAssetEditor.h"

#include "Core/CameraMode.h"
#include "EditorModeManager.h"
#include "Toolkits/CameraModeAssetEditorToolkit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraModeAssetEditor)

void UCameraModeAssetEditor::Initialize(TObjectPtr<UCameraMode> InCameraModeAsset)
{
	CameraModeAsset = InCameraModeAsset;

	Super::Initialize();
}

void UCameraModeAssetEditor::GetObjectsToEdit(TArray<UObject*>& InObjectsToEdit)
{
	InObjectsToEdit.Add(CameraModeAsset.Get());
}

TSharedPtr<FBaseAssetToolkit> UCameraModeAssetEditor::CreateToolkit()
{
	return MakeShared<FCameraModeAssetEditorToolkit>(this);
}

