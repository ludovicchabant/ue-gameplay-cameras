// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debugger/SGameplayCamerasDebugger.h"

#include "Commands/GameplayCamerasDebuggerCommands.h"
#include "Debug/RootCameraDebugBlock.h"
#include "Debugger/SDebugCategoryButton.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Docking/LayoutService.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IGameplayCamerasEditorModule.h"
#include "Modules/ModuleManager.h"
#include "String/ParseTokens.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Styling/SlateTypes.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SNullWidget.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#define LOCTEXT_NAMESPACE "GameplayCamerasDebugger"

namespace UE::Cameras
{

const FName SGameplayCamerasDebugger::WindowName(TEXT("GameplayCamerasDebugger"));
const FName SGameplayCamerasDebugger::ToolbarName(TEXT("GameplayCamerasDebugger.Toolbar"));

void SGameplayCamerasDebugger::RegisterTabSpawners()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		SGameplayCamerasDebugger::WindowName,
		FOnSpawnTab::CreateStatic(&SGameplayCamerasDebugger::SpawnGameplayCamerasDebugger)
	)
	.SetDisplayName(LOCTEXT("TabDisplayName", "Cameras Debugger"))
	.SetTooltipText(LOCTEXT("TabTooltipText", "Open the Cameras Debugger tab."))
	.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsDebugCategory())
	.SetCanSidebarTab(false);
}

void SGameplayCamerasDebugger::UnregisterTabSpawners()
{
	if (FSlateApplication::IsInitialized())
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SGameplayCamerasDebugger::WindowName);
	}
}

TSharedRef<SDockTab> SGameplayCamerasDebugger::SpawnGameplayCamerasDebugger(const FSpawnTabArgs& Args)
{
	auto NomadTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("TabTitle", "Cameras Debugger"));

	TSharedRef<SWidget> MainWidget = SNew(SGameplayCamerasDebugger);
	NomadTab->SetContent(MainWidget);
	return NomadTab;
}

SGameplayCamerasDebugger::SGameplayCamerasDebugger()
{
}

SGameplayCamerasDebugger::~SGameplayCamerasDebugger()
{
}

void SGameplayCamerasDebugger::Construct(const FArguments& InArgs)
{
	IGameplayCamerasEditorModule& GameplayCamerasEditorModule = FModuleManager::GetModuleChecked<IGameplayCamerasEditorModule>(TEXT("GameplayCamerasEditor"));
	TSharedRef<FGameplayCamerasEditorStyle> GameplayCamerasEditorStyle = FGameplayCamerasEditorStyle::Get();
	const FName& GameplayCamerasEditorStyleName = GameplayCamerasEditorStyle->GetStyleSetName();

	// Setup commands.
	const FGameplayCamerasDebuggerCommands& Commands = FGameplayCamerasDebuggerCommands::Get();
	TSharedRef<FUICommandList> CommandList = MakeShareable(new FUICommandList);
	CommandList->MapAction(
			Commands.EnableDebugInfo,
			FExecuteAction::CreateLambda([]() { GGameplayCamerasDebugEnable = !GGameplayCamerasDebugEnable; }),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([]() { return GGameplayCamerasDebugEnable; }));

	// Main menu bar.
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus->IsMenuRegistered(SGameplayCamerasDebugger::ToolbarName))
	{
		FToolMenuOwnerScoped ToolMenuOwnerScope(this);

		UToolMenu* ToolBarMenu = ToolMenus->RegisterMenu(
				SGameplayCamerasDebugger::ToolbarName, NAME_None, EMultiBoxType::ToolBar);
	}
	FToolMenuContext ToolbarContext;
	TSharedRef<SWidget> ToolbarContents = ToolMenus->GenerateWidget(
			SGameplayCamerasDebugger::ToolbarName, ToolbarContext);

	// Empty panel.
	EmptyPanel = SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				.Text(LOCTEXT("EmptyPanelWarning", "No custom controls for this debug category."))
		];

	// Toolbar with debug category buttons.
	FToolBarBuilder DebugCategoriesToolbarBuilder(CommandList.ToSharedPtr(), FMultiBoxCustomization::None);
	DebugCategoriesToolbarBuilder.BeginSection(TEXT("Main"));
	{
		DebugCategoriesToolbarBuilder.AddToolBarButton(
				Commands.EnableDebugInfo,
				NAME_None,
				TAttribute<FText>(FText::FromString(TEXT(""))),
				TAttribute<FText>(),
				FSlateIcon(GameplayCamerasEditorStyleName, "Debugger.EnableDebugInfo.Icon"));
	}
	DebugCategoriesToolbarBuilder.EndSection();
	DebugCategoriesToolbarBuilder.BeginSection(TEXT("DebugCategories"));
	{
		TArray<FCameraDebugCategoryInfo> RegisteredDebugCategories;
		GameplayCamerasEditorModule.GetRegisteredDebugCategories(RegisteredDebugCategories);
		for (const FCameraDebugCategoryInfo& DebugCategory : RegisteredDebugCategories)
		{
			TSharedPtr<SWidget> DebugCategoryPanel = GameplayCamerasEditorModule.CreateDebugCategoryPanel(DebugCategory.Name);
			if (DebugCategoryPanel.IsValid())
			{
				DebugPanels.Add(DebugCategory.Name, DebugCategoryPanel);
			}
			else
			{
				// If there aren't any special UI controls for this category, use an empty panel.
				DebugPanels.Add(DebugCategory.Name, EmptyPanel);
			}

			TSharedRef<SDebugCategoryButton> DebugCategoryButton = SNew(SDebugCategoryButton)
				.DebugCategoryName(DebugCategory.Name)
				.DisplayText(DebugCategory.DisplayText)
				.ToolTipText(DebugCategory.ToolTipText)
				.IconImage(DebugCategory.IconImage.GetIcon())
				.IsDebugCategoryActive_Lambda([DebugCategory](){ return SGameplayCamerasDebugger::IsDebugCategoryActive(DebugCategory.Name); })
				.RequestDebugCategoryChange(this, &SGameplayCamerasDebugger::SetActiveDebugCategoryPanel);
			DebugCategoriesToolbarBuilder.AddToolBarWidget(DebugCategoryButton);
		}
	}
	DebugCategoriesToolbarBuilder.EndSection();

	// Main layout.
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				ToolbarContents
			]
		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				DebugCategoriesToolbarBuilder.MakeWidget()
			]
		+ SVerticalBox::Slot()
		.Padding(2.0)
			[
				SAssignNew(PanelHost, SBox)
				[
					SNullWidget::NullWidget
				]
			]
	];
}

bool SGameplayCamerasDebugger::IsDebugCategoryActive(const FString& InCategoryName)
{
	TArray<FStringView, TInlineAllocator<4>> ActiveCategories;
	UE::String::ParseTokens(GGameplayCamerasDebugCategories, ',', ActiveCategories);
	return ActiveCategories.Contains(InCategoryName);
}

void SGameplayCamerasDebugger::SetActiveDebugCategoryPanel(const FString& InCategoryName)
{
	if (ensureMsgf(
				DebugPanels.Contains(InCategoryName), 
				TEXT("Debug category was not registered with IGameplayCamerasEditorModule: %s"), *InCategoryName))
	{
		TSharedPtr<SWidget> DebugPanel = DebugPanels.FindChecked(InCategoryName);
		check(DebugPanel.IsValid());
		PanelHost->SetContent(DebugPanel.ToSharedRef());

		GGameplayCamerasDebugCategories = InCategoryName;
	}
	else
	{
		PanelHost->SetContent(SNullWidget::NullWidget);
	}
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

