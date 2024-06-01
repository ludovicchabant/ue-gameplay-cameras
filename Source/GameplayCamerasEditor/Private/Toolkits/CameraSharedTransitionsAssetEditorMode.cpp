// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/CameraSharedTransitionsAssetEditorMode.h"

#include "Core/CameraAsset.h"
#include "Editors/CameraRigTransitionGraphSchema.h"
#include "Editors/SCameraRigTransitionEditor.h"
#include "ToolMenus.h"
#include "Toolkits/CameraRigTransitionEditorToolkitBase.h"
#include "Toolkits/StandardToolkitLayout.h"

#define LOCTEXT_NAMESPACE "CameraSharedTransitionsAssetEditorMode"

namespace UE::Cameras
{

namespace Internal
{

class FCameraSharedTransitionsAssetEditorModeBaseImpl : public FCameraRigTransitionEditorToolkitBase
{
public:

	FCameraSharedTransitionsAssetEditorModeBaseImpl()
		: FCameraRigTransitionEditorToolkitBase(TEXT("CameraAssetEditor_Mode_SharedTransitions_v1"))
	{}

protected:

	virtual void GetTransitionOwnerInfo(FCameraRigTransitionOwnerInfo& OutTransitionOwnerInfo) override
	{
		OutTransitionOwnerInfo.TransitionOwnerClass = UCameraAsset::StaticClass();
		OutTransitionOwnerInfo.GraphName = UCameraAsset::SharedTransitionsGraphName;
		OutTransitionOwnerInfo.EnterTransitionsPropertyName = GET_MEMBER_NAME_CHECKED(UCameraAsset, EnterTransitions);
		OutTransitionOwnerInfo.ExitTransitionsPropertyName = GET_MEMBER_NAME_CHECKED(UCameraAsset, ExitTransitions);
	}

	virtual void GetTransitionGraphDisplayInfo(FGraphDisplayInfo& OutGraphDisplayInfo) override
	{
		OutGraphDisplayInfo.PlainName = LOCTEXT("SharedTransitionGraphPlainName", "SharedTransitions");
		OutGraphDisplayInfo.DisplayName = LOCTEXT("SharedTransitionGraphDisplayName", "Shared Transitions");
	}

	virtual void GetTransitionGraphAppearanceInfo(FGraphAppearanceInfo& OutGraphAppearanceInfo) override
	{
		OutGraphAppearanceInfo.CornerText = LOCTEXT("SharedTransitionGraphCornerText", "SHARED TRANSITIONS");
	}
};

}  // namespace Internal

FName FCameraSharedTransitionsAssetEditorMode::ModeName(TEXT("SharedTransitions"));

FCameraSharedTransitionsAssetEditorMode::FCameraSharedTransitionsAssetEditorMode(UCameraAsset* InCameraAsset)
	: FAssetEditorMode(ModeName)
	, CameraAsset(InCameraAsset)
{
	Impl = MakeShared<Internal::FCameraSharedTransitionsAssetEditorModeBaseImpl>();

	Impl->SetTransitionOwner(CameraAsset);

	DefaultLayout = Impl->GetStandardLayout()->GetLayout();
}

void FCameraSharedTransitionsAssetEditorMode::OnActivateMode(const FAssetEditorModeActivateParams& InParams)
{
	if (!bInitializedToolkit)
	{
		Impl->CreateWidgets();
		bInitializedToolkit = true;
	}

	Impl->RegisterTabSpawners(InParams.TabManager.ToSharedRef(), InParams.AssetEditorTabsCategory);

	FToolMenuOwnerScoped OwnerScoped(this);
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(InParams.ToolbarMenuName);
	Impl->BuildToolbarMenu(ToolbarMenu);
}

void FCameraSharedTransitionsAssetEditorMode::OnDeactivateMode(const FAssetEditorModeDeactivateParams& InParams)
{
	Impl->UnregisterTabSpawners(InParams.TabManager.ToSharedRef());

	UToolMenus::UnregisterOwner(this);
}

bool FCameraSharedTransitionsAssetEditorMode::JumpToNode(UEdGraphNode* InNode)
{
	return false;
}

bool FCameraSharedTransitionsAssetEditorMode::JumpToObject(UObject* InObject)
{
	TSharedPtr<SCameraRigTransitionEditor> TransitionEditor = Impl->GetCameraRigTransitionEditor();
	return TransitionEditor->FindAndJumpToObjectNode(InObject);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

