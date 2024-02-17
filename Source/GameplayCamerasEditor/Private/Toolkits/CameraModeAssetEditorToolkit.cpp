// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/CameraModeAssetEditorToolkit.h"

#include "AssetTools/CameraModeAssetEditor.h"
#include "Core/CameraMode.h"
#include "EditorModeManager.h"
#include "Framework/Docking/LayoutExtender.h"
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
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// Create the message log.
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions LogOptions;
	LogOptions.bShowPages = false;
	LogOptions.bShowFilters = false;
	LogOptions.bAllowClear = false;
	LogOptions.MaxPageCount = 1;
	StatsListing = MessageLogModule.CreateLogListing("CameraModeAssetEditorStats", LogOptions);

	Stats = MessageLogModule.CreateLogListingWidget(StatsListing.ToSharedRef());}

void FCameraModeAssetEditorToolkit::PostInitAssetEditor()
{
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

#undef LOCTEXT_NAMESPACE

