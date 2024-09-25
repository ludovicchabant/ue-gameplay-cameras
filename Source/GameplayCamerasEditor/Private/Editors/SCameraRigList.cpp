// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigList.h"

#include "Commands/CameraAssetEditorCommands.h"
#include "Core/CameraAsset.h"
#include "Editor.h"
#include "Framework/Commands/UICommandList.h"
#include "ScopedTransaction.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "ToolMenus.h"
#include "UObject/UObjectGlobals.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SDeleteCameraObjectDialog.h"
#include "Widgets/SWindow.h"
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

		const FString NewDisplayName = Text.ToString();
		CameraRigAsset->Modify();
		CameraRigAsset->Interface.DisplayName = NewDisplayName;
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

	CameraAsset->EventHandlers.Register(EventHandler, this);
}

SCameraRigList::~SCameraRigList()
{
}

void SCameraRigList::RequestListRefresh()
{
	bUpdateItemSource = true;
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

	if (DeferredFinishAddCameraRig)
	{
		// If we just added a new camera rig, find it in the list of items. We're going 
		// to request for it to be open in the graph editor, and enter rename mode.
		TSharedPtr<FCameraRigListItem> AddedListItem = FindListItem(DeferredFinishAddCameraRig);
		DeferredFinishAddCameraRig = nullptr;

		if (AddedListItem)
		{
			ListView->SetSelection(AddedListItem);
			OnRequestEditCameraRig.ExecuteIfBound(AddedListItem->CameraRigAsset);

			DeferredRequestRenameItem = AddedListItem;
			ListView->RequestScrollIntoView(AddedListItem);
		}
	}

	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

FReply SCameraRigList::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (CommandList->ProcessCommandBindings(InKeyEvent))
	{
		return FReply::Handled();
	}
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

void SCameraRigList::OnCameraRigsChanged(UCameraAsset* InCameraAsset, const TCameraArrayChangedEvent<UCameraRigAsset*>& Event)
{
	RequestListRefresh();
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

	UCameraRigAsset* NewCameraRig = NewObject<UCameraRigAsset>(
			CameraAsset,
			NAME_None,
			RF_Transactional | RF_Public  // Must be referenceable from camera directors.
			);
	CameraAsset->AddCameraRig(NewCameraRig);

	DeferredFinishAddCameraRig = NewCameraRig;
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
	// Check if we have anything to delete.
	TArray<TSharedPtr<FCameraRigListItem>> SelectedItems;
	ListView->GetSelectedItems(SelectedItems);
	if (SelectedItems.IsEmpty())
	{
		return;
	}

	// Display a dialog showing any referencing assets that would need to be modified.
	TSharedRef<SWindow> DeleteCameraRigWindow = SNew(SWindow)
		.Title(LOCTEXT("DeleteCameraRigWindowTitle", "Delete Camera Rig(s)"))
		.ClientSize(FVector2D(600, 700));

	TArray<UObject*> ObjectsToDelete;
	for (TSharedPtr<FCameraRigListItem> SelectedItem : SelectedItems)
	{
		ObjectsToDelete.Add(SelectedItem->CameraRigAsset);
	}

	TSharedRef<SDeleteCameraObjectDialog> DeleteCameraRigDialog = SNew(SDeleteCameraObjectDialog)
		.ParentWindow(DeleteCameraRigWindow)
		.ObjectsToDelete(ObjectsToDelete)
		.OnDeletedObject_Lambda([](UObject* Obj)
					{
						if (UCameraRigAsset* TrashCameraRig = Cast<UCameraRigAsset>(Obj))
						{
							SDeleteCameraObjectDialog::RenameObjectAsTrash(TrashCameraRig->Interface.DisplayName);
						}
					});
	DeleteCameraRigWindow->SetContent(DeleteCameraRigDialog);

	GEditor->EditorAddModalWindow(DeleteCameraRigWindow);

	// Remove the camera rigs from the camera asset and perform the reference replacement.
	const bool bPerformDelete = DeleteCameraRigDialog->ShouldPerformDelete();
	if (bPerformDelete)
	{
		FScopedTransaction Transaction(LOCTEXT("DeleteCameraRigs", "Delete Camera Rig(s)"));

		CameraAsset->Modify();

		TArray<UCameraRigAsset*> DeletedCameraRigs;
		for (TSharedPtr<FCameraRigListItem> Item : SelectedItems)
		{
			UCameraRigAsset* CameraRigAsset = Item->CameraRigAsset;
			if (CameraRigAsset)
			{
				const int32 NumRemoved = CameraAsset->RemoveCameraRig(CameraRigAsset);
				ensure(NumRemoved == 1);

				DeletedCameraRigs.Add(CameraRigAsset);
			}
		}

		DeleteCameraRigDialog->PerformReferenceReplacement();

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

TSharedPtr<FCameraRigListItem> SCameraRigList::FindListItem(UCameraRigAsset* InCameraRig)
{
	TSharedPtr<FCameraRigListItem>* FoundItem = FilteredItemSource.FindByPredicate(
			[InCameraRig](TSharedPtr<FCameraRigListItem> Item)
			{
				return Item->CameraRigAsset == InCameraRig;
			});
	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
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

