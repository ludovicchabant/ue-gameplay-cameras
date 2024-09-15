// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debugger/SGameplayCamerasDebugger.h"

#include "Commands/GameplayCamerasDebuggerCommands.h"
#include "Debug/CameraDebugColors.h"
#include "Debug/RootCameraDebugBlock.h"
#include "Debugger/SDebugCategoryButton.h"
#include "Debugger/SDebugWidgetUtils.h"
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
#include "ToolMenuDelegates.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SNullWidget.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SGameplayCamerasDebugger)

#define LOCTEXT_NAMESPACE "GameplayCamerasDebugger"

namespace UE::Cameras
{

const FName SGameplayCamerasDebugger::WindowName(TEXT("GameplayCamerasDebugger"));
const FName SGameplayCamerasDebugger::MenubarName(TEXT("GameplayCamerasDebugger.Menubar"));
const FName SGameplayCamerasDebugger::ToolbarName(TEXT("GameplayCamerasDebugger.Toolbar"));

void SGameplayCamerasDebugger::RegisterTabSpawners()
{
	TSharedRef<FGameplayCamerasEditorStyle> CamerasEditorStyle = FGameplayCamerasEditorStyle::Get();

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		SGameplayCamerasDebugger::WindowName,
		FOnSpawnTab::CreateStatic(&SGameplayCamerasDebugger::SpawnGameplayCamerasDebugger)
	)
	.SetDisplayName(LOCTEXT("TabDisplayName", "Cameras Debugger"))
	.SetTooltipText(LOCTEXT("TabTooltipText", "Open the Cameras Debugger tab."))
	.SetIcon(FSlateIcon(CamerasEditorStyle->GetStyleSetName(), "Debugger.TabIcon"))
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
	TSharedRef<FGameplayCamerasEditorStyle> GameplayCamerasEditorStyle = FGameplayCamerasEditorStyle::Get();
	GameplayCamerasEditorStyleName = GameplayCamerasEditorStyle->GetStyleSetName();

	InitializeColorSchemeNames();

	// Setup commands.
	const FGameplayCamerasDebuggerCommands& Commands = FGameplayCamerasDebuggerCommands::Get();
	TSharedRef<FUICommandList> CommandList = MakeShareable(new FUICommandList);
	CommandList->MapAction(
			Commands.EnableDebugInfo,
			FExecuteAction::CreateLambda([]() { GGameplayCamerasDebugEnable = !GGameplayCamerasDebugEnable; }),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([]() { return GGameplayCamerasDebugEnable; }));

	// Build all UI elements.
	TSharedRef<SWidget> MenubarContents = ConstructMenubar();
	TSharedRef<SWidget> ToolbarContents = ConstructToolbar(CommandList);
	TSharedRef<SWidget> GeneralOptionsContents = ConstructGeneralOptions(CommandList);
	ConstructDebugPanels();

	// Main layout.
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				MenubarContents
			]
		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				ToolbarContents
			]
		+ SVerticalBox::Slot()
			.Padding(2.0)
			[
				SAssignNew(PanelHost, SBox)
					.Padding(8.f)
					[
						EmptyPanel.ToSharedRef()
					]
			]
		+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0)
			[
				GeneralOptionsContents
			]
	];

	// Set initial debug panel.
	TArray<FStringView, TInlineAllocator<4>> ActiveCategories;
	UE::String::ParseTokens(GGameplayCamerasDebugCategories, ',', ActiveCategories);
	if (!ActiveCategories.IsEmpty())
	{
		SetActiveDebugCategoryPanel(FString(ActiveCategories[0]));
	}
}

void SGameplayCamerasDebugger::InitializeColorSchemeNames()
{
	TArray<FString> RawNames;
	FCameraDebugColors::GetColorSchemeNames(RawNames);
	for (const FString& RawName :RawNames)
	{
		ColorSchemeNames.Add(MakeShared<FString>(RawName));
	}
}

SGameplayCamerasDebugger* SGameplayCamerasDebugger::FromContext(UToolMenu* InMenu)
{
	UGameplayCamerasDebuggerMenuContext* Context = InMenu->FindContext<UGameplayCamerasDebuggerMenuContext>();
	if (ensure(Context))
	{
		TSharedPtr<SGameplayCamerasDebugger> This = Context->CamerasDebugger.Pin();
		return This.Get();
	}
	return nullptr;
}

TSharedRef<SWidget> SGameplayCamerasDebugger::ConstructMenubar()
{
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus->IsMenuRegistered(SGameplayCamerasDebugger::MenubarName))
	{
		FToolMenuOwnerScoped ToolMenuOwnerScope(this);

		UToolMenu* Menubar = ToolMenus->RegisterMenu(
				SGameplayCamerasDebugger::MenubarName, NAME_None, EMultiBoxType::MenuBar);
	}

	FToolMenuContext MenubarContext;
	return ToolMenus->GenerateWidget(SGameplayCamerasDebugger::MenubarName, MenubarContext);
}

TSharedRef<SWidget> SGameplayCamerasDebugger::ConstructToolbar(TSharedRef<FUICommandList> InCommandList)
{
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus->IsMenuRegistered(SGameplayCamerasDebugger::ToolbarName))
	{
		FToolMenuOwnerScoped ToolMenuOwnerScope(this);

		UToolMenu* Toolbar = ToolMenus->RegisterMenu(
				SGameplayCamerasDebugger::ToolbarName, NAME_None, EMultiBoxType::SlimHorizontalToolBar);

		Toolbar->AddDynamicSection(TEXT("Main"), FNewToolMenuDelegate::CreateLambda(
			[](UToolMenu* InMenu)
			{
				const FGameplayCamerasDebuggerCommands& Commands = FGameplayCamerasDebuggerCommands::Get();
				SGameplayCamerasDebugger* This = SGameplayCamerasDebugger::FromContext(InMenu);

				FToolMenuSection& MainSection = InMenu->AddSection(TEXT("Main"));

				FToolMenuEntry ToggleDebugInfo = FToolMenuEntry::InitToolBarButton(
						Commands.EnableDebugInfo,
						TAttribute<FText>::CreateSP(This, &SGameplayCamerasDebugger::GetToggleDebugDrawText),
						TAttribute<FText>(),
						TAttribute<FSlateIcon>::CreateSP(This, &SGameplayCamerasDebugger::GetToggleDebugDrawIcon));
				MainSection.AddEntry(ToggleDebugInfo);
			}));
	
		Toolbar->AddDynamicSection(TEXT("DebugCategories"), FNewToolMenuDelegate::CreateLambda(
			[](UToolMenu* InMenu)
			{
				IGameplayCamerasEditorModule& ThisModule = FModuleManager::GetModuleChecked<IGameplayCamerasEditorModule>(
						TEXT("GameplayCamerasEditor"));
				TArray<FCameraDebugCategoryInfo> RegisteredDebugCategories;
				ThisModule.GetRegisteredDebugCategories(RegisteredDebugCategories);

				SGameplayCamerasDebugger* This = SGameplayCamerasDebugger::FromContext(InMenu);

				FToolMenuSection& DebugCategoriesSection = InMenu->AddSection(TEXT("DebugCategories"));

				for (const FCameraDebugCategoryInfo& DebugCategory : RegisteredDebugCategories)
				{
					FToolMenuEntry ToggleDebugCategory = FToolMenuEntry::InitToolBarButton(
						FName(DebugCategory.Name),
						FUIAction(
							FExecuteAction::CreateSP(This, &SGameplayCamerasDebugger::SetActiveDebugCategoryPanel, DebugCategory.Name),
							FCanExecuteAction(),
							FIsActionChecked::CreateStatic(&SGameplayCamerasDebugger::IsDebugCategoryActive, DebugCategory.Name)),
						DebugCategory.DisplayText,
						DebugCategory.ToolTipText,
						DebugCategory.IconImage,
						EUserInterfaceActionType::ToggleButton);
					DebugCategoriesSection.AddEntry(ToggleDebugCategory);
				}
			}));
	}

	UGameplayCamerasDebuggerMenuContext* ThisContextWrapper = NewObject<UGameplayCamerasDebuggerMenuContext>();
	ThisContextWrapper->CamerasDebugger = SharedThis(this);
	FToolMenuContext ToolbarContext(InCommandList, TSharedPtr<FExtender>());
	ToolbarContext.AddObject(ThisContextWrapper);

	return ToolMenus->GenerateWidget(SGameplayCamerasDebugger::ToolbarName, ToolbarContext);
}

TSharedRef<SWidget> SGameplayCamerasDebugger::ConstructGeneralOptions(TSharedRef<FUICommandList> InCommandList)
{
	const ISlateStyle& AppStyle = FAppStyle::Get();
	const FMargin GridCellPadding(4.f);

	return SNew(SExpandableArea)
		.BorderImage(AppStyle.GetBrush("Brushes.Header"))
		.BodyBorderImage(AppStyle.GetBrush("Brushes.Recessed"))
		.HeaderPadding(FMargin(4.0f))
		.Padding(FMargin(0, 1, 0, 0))
		.InitiallyCollapsed(true)
		.AllowAnimatedTransition(false)
		.HeaderContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
						.Text(LOCTEXT("GeneralOptions", "General Options"))
						.TextStyle(AppStyle, "ButtonText")
						.Font(AppStyle.GetFontStyle("NormalFontBold"))
				]
		]
		.BodyContent()
		[
			SNew(SBorder)
				.BorderImage(AppStyle.GetBrush("Brushes.Header"))
				.Padding(2.f)
				[
					SNew(SGridPanel)
						.FillColumn(0, 1.f)
						.FillColumn(2, 1.f)
					+ SGridPanel::Slot(0, 0)
					.Padding(GridCellPadding)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
							.Text(LOCTEXT("TopMargin", "Top margin"))
					]
					+ SGridPanel::Slot(1, 0)
					.Padding(GridCellPadding)
					.VAlign(VAlign_Center)
					[
						SDebugWidgetUtils::CreateConsoleVariableSpinBox(TEXT("GameplayCameras.Debug.TopMargin"))
					]
					+ SGridPanel::Slot(0, 1)
					.Padding(GridCellPadding)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
							.Text(LOCTEXT("LeftMargin", "Left margin"))
					]
					+ SGridPanel::Slot(1, 1)
					.Padding(GridCellPadding)
					.VAlign(VAlign_Center)
					[
						SDebugWidgetUtils::CreateConsoleVariableSpinBox(TEXT("GameplayCameras.Debug.LeftMargin"))
					]
					+ SGridPanel::Slot(0, 2)
					.Padding(GridCellPadding)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
							.Text(LOCTEXT("InnerMargin", "Inner margin"))
					]
					+ SGridPanel::Slot(1, 2)
					.Padding(GridCellPadding)
					.VAlign(VAlign_Center)
					[
						SDebugWidgetUtils::CreateConsoleVariableSpinBox(TEXT("GameplayCameras.Debug.InnerMargin"))
					]
					+ SGridPanel::Slot(0, 3)
					.Padding(GridCellPadding)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
							.Text(LOCTEXT("IndentSize", "Indent size"))
					]
					+ SGridPanel::Slot(1, 3)
					.Padding(GridCellPadding)
					.VAlign(VAlign_Center)
					[
						SDebugWidgetUtils::CreateConsoleVariableSpinBox(TEXT("GameplayCameras.Debug.Indent"))
					]
					+ SGridPanel::Slot(2, 0)
					.Padding(GridCellPadding)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
							.Text(LOCTEXT("ColorScheme", "Color scheme"))
					]
					+ SGridPanel::Slot(3, 0)
					.Padding(GridCellPadding)
					.VAlign(VAlign_Center)
					[
						SDebugWidgetUtils::CreateConsoleVariableComboBox(TEXT("GameplayCameras.Debug.ColorScheme"), &ColorSchemeNames)
					]
				]
		];
}

void SGameplayCamerasDebugger::ConstructDebugPanels()
{
	// Empty panel.
	EmptyPanel = SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				.Text(LOCTEXT("EmptyPanelWarning", "No custom controls for this debug category."))
		];

	// Register custom panels.
	IGameplayCamerasEditorModule& ThisModule = FModuleManager::GetModuleChecked<IGameplayCamerasEditorModule>(
			TEXT("GameplayCamerasEditor"));
	TArray<FCameraDebugCategoryInfo> RegisteredDebugCategories;
	ThisModule.GetRegisteredDebugCategories(RegisteredDebugCategories);

	for (const FCameraDebugCategoryInfo& DebugCategory : RegisteredDebugCategories)
	{
		TSharedPtr<SWidget> DebugCategoryPanel = ThisModule.CreateDebugCategoryPanel(DebugCategory.Name);
		if (DebugCategoryPanel.IsValid())
		{
			DebugPanels.Add(DebugCategory.Name, DebugCategoryPanel);
		}
		else
		{
			// If there aren't any special UI controls for this category, use an empty panel.
			DebugPanels.Add(DebugCategory.Name, EmptyPanel);
		}
	}
}

FText SGameplayCamerasDebugger::GetToggleDebugDrawText() const
{
	if (GGameplayCamerasDebugEnable)
	{
		return LOCTEXT("DebugInfoEnabled", "Debug Info Enabled");
	}
	else
	{
		return LOCTEXT("DebugInfoDisabled", "Debug Info Disabled");
	}
}

FSlateIcon SGameplayCamerasDebugger::GetToggleDebugDrawIcon() const
{
	if (GGameplayCamerasDebugEnable)
	{
		return FSlateIcon(GameplayCamerasEditorStyleName, "Debugger.DebugInfoEnabled.Icon");
	}
	else
	{
		return FSlateIcon(GameplayCamerasEditorStyleName, "Debugger.DebugInfoDisabled.Icon");
	}
}

bool SGameplayCamerasDebugger::IsDebugCategoryActive(FString InCategoryName)
{
	TArray<FStringView, TInlineAllocator<4>> ActiveCategories;
	UE::String::ParseTokens(GGameplayCamerasDebugCategories, ',', ActiveCategories);
	return ActiveCategories.Contains(InCategoryName);
}

void SGameplayCamerasDebugger::SetActiveDebugCategoryPanel(FString InCategoryName)
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

