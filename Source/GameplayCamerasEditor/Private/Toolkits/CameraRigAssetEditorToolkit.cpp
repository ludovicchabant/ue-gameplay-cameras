// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/CameraRigAssetEditorToolkit.h"

#include "AssetTools/CameraRigAssetEditor.h"
#include "Commands/CameraRigAssetEditorCommands.h"
#include "Core/CameraRigAllocationInfoBuilder.h"
#include "Core/CameraRigAsset.h"
#include "EditorModeManager.h"
#include "Framework/Docking/LayoutExtender.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasModule.h"
#include "IMessageLogListing.h"
#include "MessageLogInitializationOptions.h"
#include "MessageLogModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "CameraRigAssetEditorToolkit"

namespace UE::Cameras
{

const FName FCameraRigAssetEditorToolkit::DetailsViewTabId(TEXT("CameraRigAssetEditor_DetailsView"));

FCameraRigAssetEditorToolkit::FCameraRigAssetEditorToolkit(UCameraRigAssetEditor* InOwningAssetEditor)
	: FBaseAssetToolkit(InOwningAssetEditor)
	, CommandBindings(new FUICommandList())
	, CameraRigAsset(InOwningAssetEditor->GetCameraRigAsset())
{
	// Override base class default layout.
	StandaloneDefaultLayout = FTabManager::NewLayout("CameraRigAssetEditor_Layout")
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

FCameraRigAssetEditorToolkit::~FCameraRigAssetEditorToolkit()
{
}

void FCameraRigAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(DetailsViewTabId, FOnSpawnTab::CreateSP(this, &FCameraRigAssetEditorToolkit::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("Details", "Details"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FCameraRigAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

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

	// Create the message log.
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions LogOptions;
	LogOptions.bShowPages = false;
	LogOptions.bShowFilters = false;
	LogOptions.bAllowClear = false;
	LogOptions.MaxPageCount = 1;
	StatsListing = MessageLogModule.CreateLogListing("CameraRigAssetEditorStats", LogOptions);

	Stats = MessageLogModule.CreateLogListingWidget(StatsListing.ToSharedRef());
}

void FCameraRigAssetEditorToolkit::RegisterToolbar()
{
	FName ParentName;
	const FName MenuName = GetToolMenuToolbarName(ParentName);
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus->IsMenuRegistered(MenuName))
	{
		const FCameraRigAssetEditorCommands& Commands = FCameraRigAssetEditorCommands::Get();

		FToolMenuOwnerScoped ToolMenuOwnerScope(this);

		UToolMenu* ToolBarMenu = UToolMenus::Get()->RegisterMenu(
				MenuName, ParentName, EMultiBoxType::ToolBar);

		FToolMenuSection& ToolBarSection = ToolBarMenu->FindOrAddSection("Build");
		FToolMenuEntry BuildButton = FToolMenuEntry::InitToolBarButton(Commands.Build);
		BuildButton.Icon = TAttribute<FSlateIcon>(
				SharedThis(this), &FCameraRigAssetEditorToolkit::GetBuildButtonIcon);
		BuildButton.ToolTip = TAttribute<FText>(
				SharedThis(this), &FCameraRigAssetEditorToolkit::GetBuildButtonTooltip);
		ToolBarSection.AddEntry(BuildButton);
	}
}

FSlateIcon FCameraRigAssetEditorToolkit::GetBuildButtonIcon() const
{
	static const FName BuildStatusBackground("Blueprint.CompileStatus.Background");
	static const FName BuildStatusUnknown("Blueprint.CompileStatus.Overlay.Unknown");
	static const FName BuildStatusError("Blueprint.CompileStatus.Overlay.Error");
	static const FName BuildStatusGood("Blueprint.CompileStatus.Overlay.Good");
	static const FName BuildStatusWarning("Blueprint.CompileStatus.Overlay.Warning");

	switch (CameraRigAsset->BuildStatus)
	{
		default:
		case ECameraRigBuildStatus::Dirty:
			return FSlateIcon(
					FAppStyle::GetAppStyleSetName(), BuildStatusBackground, 
					NAME_None, BuildStatusUnknown);
		case ECameraRigBuildStatus::WithErrors:
			return FSlateIcon(
					FAppStyle::GetAppStyleSetName(), BuildStatusBackground, 
					NAME_None, BuildStatusError);
		case ECameraRigBuildStatus::Clean:
			return FSlateIcon(
					FAppStyle::GetAppStyleSetName(), BuildStatusBackground, 
					NAME_None, BuildStatusGood);
		case ECameraRigBuildStatus::CleanWithWarnings:
			return FSlateIcon(
					FAppStyle::GetAppStyleSetName(), BuildStatusBackground, 
					NAME_None, BuildStatusWarning);
	}
}

FText FCameraRigAssetEditorToolkit::GetBuildButtonTooltip() const
{
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

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

