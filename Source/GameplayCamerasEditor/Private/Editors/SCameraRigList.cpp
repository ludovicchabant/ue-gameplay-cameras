// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigList.h"

#include "Commands/CameraAssetEditorCommands.h"
#include "Core/CameraAsset.h"
#include "Framework/Commands/UICommandList.h"
#include "ScopedTransaction.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "ToolMenus.h"
#include "UObject/UObjectGlobals.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/Views/SListView.h"

#define LOCTEXT_NAMESPACE "SCameraRigList"

namespace UE::Cameras
{

void SCameraRigListEntry::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable)
{
	using FSuperRowType = STableRow<TSharedPtr<FCameraRigListItem>>;

	WeakItem = InArgs._Item;

	ChildSlot
	.Padding(FMargin(8.f, 2.f, 12.f, 2.f))
	[
		SNew(SBox)
		.Padding(8.f, 4.f)
		[
			SAssignNew(EditableTextBlock, SInlineEditableTextBlock)
			.Text(this, &SCameraRigListEntry::GetDisplayName)
			.OnTextCommitted(this, &SCameraRigListEntry::OnTextCommitted)
			.OnVerifyTextChanged(this, &SCameraRigListEntry::OnVerifyTextChanged)
			.HighlightText(InArgs._HighlightText)
			.IsSelected(this, &FSuperRowType::IsSelectedExclusively)
		]
	];

	if (TSharedPtr<FCameraRigListItem> Item = WeakItem.Pin())
	{
		Item->OnRequestRename.BindSP(this, &SCameraRigListEntry::OnRename);
	}

	TSharedRef<FGameplayCamerasEditorStyle> CamerasStyle = FGameplayCamerasEditorStyle::Get();

	FSuperRowType::ConstructInternal(
		FSuperRowType::FArguments()
			.Style(&CamerasStyle->GetWidgetStyle<FTableRowStyle>("CameraAssetEditor.CameraRigsList.RowStyle")),
		OwnerTable);
}

FText SCameraRigListEntry::GetDisplayName() const
{
	if (TSharedPtr<FCameraRigListItem> Item = WeakItem.Pin())
	{
		UCameraRigAsset* CameraRigAsset = Item->CameraRigAsset;
		return FText::FromString(CameraRigAsset->GetDisplayName());
	}
	return FText();
}

void SCameraRigListEntry::OnRename()
{
	EditableTextBlock->EnterEditingMode();
}

bool SCameraRigListEntry::OnVerifyTextChanged(const FText& Text, FText& OutErrorMessage)
{
	TSharedPtr<FCameraRigListItem> Item = WeakItem.Pin();
	if (!Item)
	{
		OutErrorMessage = LOCTEXT("InvalidEntry", "Invalid entry");
		return false;
	}

	UCameraRigAsset* CameraRigAsset = Item->CameraRigAsset;
	UCameraAsset* OwnerCamera = CameraRigAsset->GetTypedOuter<UCameraAsset>();
	if (!OwnerCamera)
	{
		OutErrorMessage = LOCTEXT("InvalidEntry", "Invalid entry");
		return false;
	}

	const FString TextString = Text.ToString();
	const TObjectPtr<UCameraRigAsset>* FoundItem = OwnerCamera->GetCameraRigs().FindByPredicate(
			[&TextString](const UCameraRigAsset* Item) { return Item->GetDisplayName() == TextString; });
	if (FoundItem)
	{
		OutErrorMessage = LOCTEXT("NamingCollection", "A camera rig already exists with that name");
		return false;
	}

	return true;
}

void SCameraRigListEntry::OnTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (TSharedPtr<FCameraRigListItem> Item = WeakItem.Pin())
	{
		FScopedTransaction Transaction(LOCTEXT("RenameCameraRig", "Rename Camera Rig"));

		UCameraRigAsset* CameraRigAsset = Item->CameraRigAsset;
		CameraRigAsset->Modify();

		// Set the interface name, but also rename the object itself, which helps for debugging.
		const FString NewDisplayName = Text.ToString();
		CameraRigAsset->Interface.DisplayName = NewDisplayName;

		FName NewObjectName = MakeObjectNameFromDisplayLabel(NewDisplayName, CameraRigAsset->GetFName());
		NewObjectName = MakeUniqueObjectName(CameraRigAsset->GetOuter(), UCameraRigAsset::StaticClass(), NewObjectName);
		CameraRigAsset->Rename(*NewObjectName.ToString());
	}
}

void SCameraRigList::Construct(const FArguments& InArgs)
{
	CameraAsset = InArgs._CameraAsset;

	OnCameraRigListChanged = InArgs._OnCameraRigListChanged;
	OnRequestEditCameraRig = InArgs._OnRequestEditCameraRig;
	OnCameraRigDeleted = InArgs._OnCameraRigDeleted;

	CommandList = MakeShared<FUICommandList>();

	SearchTextFilter = MakeShareable(new FEntryTextFilter(
		FEntryTextFilter::FItemToStringArray::CreateSP(this, &SCameraRigList::GetEntryStrings)));

	TSharedPtr<SWidget> ToolbarWidget = GenerateToolbar();

	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				ToolbarWidget.ToSharedRef()
			]
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(8.f)
			[
				SAssignNew(SearchBox, SSearchBox)
				.HintText(LOCTEXT("SearchHint", "Search"))
				.OnTextChanged(this, &SCameraRigList::OnSearchTextChanged)
				.OnTextCommitted(this, &SCameraRigList::OnSearchTextCommitted)
			]
		]
		+SVerticalBox::Slot()
		.Padding(0.f, 3.f)
		[
			SAssignNew(ListView, SListView<TSharedPtr<FCameraRigListItem>>)
				.ListItemsSource(&FilteredItemSource)
				.OnGenerateRow(this, &SCameraRigList::OnListGenerateItemRow)
				.OnItemScrolledIntoView(this, &SCameraRigList::OnListItemScrolledIntoView)
				.OnMouseButtonDoubleClick(this, &SCameraRigList::OnListMouseButtonDoubleClick)
				.OnContextMenuOpening(this, &SCameraRigList::OnListContextMenuOpening)
		]
	];

	const FCameraAssetEditorCommands& Commands = FCameraAssetEditorCommands::Get();

	CommandList->MapAction(
			Commands.EditCameraRig,
			FExecuteAction::CreateSP(this, &SCameraRigList::OnEditCameraRig),
			FCanExecuteAction::CreateSP(this, &SCameraRigList::CanEditCameraRig));
	CommandList->MapAction(
			Commands.AddCameraRig,
			FExecuteAction::CreateSP(this, &SCameraRigList::OnAddCameraRig));
	CommandList->MapAction(
			Commands.RenameCameraRig,
			FExecuteAction::CreateSP(this, &SCameraRigList::OnRenameCameraRig),
			FCanExecuteAction::CreateSP(this, &SCameraRigList::CanRenameCameraRig));
	CommandList->MapAction(
			Commands.DeleteCameraRig,
			FExecuteAction::CreateSP(this, &SCameraRigList::OnDeleteCameraRig),
			FCanExecuteAction::CreateSP(this, &SCameraRigList::CanDeleteCameraRig));

	UpdateItemSource();
	UpdateFilteredItemSource();
	ListView->RequestListRefresh();
	if (!FilteredItemSource.IsEmpty())
	{
		ListView->SetSelection(FilteredItemSource[0]);
		OnRequestEditCameraRig.ExecuteIfBound(FilteredItemSource[0]->CameraRigAsset);
	}
}

SCameraRigList::~SCameraRigList()
{
}

void SCameraRigList::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bUpdateItemSource)
	{
		UpdateItemSource();
	}

	if (bUpdateItemSource || bUpdateFilteredItemSource)
	{
		UpdateFilteredItemSource();
	}

	const bool bRequestListRefresh = bUpdateItemSource || bUpdateFilteredItemSource;
	bUpdateItemSource = false;
	bUpdateFilteredItemSource = false;

	if (bRequestListRefresh)
	{
		ListView->RequestListRefresh();
	}

	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

TSharedPtr<SWidget> SCameraRigList::GenerateToolbar()
{
	static const FName ToolbarName("CameraRigList.ToolBar");

	UToolMenus* ToolMenus = UToolMenus::Get();

	if (!ToolMenus->IsMenuRegistered(ToolbarName))
	{
		const FCameraAssetEditorCommands& Commands = FCameraAssetEditorCommands::Get();

		UToolMenu* Toolbar = ToolMenus->RegisterMenu(ToolbarName, NAME_None, EMultiBoxType::SlimHorizontalToolBar);

		FToolMenuSection& Section = Toolbar->AddSection("CameraRigs");
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(
					Commands.AddCameraRig,
					LOCTEXT("AddCameraRigButton", "Add")  // Shorter label
				));
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(
					Commands.RenameCameraRig,
					LOCTEXT("RenameCameraRigButton", "Rename")  // Shorter label
				));
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(
					Commands.DeleteCameraRig,
					LOCTEXT("DeleteCameraRigButton", "Delete")  // Shorter label
				));
	}

	FToolMenuContext MenuContext;
	MenuContext.AppendCommandList(CommandList);
	return ToolMenus->GenerateWidget(ToolbarName, MenuContext);
}

void SCameraRigList::OnEditCameraRig()
{
	TArray<TSharedPtr<FCameraRigListItem>> SelectedItems;
	ListView->GetSelectedItems(SelectedItems);
	if (!SelectedItems.IsEmpty())
	{
		OnRequestEditCameraRig.ExecuteIfBound(SelectedItems[0]->CameraRigAsset);
	}
}

bool SCameraRigList::CanEditCameraRig()
{
	TArray<TSharedPtr<FCameraRigListItem>> SelectedItems;
	ListView->GetSelectedItems(SelectedItems);
	return SelectedItems.Num() == 1;
}

void SCameraRigList::OnAddCameraRig()
{
	FScopedTransaction Transaction(LOCTEXT("AddCameraRig", "Add Camera Rig"));

	CameraAsset->Modify();

	const FName NewCameraRigName = MakeUniqueObjectName(CameraAsset, UCameraRigAsset::StaticClass(), TEXT("NewCameraRig"));
	UCameraRigAsset* NewCameraRig = NewObject<UCameraRigAsset>(
			CameraAsset, 
			NewCameraRigName, 
			RF_Transactional | RF_Public  // Must be referenceable from camera directors.
			);
	CameraAsset->AddCameraRig(NewCameraRig);

	bUpdateItemSource = true;
}

void SCameraRigList::OnRenameCameraRig()
{
	TArray<TSharedPtr<FCameraRigListItem>> SelectedItems;
	ListView->GetSelectedItems(SelectedItems);
	if (SelectedItems.Num() > 0)
	{
		ListView->RequestScrollIntoView(SelectedItems[0]);
		DeferredRequestRenameItem = SelectedItems[0];
	}
}

bool SCameraRigList::CanRenameCameraRig()
{
	TArray<TSharedPtr<FCameraRigListItem>> SelectedItems;
	ListView->GetSelectedItems(SelectedItems);
	return SelectedItems.Num() == 1;
}

void SCameraRigList::OnDeleteCameraRig()
{
	TArray<TSharedPtr<FCameraRigListItem>> SelectedItems;
	ListView->GetSelectedItems(SelectedItems);
	if (SelectedItems.Num() > 0)
	{
		FScopedTransaction Transaction(LOCTEXT("DeleteCameraRigs", "Delete Camera Rigs"));

		CameraAsset->Modify();

		TStringBuilder<256> StringBuilder;
		TArray<UCameraRigAsset*> DeletedCameraRigs;

		for (TSharedPtr<FCameraRigListItem> Item : SelectedItems)
		{
			UCameraRigAsset* CameraRigAsset = Item->CameraRigAsset;
			if (CameraRigAsset)
			{
				CameraRigAsset->Modify();
				
				StringBuilder.Reset();
				StringBuilder.Append(TEXT("DELETED_"));
				StringBuilder.Append(CameraRigAsset->GetName());
				CameraRigAsset->Rename(StringBuilder.ToString());

				const int32 NumRemoved = CameraAsset->RemoveCameraRig(CameraRigAsset);
				ensure(NumRemoved == 1);

				DeletedCameraRigs.Add(CameraRigAsset);
			}
		}

		bUpdateItemSource = true;

		OnCameraRigDeleted.ExecuteIfBound(DeletedCameraRigs);
	}
}

bool SCameraRigList::CanDeleteCameraRig()
{
	TArray<TSharedPtr<FCameraRigListItem>> SelectedItems;
	ListView->GetSelectedItems(SelectedItems);
	return SelectedItems.Num() > 0;
}

void SCameraRigList::GetEntryStrings(const TSharedPtr<FCameraRigListItem> InItem, TArray<FString>& OutStrings)
{
	if (InItem->CameraRigAsset)
	{
		FString DisplayName = InItem->CameraRigAsset->GetDisplayName();
		OutStrings.Add(DisplayName);
	}
}

void SCameraRigList::UpdateItemSource()
{
	ItemSource.Reset();

	if (CameraAsset)
	{
		for (UCameraRigAsset* CameraRigAsset : CameraAsset->GetCameraRigs())
		{
			TSharedPtr<FCameraRigListItem> Item = MakeShared<FCameraRigListItem>();
			Item->CameraRigAsset = CameraRigAsset;
			ItemSource.Add(Item);
		}
	}

	OnCameraRigListChanged.ExecuteIfBound(CameraAsset->GetCameraRigs());
}

void SCameraRigList::UpdateFilteredItemSource()
{
	FilteredItemSource = ItemSource;
	FilteredItemSource.StableSort([](TSharedPtr<FCameraRigListItem> A, TSharedPtr<FCameraRigListItem> B)
			{ 
				return A->CameraRigAsset->GetDisplayName().Compare(B->CameraRigAsset->GetDisplayName()) < 0;
			});

	if (!SearchTextFilter->GetRawFilterText().IsEmpty())
	{
		FilteredItemSource = FilteredItemSource.FilterByPredicate(
				[this](TSharedPtr<FCameraRigListItem> Item)
				{
					return SearchTextFilter->PassesFilter(Item);
				});
	}
}

TSharedRef<ITableRow> SCameraRigList::OnListGenerateItemRow(TSharedPtr<FCameraRigListItem> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SCameraRigListEntry, OwnerTable)
		.Item(Item)
		.HighlightText(this, &SCameraRigList::GetHighlightText);
}

void SCameraRigList::OnListItemScrolledIntoView(TSharedPtr<FCameraRigListItem> Item, const TSharedPtr<ITableRow>& ItemWidget)
{
	if (DeferredRequestRenameItem)
	{
		DeferredRequestRenameItem->OnRequestRename.ExecuteIfBound();
		DeferredRequestRenameItem.Reset();
	}
}

void SCameraRigList::OnListMouseButtonDoubleClick(TSharedPtr<FCameraRigListItem> Item)
{
	if (Item)
	{
		OnRequestEditCameraRig.ExecuteIfBound(Item->CameraRigAsset);
	}
}

TSharedPtr<SWidget> SCameraRigList::OnListContextMenuOpening()
{
	static const FName ContextMenuName("CameraRigList.ContextMenu");

	UToolMenus* ToolMenus = UToolMenus::Get();

	if (!ToolMenus->IsMenuRegistered(ContextMenuName))
	{
		const FCameraAssetEditorCommands& Commands = FCameraAssetEditorCommands::Get();

		UToolMenu* ContextMenu = ToolMenus->RegisterMenu(ContextMenuName, NAME_None, EMultiBoxType::Menu);

		FToolMenuSection& Section = ContextMenu->AddSection("Actions");
		Section.AddEntry(FToolMenuEntry::InitMenuEntry(
					Commands.EditCameraRig,
					LOCTEXT("AddCameraRigButton", "Add")  // Shorter label
				));
		Section.AddEntry(FToolMenuEntry::InitMenuEntry(
					Commands.RenameCameraRig,
					LOCTEXT("RenameCameraRigButton", "Rename")  // Shorter label
				));
		Section.AddEntry(FToolMenuEntry::InitMenuEntry(
					Commands.DeleteCameraRig,
					LOCTEXT("DeleteCameraRigButton", "Delete")  // Shorter label
				));
	}

	FToolMenuContext MenuContext;
	MenuContext.AppendCommandList(CommandList);
	return ToolMenus->GenerateWidget(ContextMenuName, MenuContext);
}

void SCameraRigList::OnSearchTextChanged(const FText& InFilterText)
{
	SearchTextFilter->SetRawFilterText(InFilterText);
	SearchBox->SetError(SearchTextFilter->GetFilterErrorText());

	bUpdateFilteredItemSource = true;
}

void SCameraRigList::OnSearchTextCommitted(const FText& InFilterText, ETextCommit::Type InCommitType)
{
	OnSearchTextChanged(InFilterText);
}

FText SCameraRigList::GetHighlightText() const
{
	return SearchTextFilter->GetRawFilterText();
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

