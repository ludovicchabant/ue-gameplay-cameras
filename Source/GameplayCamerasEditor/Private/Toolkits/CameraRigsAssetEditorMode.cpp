// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/CameraRigsAssetEditorMode.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Editor.h"
#include "Editors/CameraNodeGraphSchema.h"
#include "Editors/CameraRigTransitionGraphSchema.h"
#include "Editors/SCameraRigAssetEditor.h"
#include "Editors/SFindInObjectTreeGraph.h"
#include "Framework/Docking/TabManager.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "ToolMenus.h"
#include "Toolkits/CameraRigAssetEditorToolkitBase.h"
#include "Toolkits/StandardToolkitLayout.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "CameraRigsAssetEditorMode"

namespace UE::Cameras
{

FName FCameraRigsAssetEditorMode::ModeName(TEXT("CameraRigs"));

const FName FCameraRigsAssetEditorMode::CameraRigsTabId(TEXT("CameraRigAssetEditor_CameraRigs"));

FCameraRigsAssetEditorMode::FCameraRigsAssetEditorMode(UCameraAsset* InCameraAsset)
	: FAssetEditorMode(ModeName)
	, CameraAsset(InCameraAsset)
{
	Impl = MakeShared<FCameraRigAssetEditorToolkitBase>(TEXT("CameraAssetEditor_Mode_CameraRigs_v1"));

	TSharedPtr<FStandardToolkitLayout> StandardLayout = Impl->GetStandardLayout();
	{
		StandardLayout->AddLeftTab(CameraRigsTabId);
	}
	DefaultLayout = StandardLayout->GetLayout();

	UClass* NodeGraphSchemaClass = UCameraNodeGraphSchema::StaticClass();
	UCameraNodeGraphSchema* DefaultNodeGraphSchema = Cast<UCameraNodeGraphSchema>(NodeGraphSchemaClass->GetDefaultObject());
	NodeGraphConfig = DefaultNodeGraphSchema->BuildGraphConfig();

	UClass* TransitionSchemaClass = UCameraRigTransitionGraphSchema::StaticClass();
	UCameraRigTransitionGraphSchema* DefaultTransitionGraphSchema = Cast<UCameraRigTransitionGraphSchema>(TransitionSchemaClass->GetDefaultObject());
	TransitionGraphConfig = DefaultTransitionGraphSchema->BuildGraphConfig();
}

void FCameraRigsAssetEditorMode::OnActivateMode(const FAssetEditorModeActivateParams& InParams)
{
	if (!bInitializedToolkit)
	{
		Impl->CreateWidgets();

		CameraRigsListWidget = SNew(SCameraRigList)
			.CameraAsset(CameraAsset)
			.OnCameraRigListChanged(this, &FCameraRigsAssetEditorMode::OnCameraRigListChanged)
			.OnRequestEditCameraRig(this, &FCameraRigsAssetEditorMode::OnCameraRigEditRequested)
			.OnCameraRigDeleted(this, &FCameraRigsAssetEditorMode::OnCameraRigDeleted);

		bInitializedToolkit = true;
	}

	Impl->RegisterTabSpawners(InParams.TabManager.ToSharedRef(), InParams.AssetEditorTabsCategory);

	const FName CamerasStyleSetName = FGameplayCamerasEditorStyle::Get()->GetStyleSetName();

	InParams.TabManager->RegisterTabSpawner(CameraRigsTabId, FOnSpawnTab::CreateSP(this, &FCameraRigsAssetEditorMode::SpawnTab_CameraRigs))
		.SetDisplayName(LOCTEXT("CameraRigs", "Camera Rigs"))
		.SetGroup(InParams.AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(CamerasStyleSetName, "CameraAssetEditor.Tabs.CameraRigs"));

	FToolMenuOwnerScoped OwnerScoped(this);
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(InParams.ToolbarMenuName);
	Impl->BuildToolbarMenu(ToolbarMenu);

	Impl->BindCommands(InParams.CommandList.ToSharedRef());

	Impl->OnCameraRigBuildStatusDirtied().AddSP(this, &FCameraRigsAssetEditorMode::OnCameraRigBuildStatusDirtied);

	if (GEditor)
	{
		GEditor->RegisterForUndo(this);
	}
}

TSharedRef<SDockTab> FCameraRigsAssetEditorMode::SpawnTab_CameraRigs(const FSpawnTabArgs& Args)
{
	TSharedPtr<SDockTab> CameraRigsTab = SNew(SDockTab)
		.Label(LOCTEXT("CameraRigsTitle", "Camera Rigs"))
		[
			CameraRigsListWidget.ToSharedRef()
		];

	return CameraRigsTab.ToSharedRef();
}

void FCameraRigsAssetEditorMode::OnDeactivateMode(const FAssetEditorModeDeactivateParams& InParams)
{
	if (GEditor)
	{
		GEditor->UnregisterForUndo(this);
	}

	Impl->OnCameraRigBuildStatusDirtied().RemoveAll(this);

	Impl->UnregisterTabSpawners(InParams.TabManager.ToSharedRef());

	InParams.TabManager->UnregisterTabSpawner(CameraRigsTabId);

	UToolMenus::UnregisterOwner(this);
}

void FCameraRigsAssetEditorMode::PostUndo(bool bSuccess)
{
	if (CameraRigsListWidget)
	{
		CameraRigsListWidget->RequestListRefresh();
	}
}

void FCameraRigsAssetEditorMode::PostRedo(bool bSuccess)
{
	if (CameraRigsListWidget)
	{
		CameraRigsListWidget->RequestListRefresh();
	}
}

void FCameraRigsAssetEditorMode::OnCameraRigListChanged(TArrayView<UCameraRigAsset* const> InCameraRigs)
{
}

void FCameraRigsAssetEditorMode::OnCameraRigEditRequested(UCameraRigAsset* InCameraRig)
{
	Impl->SetCameraRigAsset(InCameraRig);
}

void FCameraRigsAssetEditorMode::OnCameraRigDeleted(const TArray<UCameraRigAsset*>& InCameraRigs)
{
	if (InCameraRigs.Contains(Impl->GetCameraRigAsset()))
	{
		Impl->SetCameraRigAsset(nullptr);
	}
}

void FCameraRigsAssetEditorMode::OnCameraRigBuildStatusDirtied()
{
	if (CameraAsset)
	{
		CameraAsset->DirtyBuildStatus();
	}
}

void FCameraRigsAssetEditorMode::OnGetRootObjectsToSearch(TArray<FFindInObjectTreeGraphSource>& OutSources)
{
	for (UCameraRigAsset* CameraRig : CameraAsset->GetCameraRigs())
	{
		OutSources.Add(FFindInObjectTreeGraphSource{ CameraRig, &NodeGraphConfig });
		OutSources.Add(FFindInObjectTreeGraphSource{ CameraRig, &TransitionGraphConfig });
	}
}

bool FCameraRigsAssetEditorMode::JumpToObject(UObject* InObject, FName PropertyName)
{
	UCameraRigAsset* FindInCameraRig = nullptr;
	UObject* CurOuter = InObject;
	while (CurOuter != nullptr)
	{
		if (CurOuter->IsA<UCameraRigAsset>())
		{
			FindInCameraRig = Cast<UCameraRigAsset>(CurOuter);
			break;
		}
		CurOuter = CurOuter->GetOuter();
	}
	if (!FindInCameraRig)
	{
		return false;
	}

	Impl->SetCameraRigAsset(FindInCameraRig);

	if (TSharedPtr<SCameraRigAssetEditor> CameraRigAssetEditor = Impl->GetCameraRigAssetEditor())
	{
		return CameraRigAssetEditor->FindAndJumpToObjectNode(InObject);
	}
	return false;
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

