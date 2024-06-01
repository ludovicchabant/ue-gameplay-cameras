// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTools/CameraRigTransitionEditor.h"

#include "Core/CameraRigTransition.h"
#include "EditorModeManager.h"
#include "Toolkits/CameraRigTransitionEditorToolkit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigTransitionEditor)

void UCameraRigTransitionEditor::Initialize(TObjectPtr<UObject> InTransitionOwner)
{
	TransitionOwner = InTransitionOwner;

	Super::Initialize();
}

void UCameraRigTransitionEditor::GetObjectsToEdit(TArray<UObject*>& InObjectsToEdit)
{
	InObjectsToEdit.Add(TransitionOwner.Get());
}

TSharedPtr<FBaseAssetToolkit> UCameraRigTransitionEditor::CreateToolkit()
{
	return MakeShared<UE::Cameras::FCameraRigTransitionEditorToolkit>(this);
}

