// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/CameraModeAssetEditorToolkit.h"

#include "AssetTools/CameraModeAssetEditor.h"
#include "Commands/CameraModeAssetEditorCommands.h"
#include "Core/CameraMode.h"
#include "EditorModeManager.h"
#include "Framework/Docking/LayoutExtender.h"
#include "IGameplayCamerasModule.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "IMessageLogListing.h"
#include "MessageLogInitializationOptions.h"
#include "MessageLogModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "CameraModeAssetEditorToolkit"

const FName FCameraModeAssetEditorToolkit::DetailsViewTabId(TEXT("CameraModeAssetEditor_DetailsView"));

FCameraModeAssetEditorToolkit::FCameraModeAssetEditorToolkit(UCameraModeAssetEditor* InOwningAssetEditor)
	: FBaseAssetToolkit(InOwningAssetEditor)
	, CommandBindings(new FUICommandList())
	, CameraModeAsset(InOwningAssetEditor->GetCameraModeAsset())
{
	// Override base class default layout.
	StandaloneDefaultLayout = FTabManager::NewLayout("CameraModeAssetEditor_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(DetailsViewTabId, ETabState::OpenedTab)
					->SetForegroundTab(DetailsViewTabId)
				)
			)
		);
}

FCameraModeAssetEditorToolkit::~FCameraModeAssetEditorToolkit()
{
}

void FCameraModeAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(DetailsViewTabId, FOnSpawnTab::CreateSP(this, &FCameraModeAssetEditorToolkit::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("Details", "Details"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FCameraModeAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(DetailsViewTabId);
}

void FCameraModeAssetEditorToolkit::CreateWidgets()
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

	// Create the message log.
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions LogOptions;
	LogOptions.bShowPages = false;
	LogOptions.bShowFilters = false;
	LogOptions.bAllowClear = false;
	LogOptions.MaxPageCount = 1;
	StatsListing = MessageLogModule.CreateLogListing("CameraModeAssetEditorStats", LogOptions);

	Stats = MessageLogModule.CreateLogListingWidget(StatsListing.ToSharedRef());
}

void FCameraModeAssetEditorToolkit::RegisterToolbar()
{
	FName ParentName;
	const FName MenuName = GetToolMenuToolbarName(ParentName);
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus->IsMenuRegistered(MenuName))
	{
		const FCameraModeAssetEditorCommands& Commands = FCameraModeAssetEditorCommands::Get();

		FToolMenuOwnerScoped ToolMenuOwnerScope(this);

		UToolMenu* ToolBarMenu = UToolMenus::Get()->RegisterMenu(
				MenuName, ParentName, EMultiBoxType::ToolBar);

		FToolMenuSection& ToolBarSection = ToolBarMenu->FindOrAddSection("Build");
		FToolMenuEntry BuildButton = FToolMenuEntry::InitToolBarButton(Commands.Build);
		BuildButton.Icon = TAttribute<FSlateIcon>(
				SharedThis(this), &FCameraModeAssetEditorToolkit::GetBuildButtonIcon);
		BuildButton.ToolTip = TAttribute<FText>(
				SharedThis(this), &FCameraModeAssetEditorToolkit::GetBuildButtonTooltip);
		ToolBarSection.AddEntry(BuildButton);
	}
}

FSlateIcon FCameraModeAssetEditorToolkit::GetBuildButtonIcon() const
{
	static const FName BuildStatusBackground("Blueprint.CompileStatus.Background");
	static const FName BuildStatusUnknown("Blueprint.CompileStatus.Overlay.Unknown");
	static const FName BuildStatusError("Blueprint.CompileStatus.Overlay.Error");
	static const FName BuildStatusGood("Blueprint.CompileStatus.Overlay.Good");
	static const FName BuildStatusWarning("Blueprint.CompileStatus.Overlay.Warning");

	switch (CameraModeAsset->BuildStatus)
	{
		default:
		case ECameraModeBuildStatus::Dirty:
			return FSlateIcon(
					FAppStyle::GetAppStyleSetName(), BuildStatusBackground, 
					NAME_None, BuildStatusUnknown);
		case ECameraModeBuildStatus::WithErrors:
			return FSlateIcon(
					FAppStyle::GetAppStyleSetName(), BuildStatusBackground, 
					NAME_None, BuildStatusError);
		case ECameraModeBuildStatus::Clean:
			return FSlateIcon(
					FAppStyle::GetAppStyleSetName(), BuildStatusBackground, 
					NAME_None, BuildStatusGood);
		case ECameraModeBuildStatus::CleanWithWarnings:
			return FSlateIcon(
					FAppStyle::GetAppStyleSetName(), BuildStatusBackground, 
					NAME_None, BuildStatusWarning);
	}
}

FText FCameraModeAssetEditorToolkit::GetBuildButtonTooltip() const
{
	switch (CameraModeAsset->BuildStatus)
	{
		default:
		case ECameraModeBuildStatus::Dirty:
			return LOCTEXT("BuildButtonStatusDirty", "Dirty or unknown, should rebuild");
		case ECameraModeBuildStatus::WithErrors:
			return LOCTEXT("BuildButtonStatusWithErrors", "There were errors during the build, see the log window for details");
		case ECameraModeBuildStatus::Clean:
			return LOCTEXT("BuildButtonStatusClean", "Good to go");
		case ECameraModeBuildStatus::CleanWithWarnings:
			return LOCTEXT("BuildButtonStatusCleanWithWarnings", "There were warnings during the build, see the log window for details");
	}
}

void FCameraModeAssetEditorToolkit::PostInitAssetEditor()
{
	const FCameraModeAssetEditorCommands& Commands = FCameraModeAssetEditorCommands::Get();

	ToolkitCommands->MapAction(
		Commands.Build,
		FExecuteAction::CreateSP(this, &FCameraModeAssetEditorToolkit::OnBuild));

	IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
	LiveEditManager = GameplayCamerasModule.GetLiveEditManager();
}

FText FCameraModeAssetEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Camera Mode Asset");
}

FName FCameraModeAssetEditorToolkit::GetToolkitFName() const
{
	static FName SequencerName("CameraModeAssetEditor");
	return SequencerName;
}

FString FCameraModeAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Camera Mode Asset ").ToString();
}

FLinearColor FCameraModeAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.7, 0.0f, 0.0f, 0.5f);
}

void FCameraModeAssetEditorToolkit::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	CameraModeAsset->BuildStatus = ECameraModeBuildStatus::Dirty;
}

void FCameraModeAssetEditorToolkit::OnBuild()
{
	CameraModeAsset->BuildStatus = ECameraModeBuildStatus::Clean;

	FCameraModePackages BuiltPackages;
	CameraModeAsset->GatherPackages(BuiltPackages);

	for (const UPackage* BuiltPackage : BuiltPackages)
	{
		LiveEditManager->NotifyPostBuildAsset(BuiltPackage);
	}
}

#undef LOCTEXT_NAMESPACE

