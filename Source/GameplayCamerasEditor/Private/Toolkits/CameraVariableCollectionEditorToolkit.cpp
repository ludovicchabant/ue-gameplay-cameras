// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/CameraVariableCollectionEditorToolkit.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetTools/CameraVariableCollectionEditor.h"
#include "Commands/CameraVariableCollectionEditorCommands.h"
#include "ContentBrowserModule.h"
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableCollection.h"
#include "Editors/SCameraVariableCollectionEditor.h"
#include "Framework/Docking/LayoutExtender.h"
#include "Framework/Docking/TabManager.h"
#include "IContentBrowserSingleton.h"
#include "Misc/FeedbackContext.h"
#include "Modules/ModuleManager.h"
#include "ObjectTools.h"
#include "PropertyEditorModule.h"
#include "PropertyPath.h"
#include "ScopedTransaction.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "ToolMenus.h"
#include "UObject/UObjectIterator.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraVariableCollectionEditorToolkit)

#define LOCTEXT_NAMESPACE "CameraVariableCollectionEditorToolkit"

namespace UE::Cameras
{

const FName FCameraVariableCollectionEditorToolkit::VariableCollectionEditorTabId(TEXT("CameraVariableCollectionEditor_VariableCollectionEditor"));
const FName FCameraVariableCollectionEditorToolkit::DetailsViewTabId(TEXT("CameraVariableCollectionEditor_DetailsView"));

class SDeleteVariableDialog : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SDeleteVariableDialog)
	{}
		SLATE_ARGUMENT(TWeakPtr<SWindow>, ParentWindow)
		SLATE_ARGUMENT(TSet<FName>, ReferencingPackages)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		WeakParentWindow = InArgs._ParentWindow;

		ReferencingPackages = InArgs._ReferencingPackages;

		ChildSlot
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.0f)
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor::Green)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Visibility(this, &SDeleteVariableDialog::GetNoReferencesVisibility)
				.Padding(5.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT(
								"VariablesOkToDelete", 
								"No assets reference the variables being deleted."))
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.0f)
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor::Red)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Visibility(this, &SDeleteVariableDialog::GetReferencesVisiblity)
				.Padding(5.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT(
								"VariablesPendingDeleteAreInUse", 
								"Some of the camera variables being deleted are referenced by camera assets."))
				]
			]

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(5.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(FMargin(0, 0, 0, 3))
				.Visibility(this, &SDeleteVariableDialog::GetReferencesVisiblity)
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop"))
						.BorderBackgroundColor(FLinearColor(.6, .6, .6, 1.0f))
						.Padding(3.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT(
										"AssetsReferencingVariablesPendingDelete", 
										"Assets Referencing the Camera Variables to Delete"))
							.Font(FAppStyle::GetFontStyle("BoldFont"))
							.ShadowOffset(FVector2D(1.0f, 1.0f))
						]
					]

					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					[
						SDeleteVariableDialog::BuildReferencerAssetPicker()
					]
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 4.f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(6, 0)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("NoBorder"))
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.Text(LOCTEXT("Delete", "Delete"))
						.ToolTipText(LOCTEXT("DeleteTooltipText", "Perform the delete"))
						.ButtonStyle(FAppStyle::Get(), "FlatButton.Danger")
						.TextStyle(FAppStyle::Get(), "FlatButton.DefaultTextStyle")
						.OnClicked(this, &SDeleteVariableDialog::OnDeleteClicked)
					]
				]

				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(6, 0)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("NoBorder"))
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.Text(LOCTEXT("Cancel", "Cancel"))
						.ToolTipText(LOCTEXT("CancelDeleteTooltipText", "Cancel the delete"))
						.ButtonStyle(FAppStyle::Get(), "FlatButton.Default")
						.TextStyle(FAppStyle::Get(), "FlatButton.DefaultTextStyle")
						.OnClicked(this, &SDeleteVariableDialog::OnCancelClicked)
					]
				]
			]
		];
	}

	bool ShouldPerformDelete() const
	{
		return bPerformDelete;
	}
	
protected:

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			OnCancelClicked();
			return FReply::Handled();
		}
		return FReply::Unhandled();
	}

private:

	EVisibility GetNoReferencesVisibility() const
	{
		return ReferencingPackages.IsEmpty() ? EVisibility::Visible : EVisibility::Collapsed;
	}

	EVisibility GetReferencesVisiblity() const
	{
		return ReferencingPackages.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
	}

	TSharedRef<SWidget> BuildReferencerAssetPicker()
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

		FARFilter ARFilter;
		ARFilter.PackageNames = ReferencingPackages.Array();

		FAssetPickerConfig AssetPickerConfig;
		AssetPickerConfig.bAllowDragging = false;
		AssetPickerConfig.bCanShowClasses = false;
		AssetPickerConfig.bAllowNullSelection = false;
		AssetPickerConfig.bShowBottomToolbar = false;
		AssetPickerConfig.bAutohideSearchBar = true;
		AssetPickerConfig.Filter = ARFilter;
		AssetPickerConfig.InitialAssetViewType = EAssetViewType::Tile;
		AssetPickerConfig.OnAssetsActivated = FOnAssetsActivated::CreateSP(this, &SDeleteVariableDialog::OnAssetsActivated);

		return ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig);
	}

	void OnAssetsActivated(const TArray<FAssetData>& ActivatedAssets, EAssetTypeActivationMethod::Type ActivationMethod)
	{
		if (ActivationMethod == EAssetTypeActivationMethod::DoubleClicked || ActivationMethod == EAssetTypeActivationMethod::Opened)
		{
			CloseWindow();

			for (const FAssetData& ActivatedAsset : ActivatedAssets)
			{
				GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(ActivatedAsset.GetAsset());
			}
		}
	}

	void CloseWindow()
	{
		if (TSharedPtr<SWindow> ParentWindow = WeakParentWindow.Pin())
		{
			ParentWindow->RequestDestroyWindow();
		}
	}

	FReply OnDeleteClicked()
	{
		bPerformDelete = true;
		CloseWindow();
		return FReply::Handled();
	}

	FReply OnCancelClicked()
	{
		bPerformDelete = false;
		CloseWindow();
		return FReply::Handled();
	}

private:

	TWeakPtr<SWindow> WeakParentWindow;

	TSet<FName> ReferencingPackages;

	bool bPerformDelete = false;
};

FCameraVariableCollectionEditorToolkit::FCameraVariableCollectionEditorToolkit(UCameraVariableCollectionEditor* InOwningAssetEditor)
	: FBaseAssetToolkit(InOwningAssetEditor)
	, VariableCollection(InOwningAssetEditor->GetVariableCollection())
	, CommandBindings(new FUICommandList())
{
	// Override base class default layout.
	StandaloneDefaultLayout = FTabManager::NewLayout("CameraVariableCollectionEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.8f)
				->SetHideTabWell(true)
				->AddTab(VariableCollectionEditorTabId, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.2f)
				->SetHideTabWell(true)
				->AddTab(DetailsViewTabId, ETabState::OpenedTab)
			)
		);
}

FCameraVariableCollectionEditorToolkit::~FCameraVariableCollectionEditorToolkit()
{
}

void FCameraVariableCollectionEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	const FName CamerasStyleSetName = FGameplayCamerasEditorStyle::Get()->GetStyleSetName();

	InTabManager->RegisterTabSpawner(VariableCollectionEditorTabId, FOnSpawnTab::CreateSP(this, &FCameraVariableCollectionEditorToolkit::SpawnTab_VariableCollectionEditor))
		.SetDisplayName(LOCTEXT("VariableCollectionEditor", "Camera Variable Collection"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef());

	InTabManager->RegisterTabSpawner(DetailsViewTabId, FOnSpawnTab::CreateSP(this, &FCameraVariableCollectionEditorToolkit::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("Details", "Details"))
		.SetGroup(AssetEditorTabsCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

TSharedRef<SDockTab> FCameraVariableCollectionEditorToolkit::SpawnTab_VariableCollectionEditor(const FSpawnTabArgs& Args)
{
	TSharedPtr<SDockTab> VariableCollectionEditorTab = SNew(SDockTab)
		.Label(LOCTEXT("VariableCollectionEditorTabTitle", "Camera Variable Collection"))
		[
			VariableCollectionEditorWidget.ToSharedRef()
		];

	return VariableCollectionEditorTab.ToSharedRef();
}

void FCameraVariableCollectionEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(VariableCollectionEditorTabId);
	InTabManager->UnregisterTabSpawner(DetailsViewTabId);
}

void FCameraVariableCollectionEditorToolkit::CreateWidgets()
{
	// Skip FBaseAssetToolkit here because we don't want a viewport tab.
	// ...no up-call...

	RegisterToolbar();
	LayoutExtender = MakeShared<FLayoutExtender>();

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	
	// Now do our custom stuff.

	// Create the variable collection editor.
	VariableCollectionEditorWidget = SNew(SCameraVariableCollectionEditor)
		.DetailsView(DetailsView)
		.VariableCollection(VariableCollection);
}

void FCameraVariableCollectionEditorToolkit::RegisterToolbar()
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
		const FCameraVariableCollectionEditorCommands& Commands = FCameraVariableCollectionEditorCommands::Get();
		const FName CamerasStyleSetName = FGameplayCamerasEditorStyle::Get()->GetStyleSetName();

		FToolMenuSection& VariablesSection = ToolbarMenu->AddSection("Variables", TAttribute<FText>(), InsertAfterAssetSection);

		FToolMenuEntry CreateVariableEntry = FToolMenuEntry::InitComboButton(
				"CreateVariable",
				FUIAction(),
				FNewToolMenuDelegate::CreateStatic(&FCameraVariableCollectionEditorToolkit::GenerateAddNewVariableMenu),
				LOCTEXT("CreateVariableCombo_Label", "Add"),
				LOCTEXT("CreateVariableCombo_ToolTip", "Add a new camera variable to the collection"),
				FSlateIcon(CamerasStyleSetName, "CameraVariableCollectionEditor.CreateVariable")
				);
		VariablesSection.AddEntry(CreateVariableEntry);

		FToolMenuEntry RenameVariableEntry = FToolMenuEntry::InitToolBarButton(Commands.RenameVariable);
		VariablesSection.AddEntry(RenameVariableEntry);

		FToolMenuEntry DeleteVariableEntry = FToolMenuEntry::InitToolBarButton(Commands.DeleteVariable);
		VariablesSection.AddEntry(DeleteVariableEntry);
	}
}

void FCameraVariableCollectionEditorToolkit::GenerateAddNewVariableMenu(UToolMenu* InMenu)
{
	UCameraVariableCollectionEditorMenuContext* Context = InMenu->FindContext<UCameraVariableCollectionEditorMenuContext>();
	if (!ensure(Context))
	{
		return;
	}

	FCameraVariableCollectionEditorToolkit* This = Context->EditorToolkit.Pin().Get();
	if (!ensure(This))
	{
		return;
	}

	const FCameraVariableCollectionEditorCommands& Commands = FCameraVariableCollectionEditorCommands::Get();
	FToolMenuSection& VariableTypesSection = InMenu->AddSection("VariableTypes");

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* VariableClass = *It;
		if (VariableClass->IsChildOf<UCameraVariableAsset>() &&
				!VariableClass->HasAnyClassFlags(CLASS_Abstract))
		{
			const FText VariableTypeDisplayName(VariableClass->GetDisplayNameText());
			VariableTypesSection.AddEntry(FToolMenuEntry::InitMenuEntry(
						FName(FString::Format(TEXT("AddCameraVariable_{0}"), { VariableClass->GetName() })),
						TAttribute<FText>(VariableTypeDisplayName),
						TAttribute<FText>(FText::Format(
								LOCTEXT("CreateVariableEntry_LabelFmt", "Add a {0} to the collection"), VariableTypeDisplayName)),
						TAttribute<FSlateIcon>(),
						FExecuteAction::CreateSP(
							This, &FCameraVariableCollectionEditorToolkit::OnCreateVariable, 
							TSubclassOf<UCameraVariableAsset>(VariableClass))
						));
		}
	}
}

void FCameraVariableCollectionEditorToolkit::InitToolMenuContext(FToolMenuContext& MenuContext)
{
	FBaseAssetToolkit::InitToolMenuContext(MenuContext);

	UCameraVariableCollectionEditorMenuContext* Context = NewObject<UCameraVariableCollectionEditorMenuContext>();
	Context->EditorToolkit = SharedThis(this);
	MenuContext.AddObject(Context);
}

void FCameraVariableCollectionEditorToolkit::PostInitAssetEditor()
{
	const FCameraVariableCollectionEditorCommands& Commands = FCameraVariableCollectionEditorCommands::Get();

	ToolkitCommands->MapAction(
		Commands.RenameVariable,
		FExecuteAction::CreateSP(this, &FCameraVariableCollectionEditorToolkit::OnRenameVariable),
		FCanExecuteAction::CreateSP(this, &FCameraVariableCollectionEditorToolkit::CanRenameVariable));

	ToolkitCommands->MapAction(
		Commands.DeleteVariable,
		FExecuteAction::CreateSP(this, &FCameraVariableCollectionEditorToolkit::OnDeleteVariable),
		FCanExecuteAction::CreateSP(this, &FCameraVariableCollectionEditorToolkit::CanDeleteVariable));
}

FText FCameraVariableCollectionEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Camera Variable Collection");
}

FName FCameraVariableCollectionEditorToolkit::GetToolkitFName() const
{
	static FName ToolkitName("CameraVariableCollectionEditor");
	return ToolkitName;
}

FString FCameraVariableCollectionEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Camera Variable Collection ").ToString();
}

FLinearColor FCameraVariableCollectionEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.1f, 0.8f, 0.2f, 0.5f);
}

void FCameraVariableCollectionEditorToolkit::OnCreateVariable(TSubclassOf<UCameraVariableAsset> InVariableClass)
{
	UCameraVariableAsset* NewVariable = NewObject<UCameraVariableAsset>(
			VariableCollection, 
			InVariableClass.Get(),
			NAME_None,
			RF_Transactional | RF_Public  // Must be referenceable by camera parameters.
			);
	VariableCollection->Variables.Add(NewVariable);

	VariableCollectionEditorWidget->RequestListRefresh();
}

void FCameraVariableCollectionEditorToolkit::OnRenameVariable()
{
	UClass* VariableAssetClass = UCameraVariableAsset::StaticClass();
	FProperty* DisplayNameProperty = VariableAssetClass->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCameraVariableAsset, DisplayName));

	FPropertyPath PropertyPath;
	PropertyPath.AddProperty(FPropertyInfo(DisplayNameProperty));
	DetailsView->HighlightProperty(PropertyPath);
}

bool FCameraVariableCollectionEditorToolkit::CanRenameVariable()
{
	TArray<UCameraVariableAsset*> Selection;
	VariableCollectionEditorWidget->GetSelectedVariables(Selection);
	return !Selection.IsEmpty();
}

void FCameraVariableCollectionEditorToolkit::OnDeleteVariable()
{
	TArray<UCameraVariableAsset*> Selection;
	VariableCollectionEditorWidget->GetSelectedVariables(Selection);
	if (Selection.IsEmpty())
	{
		return;
	}

	GWarn->BeginSlowTask(LOCTEXT("PreDeleteScanning", "Scanning assets before deleting camera variables"), true);

	UPackage* VariableCollectionPackage = VariableCollection->GetOutermost();
	const FName VariableCollectionPackageName = VariableCollectionPackage->GetFName();

	TSet<FName> AllReferencers;

	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		TArray<FName> OnDiskReferencers;
		AssetRegistry.GetReferencers(VariableCollectionPackageName, OnDiskReferencers);
		AllReferencers.Append(OnDiskReferencers);
	}

	for (UCameraVariableAsset* CameraVariable : Selection)
	{
		bool bIsReferencedInMemoryByNonUndo = false;
		bool bIsReferencedInMemoryByUndo = false;
		FReferencerInformationList MemoryReferences;
		ObjectTools::GatherObjectReferencersForDeletion(
				CameraVariable, bIsReferencedInMemoryByNonUndo, bIsReferencedInMemoryByUndo, &MemoryReferences);

		UPackage* TransientPackage = GetTransientPackage();
		for (const FReferencerInformation& ExternalReference : MemoryReferences.ExternalReferences)
		{
			UPackage* ExternalReferencePackage = ExternalReference.Referencer->GetOutermost();
			if (ExternalReferencePackage != VariableCollectionPackage && ExternalReferencePackage != TransientPackage)
			{
				AllReferencers.Add(ExternalReferencePackage->GetFName());
			}
		}
	}

	GWarn->EndSlowTask();

	bool bPerformDelete = false;
	{
		TSharedRef<SWindow> DeleteVariableWindow = SNew(SWindow)
			.Title(LOCTEXT("DeleteVariablesWindowTitle", "Delete Variables"))
			.ClientSize(FVector2D(600, 700));

		TSharedRef<SDeleteVariableDialog> DeleteVariableDialog = SNew(SDeleteVariableDialog)
			.ParentWindow(DeleteVariableWindow)
			.ReferencingPackages(AllReferencers);
		DeleteVariableWindow->SetContent(DeleteVariableDialog);

		GEditor->EditorAddModalWindow(DeleteVariableWindow);

		bPerformDelete = DeleteVariableDialog->ShouldPerformDelete();
	}

	if (bPerformDelete)
	{
		FScopedTransaction DeleteTransaction(LOCTEXT("DeleteVariable", "Delete camera variable"));

		VariableCollection->Modify();

		TArray<UObject*> ObjectsToReplace(Selection);
		ObjectTools::ForceReplaceReferences(nullptr, ObjectsToReplace);

		TStringBuilder<256> StringBuilder;
		for (UCameraVariableAsset* VariableToDelete : Selection)
		{
			VariableCollection->Variables.Remove(VariableToDelete);

			StringBuilder.Reset();
			StringBuilder.Append("TRASH_");
			StringBuilder.Append(VariableToDelete->GetName());
			VariableToDelete->Rename(StringBuilder.ToString());
			VariableToDelete->MarkAsGarbage();
		}

		VariableCollectionEditorWidget->RequestListRefresh();
	}
}

bool FCameraVariableCollectionEditorToolkit::CanDeleteVariable()
{
	TArray<UCameraVariableAsset*> Selection;
	VariableCollectionEditorWidget->GetSelectedVariables(Selection);
	return !Selection.IsEmpty();
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

