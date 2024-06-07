// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/CameraAssetEditorToolkit.h"

#include "AssetTools/CameraAssetEditor.h"
#include "Commands/CameraAssetEditorCommands.h"
#include "Core/CameraAsset.h"
#include "Core/CameraBuildLog.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/SFindInObjectTreeGraph.h"
#include "Framework/Docking/LayoutExtender.h"
#include "Framework/Docking/TabManager.h"
#include "GameplayCamerasEditorSettings.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "ToolMenus.h"
#include "Toolkits/BuildButtonToolkit.h"
#include "Toolkits/CameraBuildLogToolkit.h"
#include "Toolkits/CameraDirectorAssetEditorMode.h"
#include "Toolkits/CameraRigsAssetEditorMode.h"
#include "Toolkits/CameraSharedTransitionsAssetEditorMode.h"
#include "Toolkits/StandardToolkitLayout.h"
#include "Widgets/Docking/SDockTab.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraAssetEditorToolkit)

#define LOCTEXT_NAMESPACE "CameraAssetEditorToolkit"

namespace UE::Cameras
{

const FName FCameraAssetEditorToolkit::SearchTabId(TEXT("CameraAssetEditor_Search"));
const FName FCameraAssetEditorToolkit::MessagesTabId(TEXT("CameraAssetEditor_Messages"));

FCameraAssetEditorToolkit::FCameraAssetEditorToolkit(UCameraAssetEditor* InOwningAssetEditor)
	: FAssetEditorModeManagerToolkit(InOwningAssetEditor)
	, CameraAsset(InOwningAssetEditor->GetCameraAsset())
	, StandardLayout(new FStandardToolkitLayout(TEXT("CameraAssetEditor_Layout_v2")))
	, BuildButtonToolkit(MakeShared<FBuildButtonToolkit>(CameraAsset))
	, BuildLogToolkit(MakeShared<FCameraBuildLogToolkit>())
{
	StandardLayout->AddBottomTab(SearchTabId);
	StandardLayout->AddBottomTab(MessagesTabId);

	TSharedPtr<FLayoutExtender> NewLayoutExtender = MakeShared<FLayoutExtender>();
	{
		NewLayoutExtender->ExtendStack(
				FStandardToolkitLayout::BottomStackExtensionId,
				ELayoutExtensionPosition::After,
				FTabManager::FTab(SearchTabId, ETabState::ClosedTab));

		NewLayoutExtender->ExtendStack(
				FStandardToolkitLayout::BottomStackExtensionId,
				ELayoutExtensionPosition::After,
				FTabManager::FTab(MessagesTabId, ETabState::ClosedTab));
	}
	LayoutExtenders.Add(NewLayoutExtender);
}

FCameraAssetEditorToolkit::~FCameraAssetEditorToolkit()
{
}

void FCameraAssetEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(CameraAsset);
}

FString FCameraAssetEditorToolkit::GetReferencerName() const
{
	return TEXT("FCameraAssetEditorToolkit");
}

void FCameraAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	const FName CamerasStyleSetName = FGameplayCamerasEditorStyle::Get()->GetStyleSetName();

	InTabManager->RegisterTabSpawner(SearchTabId, FOnSpawnTab::CreateSP(this, &FCameraAssetEditorToolkit::SpawnTab_Search))
		.SetDisplayName(LOCTEXT("Search", "Search"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(CamerasStyleSetName, "CameraAssetEditor.Tabs.Search"));

	InTabManager->RegisterTabSpawner(MessagesTabId, FOnSpawnTab::CreateSP(this, &FCameraAssetEditorToolkit::SpawnTab_Messages))
		.SetDisplayName(LOCTEXT("Messages", "Messages"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(CamerasStyleSetName, "CameraAssetEditor.Tabs.Messages"));
}

TSharedRef<SDockTab> FCameraAssetEditorToolkit::SpawnTab_Search(const FSpawnTabArgs& Args)
{
	TSharedPtr<SDockTab> SearchTab = SNew(SDockTab)
		.Label(LOCTEXT("SearchTabTitle", "Search"))
		[
			SearchWidget.ToSharedRef()
		];

	return SearchTab.ToSharedRef();
}

TSharedRef<SDockTab> FCameraAssetEditorToolkit::SpawnTab_Messages(const FSpawnTabArgs& Args)
{
	TSharedPtr<SDockTab> MessagesTab = SNew(SDockTab)
		.Label(LOCTEXT("MessagesTabTitle", "Messages"))
		[
			BuildLogToolkit->GetMessagesWidget().ToSharedRef()
		];

	return MessagesTab.ToSharedRef();
}

void FCameraAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(SearchTabId);
	InTabManager->UnregisterTabSpawner(MessagesTabId);
}

void FCameraAssetEditorToolkit::CreateWidgets()
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	// ...no up-call...

	RegisterToolbar();
	CreateEditorModeManager();
	LayoutExtender = MakeShared<FLayoutExtender>();
	// We don't want a details view, but we need to because otherwise it crashes.
	DetailsView = CreateDetailsView();

	// Do our custom stuff.

	// Create the search panel.
	SearchWidget = SNew(SFindInObjectTreeGraph)
		.OnGetRootObjectsToSearch(this, &FCameraAssetEditorToolkit::OnGetRootObjectsToSearch)
		.OnJumpToObjectRequested(this, &FCameraAssetEditorToolkit::OnJumpToObject);

	// Create the message log.
	BuildLogToolkit->Initialize("CameraAssetBuildMessages");
}

void FCameraAssetEditorToolkit::RegisterToolbar()
{
	FName ParentName;
	const FName MenuName = GetToolMenuToolbarName(ParentName);
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus->IsMenuRegistered(MenuName))
	{
		FToolMenuOwnerScoped ToolMenuOwnerScope(this);

		UToolMenu* ToolbarMenu = UToolMenus::Get()->RegisterMenu(
				MenuName, ParentName, EMultiBoxType::ToolBar);

		FToolMenuInsert InsertAfterAssetSection("Asset", EToolMenuInsertType::After);
		const FCameraAssetEditorCommands& Commands = FCameraAssetEditorCommands::Get();

		ToolbarMenu->AddDynamicSection("Tools", FNewToolMenuDelegate::CreateLambda(
				[&Commands](UToolMenu* InMenu)
				{
					UCameraAssetEditorMenuContext* Context = InMenu->FindContext<UCameraAssetEditorMenuContext>();
					FCameraAssetEditorToolkit* This = Context ? Context->Toolkit.Pin().Get() : nullptr;
					if (!ensure(This))
					{
						return;
					}

					FToolMenuSection& ToolsSection = InMenu->AddSection("Tools");
					ToolsSection.AddEntry(This->BuildButtonToolkit->MakeToolbarButton(Commands.Build));
					ToolsSection.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.FindInCamera));
				}),
				InsertAfterAssetSection);

		FToolMenuSection& ModesSection = ToolbarMenu->AddSection("EditorModes", TAttribute<FText>(), InsertAfterAssetSection);
		ModesSection.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.ShowCameraDirector));
		ModesSection.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.ShowCameraRigs));
		ModesSection.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.ShowSharedTransitions));
	}
}

void FCameraAssetEditorToolkit::InitToolMenuContext(FToolMenuContext& MenuContext)
{
	FBaseAssetToolkit::InitToolMenuContext(MenuContext);

	UCameraAssetEditorMenuContext* Context = NewObject<UCameraAssetEditorMenuContext>();
	Context->Toolkit = SharedThis(this);
	MenuContext.AddObject(Context);
}

void FCameraAssetEditorToolkit::PostInitAssetEditor()
{
	Settings = GetMutableDefault<UGameplayCamerasEditorSettings>();

	const FName CameraDirectorModeName = FCameraDirectorAssetEditorMode::ModeName;
	AddEditorMode(MakeShared<FCameraDirectorAssetEditorMode>(CameraAsset));

	const FName CameraRigsModeName = FCameraRigsAssetEditorMode::ModeName;
	AddEditorMode(MakeShared<FCameraRigsAssetEditorMode>(CameraAsset));

	const FName SharedTransitionsModeName = FCameraSharedTransitionsAssetEditorMode::ModeName;
	AddEditorMode(MakeShared<FCameraSharedTransitionsAssetEditorMode>(CameraAsset));

	const FCameraAssetEditorCommands& Commands = FCameraAssetEditorCommands::Get();
	TMap<FName, TSharedPtr<FUICommandInfo>> ModeCommands;
	ModeCommands.Add(CameraDirectorModeName, Commands.ShowCameraDirector);
	ModeCommands.Add(CameraRigsModeName, Commands.ShowCameraRigs);
	ModeCommands.Add(SharedTransitionsModeName, Commands.ShowSharedTransitions);
	for (auto& Pair : ModeCommands)
	{
		ToolkitCommands->MapAction(
			Pair.Value,
			FExecuteAction::CreateSP(this, &FCameraAssetEditorToolkit::SetEditorMode, Pair.Key),
			FCanExecuteAction::CreateSP(this, &FCameraAssetEditorToolkit::CanSetEditorMode, Pair.Key),
			FIsActionChecked::CreateSP(this, &FCameraAssetEditorToolkit::IsEditorMode, Pair.Key));
	}

	ToolkitCommands->MapAction(
		Commands.Build,
		FExecuteAction::CreateSP(this, &FCameraAssetEditorToolkit::OnBuild));

	ToolkitCommands->MapAction(
		Commands.FindInCamera,
		FExecuteAction::CreateSP(this, &FCameraAssetEditorToolkit::OnFindInCamera));

	BuildLogToolkit->OnRequestJumpToObject().BindSP(this, &FCameraAssetEditorToolkit::OnJumpToObject);

	IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
	LiveEditManager = GameplayCamerasModule.GetLiveEditManager();

	const FName InitialModeName = !Settings->LastCameraAssetToolkitModeName.IsNone() ? 
		Settings->LastCameraAssetToolkitModeName : CameraRigsModeName;
	SetEditorMode(InitialModeName);
}

void FCameraAssetEditorToolkit::OnEditorToolkitModeActivated()
{
	Settings->LastCameraAssetToolkitModeName = GetCurrentEditorModeName();
	Settings->SaveConfig();
}

void FCameraAssetEditorToolkit::OnBuild()
{
	if (!CameraAsset)
	{
		return;
	}

	FCameraBuildLog BuildLog;
	CameraAsset->BuildCamera(BuildLog);
	BuildLogToolkit->PopulateMessageListing(BuildLog);

	if (CameraAsset->BuildStatus != ECameraBuildStatus::Clean)
	{
		TabManager->TryInvokeTab(MessagesTabId);
	}

	for (UCameraRigAsset* CameraRigAsset : CameraAsset->CameraRigs)
	{
		FCameraRigPackages BuiltPackages;
		CameraRigAsset->GatherPackages(BuiltPackages);

		for (const UPackage* BuiltPackage : BuiltPackages)
		{
			LiveEditManager->NotifyPostBuildAsset(BuiltPackage);
		}
	}
}

void FCameraAssetEditorToolkit::OnFindInCamera()
{
	TabManager->TryInvokeTab(SearchTabId);
	SearchWidget->FocusSearchEditBox();
}

void FCameraAssetEditorToolkit::OnGetRootObjectsToSearch(TArray<FFindInObjectTreeGraphSource>& OutSources)
{
	TSharedPtr<FCameraRigsAssetEditorMode> CameraRigsMode = GetTypedEditorMode<FCameraRigsAssetEditorMode>(
			FCameraRigsAssetEditorMode::ModeName);
	CameraRigsMode->OnGetRootObjectsToSearch(OutSources);

	TSharedPtr<FCameraSharedTransitionsAssetEditorMode> SharedTransitionsMode = GetTypedEditorMode<FCameraSharedTransitionsAssetEditorMode>(
			FCameraSharedTransitionsAssetEditorMode::ModeName);
	SharedTransitionsMode->OnGetRootObjectsToSearch(OutSources);
}

void FCameraAssetEditorToolkit::OnJumpToObject(UObject* Object)
{
	OnJumpToObject(Object, NAME_None);
}

void FCameraAssetEditorToolkit::OnJumpToObject(UObject* Object, FName PropertyName)
{
	bool bFindInCameraRig = false;
	bool bFindInSharedTranstions = false;
	UObject* CurOuter = Object;
	while (CurOuter != nullptr)
	{
		if (CurOuter->IsA<UCameraRigAsset>())
		{
			bFindInCameraRig = true;
			break;
		}
		if (CurOuter == CameraAsset)
		{
			bFindInSharedTranstions = true;
			break;
		}

		CurOuter = CurOuter->GetOuter();
	}
	
	if (bFindInCameraRig)
	{
		TSharedPtr<FCameraRigsAssetEditorMode> CameraRigsMode = GetTypedEditorMode<FCameraRigsAssetEditorMode>(
				FCameraRigsAssetEditorMode::ModeName);
		SetEditorMode(FCameraRigsAssetEditorMode::ModeName);
		CameraRigsMode->JumpToObject(Object, PropertyName);
		return;
	}

	if (bFindInSharedTranstions)
	{
		TSharedPtr<FCameraSharedTransitionsAssetEditorMode> SharedTransitionsMode = GetTypedEditorMode<FCameraSharedTransitionsAssetEditorMode>(
				FCameraSharedTransitionsAssetEditorMode::ModeName);
		SetEditorMode(FCameraSharedTransitionsAssetEditorMode::ModeName);
		SharedTransitionsMode->JumpToObject(Object, PropertyName);
		return;
	}
}


FText FCameraAssetEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Camera Asset");
}

FName FCameraAssetEditorToolkit::GetToolkitFName() const
{
	static FName SequencerName("CameraAssetEditor");
	return SequencerName;
}

FString FCameraAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Camera Asset ").ToString();
}

FLinearColor FCameraAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.7, 0.0f, 0.0f, 0.5f);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

