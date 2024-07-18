// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/CameraRigAssetEditorToolkit.h"

#include "Commands/CameraRigAssetEditorCommands.h"
#include "Core/CameraBuildLog.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetBuilder.h"
#include "Editors/CameraNodeGraphSchema.h"
#include "Editors/CameraRigTransitionGraphSchema.h"
#include "Editors/SCameraRigAssetEditor.h"
#include "Editors/SFindInObjectTreeGraph.h"
#include "Framework/Docking/LayoutExtender.h"
#include "Framework/Docking/TabManager.h"
#include "IGameplayCamerasEditorModule.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasModule.h"
#include "Modules/ModuleManager.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "ToolMenus.h"
#include "Toolkits/BuildButtonToolkit.h"
#include "Toolkits/CameraBuildLogToolkit.h"
#include "Toolkits/CameraRigAssetEditorToolkitBase.h"
#include "Toolkits/StandardToolkitLayout.h"
#include "Widgets/Docking/SDockTab.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigAssetEditorToolkit)

#define LOCTEXT_NAMESPACE "CameraRigAssetEditorToolkit"

namespace UE::Cameras
{

const FName FCameraRigAssetEditorToolkit::SearchTabId(TEXT("CameraRigAssetEditor_Search"));
const FName FCameraRigAssetEditorToolkit::MessagesTabId(TEXT("CameraRigAssetEditor_Messages"));

FCameraRigAssetEditorToolkit::FCameraRigAssetEditorToolkit(UAssetEditor* InOwningAssetEditor)
	: FBaseAssetToolkit(InOwningAssetEditor)
{
	Impl = MakeShared<FCameraRigAssetEditorToolkitBase>(TEXT("CameraRigAssetEditor_Layout_v6"));
	BuildButtonToolkit = MakeShared<FBuildButtonToolkit>();
	BuildLogToolkit = MakeShared<FCameraBuildLogToolkit>();

	// Override base class default layout.
	TSharedPtr<FStandardToolkitLayout> StandardLayout = Impl->GetStandardLayout();
	{
		StandardLayout->AddBottomTab(SearchTabId);
		StandardLayout->AddBottomTab(MessagesTabId);
	}
	StandaloneDefaultLayout = StandardLayout->GetLayout();

	UClass* NodeGraphSchemaClass = UCameraNodeGraphSchema::StaticClass();
	UCameraNodeGraphSchema* DefaultNodeGraphSchema = Cast<UCameraNodeGraphSchema>(NodeGraphSchemaClass->GetDefaultObject());
	NodeGraphConfig = DefaultNodeGraphSchema->BuildGraphConfig();

	UClass* TransitionSchemaClass = UCameraRigTransitionGraphSchema::StaticClass();
	UCameraRigTransitionGraphSchema* DefaultTransitionGraphSchema = Cast<UCameraRigTransitionGraphSchema>(TransitionSchemaClass->GetDefaultObject());
	TransitionGraphConfig = DefaultTransitionGraphSchema->BuildGraphConfig();
}

void FCameraRigAssetEditorToolkit::SetCameraRigAsset(UCameraRigAsset* InCameraRig)
{
	Impl->SetCameraRigAsset(InCameraRig);
	BuildButtonToolkit->SetTarget(InCameraRig);
}

void FCameraRigAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	Impl->RegisterTabSpawners(InTabManager, AssetEditorTabsCategory);

	const FName CamerasStyleSetName = FGameplayCamerasEditorStyle::Get()->GetStyleSetName();

	InTabManager->RegisterTabSpawner(SearchTabId, FOnSpawnTab::CreateSP(this, &FCameraRigAssetEditorToolkit::SpawnTab_Search))
		.SetDisplayName(LOCTEXT("Search", "Search"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(CamerasStyleSetName, "CameraRigAssetEditor.Tabs.Search"));

	InTabManager->RegisterTabSpawner(MessagesTabId, FOnSpawnTab::CreateSP(this, &FCameraRigAssetEditorToolkit::SpawnTab_Messages))
		.SetDisplayName(LOCTEXT("Messages", "Messages"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(CamerasStyleSetName, "CameraRigAssetEditor.Tabs.Messages"));
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
			BuildLogToolkit->GetMessagesWidget().ToSharedRef()
		];

	return MessagesTab.ToSharedRef();
}

void FCameraRigAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	
	Impl->UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(SearchTabId);
	InTabManager->UnregisterTabSpawner(MessagesTabId);
}

void FCameraRigAssetEditorToolkit::CreateWidgets()
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab, and our base class
	// has its own details view in order to get a notify hook.
	// ...no up-call...

	RegisterToolbar();
	CreateEditorModeManager();
	LayoutExtender = MakeShared<FLayoutExtender>();

	// Now do our custom stuff.

	Impl->CreateWidgets();

	// We need to set this for our FBaseAssetToolkit parent because otherwise it crashes
	// unhappily in SetObjectsToEdit.
	DetailsView = Impl->GetDetailsView();

	// Create the search panel.
	TSharedPtr<SCameraRigAssetEditor> CameraRigEditorWidget = Impl->GetCameraRigAssetEditor();
	TArray<UEdGraph*> CameraRigGraphs;
	CameraRigEditorWidget->GetGraphs(CameraRigGraphs);
	SearchWidget = SNew(SFindInObjectTreeGraph)
		.OnGetRootObjectsToSearch(this, &FCameraRigAssetEditorToolkit::OnGetRootObjectsToSearch)
		.OnJumpToObjectRequested(this, &FCameraRigAssetEditorToolkit::OnJumpToObject);

	// Create the message log.
	BuildLogToolkit->Initialize("CameraRigAssetBuildMessages");
}

void FCameraRigAssetEditorToolkit::RegisterToolbar()
{
	FName ParentName;
	const FName MenuName = GetToolMenuToolbarName(ParentName);
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus->IsMenuRegistered(MenuName))
	{
		FToolMenuOwnerScoped ToolMenuOwnerScope(this);
		FToolMenuInsert InsertAfterAssetSection("Asset", EToolMenuInsertType::After);

		UToolMenu* ToolbarMenu = UToolMenus::Get()->RegisterMenu(
				MenuName, ParentName, EMultiBoxType::ToolBar);

		ToolbarMenu->AddDynamicSection("Tools", FNewToolMenuDelegate::CreateLambda(
				[](UToolMenu* InMenu)
				{
					UCameraRigAssetEditorMenuContext* Context = InMenu->FindContext<UCameraRigAssetEditorMenuContext>();
					FCameraRigAssetEditorToolkit* This = Context ? Context->Toolkit.Pin().Get() : nullptr;
					if (!ensure(This))
					{
						return;
					}

					const FCameraRigAssetEditorCommands& Commands = FCameraRigAssetEditorCommands::Get();

					FToolMenuSection& ToolsSection = InMenu->AddSection("Tools");
					ToolsSection.AddEntry(This->BuildButtonToolkit->MakeToolbarButton(Commands.Build));
					ToolsSection.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.FindInCameraRig));
				}),
				InsertAfterAssetSection);

		Impl->BuildToolbarMenu(ToolbarMenu);
	}
}

void FCameraRigAssetEditorToolkit::InitToolMenuContext(FToolMenuContext& MenuContext)
{
	FBaseAssetToolkit::InitToolMenuContext(MenuContext);

	UCameraRigAssetEditorMenuContext* Context = NewObject<UCameraRigAssetEditorMenuContext>();
	Context->Toolkit = SharedThis(this);
	MenuContext.AddObject(Context);
}

void FCameraRigAssetEditorToolkit::PostInitAssetEditor()
{
	Impl->BindCommands(ToolkitCommands);

	const FCameraRigAssetEditorCommands& Commands = FCameraRigAssetEditorCommands::Get();

	ToolkitCommands->MapAction(
		Commands.Build,
		FExecuteAction::CreateSP(this, &FCameraRigAssetEditorToolkit::OnBuild));

	ToolkitCommands->MapAction(
		Commands.FindInCameraRig,
		FExecuteAction::CreateSP(this, &FCameraRigAssetEditorToolkit::OnFindInCameraRig));

	BuildLogToolkit->OnRequestJumpToObject().BindSPLambda(this, [this](UObject* Object)
		{
			TSharedPtr<SCameraRigAssetEditor> CameraRigEditorWidget = Impl->GetCameraRigAssetEditor();
			CameraRigEditorWidget->FindAndJumpToObjectNode(Object);
		});

	IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
	LiveEditManager = GameplayCamerasModule.GetLiveEditManager();
}

void FCameraRigAssetEditorToolkit::OnBuild()
{
	UCameraRigAsset* CameraRigAsset = Impl->GetCameraRigAsset();
	if (!CameraRigAsset)
	{
		return;
	}

	FCameraBuildLog BuildLog;
	FCameraRigAssetBuilder Builder(BuildLog);
	Builder.BuildCameraRig(
			CameraRigAsset,
			FCameraRigAssetBuilder::FCustomBuildStep::CreateLambda(
				[](UCameraRigAsset* InCameraRigAsset, FCameraBuildLog& BuildLog)
				{
					IGameplayCamerasEditorModule& GameplayCamerasEditorModule = IGameplayCamerasEditorModule::Get();
					for (const FOnBuildCameraRigAsset& Builder : GameplayCamerasEditorModule.GetCameraRigAssetBuilders())
					{
						Builder.ExecuteIfBound(InCameraRigAsset, BuildLog);
					}
				}));

	BuildLogToolkit->PopulateMessageListing(BuildLog);

	if (CameraRigAsset->BuildStatus != ECameraBuildStatus::Clean)
	{
		TabManager->TryInvokeTab(MessagesTabId);
	}

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

void FCameraRigAssetEditorToolkit::OnGetRootObjectsToSearch(TArray<FFindInObjectTreeGraphSource>& OutSources)
{
	UCameraRigAsset* CameraRig = Impl->GetCameraRigAsset();
	OutSources.Add(FFindInObjectTreeGraphSource{ CameraRig, &NodeGraphConfig });
	OutSources.Add(FFindInObjectTreeGraphSource{ CameraRig, &TransitionGraphConfig });
}

void FCameraRigAssetEditorToolkit::OnJumpToObject(UObject* Object, FName PropertyName)
{
	TSharedPtr<SCameraRigAssetEditor> CameraRigEditor = Impl->GetCameraRigAssetEditor();
	CameraRigEditor->FindAndJumpToObjectNode(Object);
}

FText FCameraRigAssetEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Camera Rig Asset");
}

FName FCameraRigAssetEditorToolkit::GetToolkitFName() const
{
	static FName ToolkitName("CameraRigAssetEditor");
	return ToolkitName;
}

FString FCameraRigAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Camera Rig Asset ").ToString();
}

FLinearColor FCameraRigAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.7f, 0.0f, 0.0f, 0.5f);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

