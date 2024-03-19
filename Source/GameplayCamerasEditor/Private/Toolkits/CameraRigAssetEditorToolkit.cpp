// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/CameraRigAssetEditorToolkit.h"

#include "AssetTools/CameraRigAssetEditor.h"
#include "Commands/CameraRigAssetEditorCommands.h"
#include "Core/CameraRigAllocationInfoBuilder.h"
#include "Core/CameraRigAsset.h"
#include "EditorModeManager.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/SCameraRigAssetEditor.h"
#include "Editors/SFindInObjectTreeGraph.h"
#include "Editors/SObjectTreeGraphEditor.h"
#include "Editors/SObjectTreeGraphToolbox.h"
#include "Framework/Docking/LayoutExtender.h"
#include "Framework/Docking/TabManager.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasModule.h"
#include "IMessageLogListing.h"
#include "MessageLogInitializationOptions.h"
#include "MessageLogModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Widgets/Docking/SDockTab.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigAssetEditorToolkit)

#define LOCTEXT_NAMESPACE "CameraRigAssetEditorToolkit"

namespace UE::Cameras
{

const FName FCameraRigAssetEditorToolkit::ToolboxTabId(TEXT("CameraRigAssetEditor_Toolbox"));
const FName FCameraRigAssetEditorToolkit::CameraRigEditorTabId(TEXT("CameraRigAssetEditor_CameraRigEditor"));
const FName FCameraRigAssetEditorToolkit::SearchTabId(TEXT("CameraRigAssetEditor_Search"));
const FName FCameraRigAssetEditorToolkit::MessagesTabId(TEXT("CameraRigAssetEditor_Messages"));
const FName FCameraRigAssetEditorToolkit::DetailsViewTabId(TEXT("CameraRigAssetEditor_DetailsView"));

FCameraRigAssetEditorToolkit::FCameraRigAssetEditorToolkit(UCameraRigAssetEditor* InOwningAssetEditor)
	: FBaseAssetToolkit(InOwningAssetEditor)
	, CameraRigAsset(InOwningAssetEditor->GetCameraRigAsset())
	, CommandBindings(new FUICommandList())
{
	// Override base class default layout.
	StandaloneDefaultLayout = FTabManager::NewLayout("CameraRigAssetEditor_Layout_v5")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->AddTab(ToolboxTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.6f)
					->AddTab(CameraRigEditorTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->AddTab(DetailsViewTabId, ETabState::OpenedTab)
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.2f)
				->AddTab(SearchTabId, ETabState::ClosedTab)
				->AddTab(MessagesTabId, ETabState::ClosedTab)
			)
		);
}

FCameraRigAssetEditorToolkit::~FCameraRigAssetEditorToolkit()
{
}

void FCameraRigAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	const FName CamerasStyleSetName = FGameplayCamerasEditorStyle::Get()->GetStyleSetName();

	InTabManager->RegisterTabSpawner(ToolboxTabId, FOnSpawnTab::CreateSP(this, &FCameraRigAssetEditorToolkit::SpawnTab_Toolbox))
		.SetDisplayName(LOCTEXT("Toolbox", "Toolbox"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(CamerasStyleSetName, "CameraRigAssetEditor.Tabs.Toolbox"));

	InTabManager->RegisterTabSpawner(CameraRigEditorTabId, FOnSpawnTab::CreateSP(this, &FCameraRigAssetEditorToolkit::SpawnTab_CameraRigEditor))
		.SetDisplayName(LOCTEXT("CameraRigEditor", "Camera Rig"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef());

	InTabManager->RegisterTabSpawner(SearchTabId, FOnSpawnTab::CreateSP(this, &FCameraRigAssetEditorToolkit::SpawnTab_Search))
		.SetDisplayName(LOCTEXT("Search", "Search"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(CamerasStyleSetName, "CameraRigAssetEditor.Tabs.Search"));

	InTabManager->RegisterTabSpawner(MessagesTabId, FOnSpawnTab::CreateSP(this, &FCameraRigAssetEditorToolkit::SpawnTab_Messages))
		.SetDisplayName(LOCTEXT("Messages", "Messages"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(CamerasStyleSetName, "CameraRigAssetEditor.Tabs.Messages"));

	InTabManager->RegisterTabSpawner(DetailsViewTabId, FOnSpawnTab::CreateSP(this, &FCameraRigAssetEditorToolkit::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("Details", "Details"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

TSharedRef<SDockTab> FCameraRigAssetEditorToolkit::SpawnTab_Toolbox(const FSpawnTabArgs& Args)
{
	TSharedPtr<SDockTab> ToolboxTab = SNew(SDockTab)
		.Label(LOCTEXT("ToolboxTabTitle", "Toolbox"))
		[
			ToolboxWidget.ToSharedRef()
		];

	return ToolboxTab.ToSharedRef();
}

TSharedRef<SDockTab> FCameraRigAssetEditorToolkit::SpawnTab_CameraRigEditor(const FSpawnTabArgs& Args)
{
	TSharedPtr<SDockTab> CameraRigEditorTab = SNew(SDockTab)
		.Label(LOCTEXT("CameraRigEditorTabTitle", "Camera Rig Editor"))
		[
			CameraRigEditorWidget.ToSharedRef()
		];

	return CameraRigEditorTab.ToSharedRef();
}

TSharedRef<SDockTab> FCameraRigAssetEditorToolkit::SpawnTab_Search(const FSpawnTabArgs& Args)
{
	TSharedPtr<SDockTab> SearchTab = SNew(SDockTab)
		.Label(LOCTEXT("SearchTabTitle", "Search"))
		[
			SearchWidget.ToSharedRef()
		];

	return SearchTab.ToSharedRef();
}

TSharedRef<SDockTab> FCameraRigAssetEditorToolkit::SpawnTab_Messages(const FSpawnTabArgs& Args)
{
	TSharedPtr<SDockTab> MessagesTab = SNew(SDockTab)
		.Label(LOCTEXT("MessagesTabTitle", "Messages"))
		[
			MessagesWidget.ToSharedRef()
		];

	return MessagesTab.ToSharedRef();
}

void FCameraRigAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(ToolboxTabId);
	InTabManager->UnregisterTabSpawner(CameraRigEditorTabId);
	InTabManager->UnregisterTabSpawner(SearchTabId);
	InTabManager->UnregisterTabSpawner(MessagesTabId);
	InTabManager->UnregisterTabSpawner(DetailsViewTabId);
}

void FCameraRigAssetEditorToolkit::CreateWidgets()
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	// ...no up-call...

	// Do most of FBaseAssetToolkit's work except for the viewport.
	RegisterToolbar();
	LayoutExtender = MakeShared<FLayoutExtender>();

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.NotifyHook = this;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// Now do our custom stuff.

	// Create the camera rig editor.
	CameraRigEditorWidget = SNew(SCameraRigAssetEditor)
		.DetailsView(DetailsView)
		.CameraRigAsset(CameraRigAsset);

	// Create the toolbox, default to the rig editor items.
	ToolboxWidget = SNew(SObjectTreeGraphToolbox)
		.GraphConfig(CameraRigEditorWidget->GetFocusedGraphConfig());

	// Create the search panel.
	TArray<UEdGraph*> CameraRigGraphs;
	CameraRigEditorWidget->GetGraphs(CameraRigGraphs);
	SearchWidget = SNew(SFindInObjectTreeGraph)
		.GraphsToSearch(CameraRigGraphs)
		.OnJumpToNodeRequested(
				SFindInObjectTreeGraph::FOnJumpToNodeRequested::CreateSP(
					CameraRigEditorWidget.ToSharedRef(), &SCameraRigAssetEditor::JumpToNode));

	// Create the message log.
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions LogOptions;
	LogOptions.bShowPages = false;
	LogOptions.bShowFilters = false;
	LogOptions.bAllowClear = false;
	LogOptions.MaxPageCount = 1;
	MessageListing = MessageLogModule.CreateLogListing("CameraRigAssetEditorStats", LogOptions);

	MessagesWidget = MessageLogModule.CreateLogListingWidget(MessageListing.ToSharedRef());
}

void FCameraRigAssetEditorToolkit::RegisterToolbar()
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
		const FCameraRigAssetEditorCommands& Commands = FCameraRigAssetEditorCommands::Get();

		ToolbarMenu->AddDynamicSection("Tools", FNewToolMenuDelegate::CreateLambda(
				[&Commands](UToolMenu* InMenu)
				{
					UCameraRigAssetEditorMenuContext* Context = InMenu->FindContext<UCameraRigAssetEditorMenuContext>();
					FCameraRigAssetEditorToolkit* This = Context->CameraRigAssetEditorToolkit.Pin().Get();

					FToolMenuSection& ToolsSection = InMenu->AddSection("Tools");

					FToolMenuEntry BuildButton = FToolMenuEntry::InitToolBarButton(Commands.Build);
					BuildButton.Icon = TAttribute<FSlateIcon>(This, &FCameraRigAssetEditorToolkit::GetBuildButtonIcon);
					BuildButton.ToolTip = TAttribute<FText>(This, &FCameraRigAssetEditorToolkit::GetBuildButtonTooltip);
					ToolsSection.AddEntry(BuildButton);

					ToolsSection.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.FindInCameraRig));
				}),
				InsertAfterAssetSection);

		FToolMenuSection& GraphsSection = ToolbarMenu->AddSection("Graphs", TAttribute<FText>(), InsertAfterAssetSection);

		GraphsSection.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.FocusHome));
		GraphsSection.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.ShowNodeHierarchy));
		GraphsSection.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.ShowTransitions));
	}
}

void FCameraRigAssetEditorToolkit::InitToolMenuContext(FToolMenuContext& MenuContext)
{
	FBaseAssetToolkit::InitToolMenuContext(MenuContext);

	UCameraRigAssetEditorMenuContext* Context = NewObject<UCameraRigAssetEditorMenuContext>();
	Context->CameraRigAssetEditorToolkit = SharedThis(this);
	MenuContext.AddObject(Context);
}

FSlateIcon FCameraRigAssetEditorToolkit::GetBuildButtonIcon() const
{
	static const FName BuildStatusBackground("CameraRigAssetEditor.BuildStatus.Background");
	static const FName BuildStatusError("CameraRigAssetEditor.BuildStatus.Overlay.Error");
	static const FName BuildStatusGood("CameraRigAssetEditor.BuildStatus.Overlay.Good");
	static const FName BuildStatusUnknown("CameraRigAssetEditor.BuildStatus.Overlay.Unknown");
	static const FName BuildStatusWarning("CameraRigAssetEditor.BuildStatus.Overlay.Warning");

	const FName CamerasStyleSetName = FGameplayCamerasEditorStyle::Get()->GetStyleSetName();

	if (!CameraRigAsset)
	{
		return FSlateIcon(CamerasStyleSetName, BuildStatusBackground, NAME_None, BuildStatusError);
	}

	switch (CameraRigAsset->BuildStatus)
	{
		default:
		case ECameraRigBuildStatus::Dirty:
			return FSlateIcon(CamerasStyleSetName, BuildStatusBackground, NAME_None, BuildStatusUnknown);
		case ECameraRigBuildStatus::WithErrors:
			return FSlateIcon(CamerasStyleSetName, BuildStatusBackground, NAME_None, BuildStatusError);
		case ECameraRigBuildStatus::Clean:
			return FSlateIcon(CamerasStyleSetName, BuildStatusBackground, NAME_None, BuildStatusGood);
		case ECameraRigBuildStatus::CleanWithWarnings:
			return FSlateIcon(CamerasStyleSetName, BuildStatusBackground, NAME_None, BuildStatusWarning);
	}
}

FText FCameraRigAssetEditorToolkit::GetBuildButtonTooltip() const
{
	if (!CameraRigAsset)
	{
		return LOCTEXT("BuildButtonStatusNoAsset", "No asset is open");
	}

	switch (CameraRigAsset->BuildStatus)
	{
		default:
		case ECameraRigBuildStatus::Dirty:
			return LOCTEXT("BuildButtonStatusDirty", "Dirty or unknown, should rebuild");
		case ECameraRigBuildStatus::WithErrors:
			return LOCTEXT("BuildButtonStatusWithErrors", "There were errors during the build, see the log window for details");
		case ECameraRigBuildStatus::Clean:
			return LOCTEXT("BuildButtonStatusClean", "Good to go");
		case ECameraRigBuildStatus::CleanWithWarnings:
			return LOCTEXT("BuildButtonStatusCleanWithWarnings", "There were warnings during the build, see the log window for details");
	}
}

void FCameraRigAssetEditorToolkit::PostInitAssetEditor()
{
	const FCameraRigAssetEditorCommands& Commands = FCameraRigAssetEditorCommands::Get();

	ToolkitCommands->MapAction(
		Commands.Build,
		FExecuteAction::CreateSP(this, &FCameraRigAssetEditorToolkit::OnBuild));

	ToolkitCommands->MapAction(
		Commands.FindInCameraRig,
		FExecuteAction::CreateSP(this, &FCameraRigAssetEditorToolkit::OnFindInCameraRig));

	TSharedRef<SCameraRigAssetEditor> CameraRigEditor = CameraRigEditorWidget.ToSharedRef();

	ToolkitCommands->MapAction(
		Commands.FocusHome,
		FExecuteAction::CreateSP(CameraRigEditor, &SCameraRigAssetEditor::FocusHome));

	ToolkitCommands->MapAction(
		Commands.ShowNodeHierarchy,
		FExecuteAction::CreateSP(CameraRigEditor, &SCameraRigAssetEditor::SetEditorMode, ECameraRigAssetEditorMode::NodeGraph),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(CameraRigEditor, &SCameraRigAssetEditor::IsEditorMode, ECameraRigAssetEditorMode::NodeGraph));

	ToolkitCommands->MapAction(
		Commands.ShowTransitions,
		FExecuteAction::CreateSP(CameraRigEditor, &SCameraRigAssetEditor::SetEditorMode, ECameraRigAssetEditorMode::TransitionGraph),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(CameraRigEditor, &SCameraRigAssetEditor::IsEditorMode, ECameraRigAssetEditorMode::TransitionGraph));

	IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
	LiveEditManager = GameplayCamerasModule.GetLiveEditManager();
}

FText FCameraRigAssetEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Camera Rig Asset");
}

FName FCameraRigAssetEditorToolkit::GetToolkitFName() const
{
	static FName SequencerName("CameraRigAssetEditor");
	return SequencerName;
}

FString FCameraRigAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Camera Rig Asset ").ToString();
}

FLinearColor FCameraRigAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.7, 0.0f, 0.0f, 0.5f);
}

void FCameraRigAssetEditorToolkit::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	CameraRigAsset->BuildStatus = ECameraRigBuildStatus::Dirty;
}

void FCameraRigAssetEditorToolkit::OnBuild()
{
	using namespace UE::Cameras;

	FCameraRigAllocationInfo AllocationInfo;
	FCameraRigAllocationInfoBuilder CameraRigBuilder;
	CameraRigBuilder.BuildAllocationInfo(CameraRigAsset, AllocationInfo);

	CameraRigAsset->BuildStatus = ECameraRigBuildStatus::Clean;
	CameraRigAsset->AllocationInfo = AllocationInfo;

	FCameraRigPackages BuiltPackages;
	CameraRigAsset->GatherPackages(BuiltPackages);

	for (const UPackage* BuiltPackage : BuiltPackages)
	{
		LiveEditManager->NotifyPostBuildAsset(BuiltPackage);
	}
}

void FCameraRigAssetEditorToolkit::OnFindInCameraRig()
{
	TabManager->TryInvokeTab(SearchTabId);
	SearchWidget->FocusSearchEditBox();
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

