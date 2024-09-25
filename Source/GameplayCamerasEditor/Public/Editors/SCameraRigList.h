// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Misc/TextFilter.h"
#include "UObject/ObjectPtr.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"

class ITableRow;
class SInlineEditableTextBlock;
class SSearchBox;
class STableViewBase;
class UCameraAsset;
class FUICommandList;

namespace UE::Cameras
{

DECLARE_DELEGATE(FOnRequestRenameCameraRig);
DECLARE_DELEGATE_OneParam(FOnCameraRigListChanged, TArrayView<UCameraRigAsset* const>);
DECLARE_DELEGATE_OneParam(FOnCameraRigEvent, UCameraRigAsset*);
DECLARE_DELEGATE_OneParam(FOnMultiCameraRigEvent, const TArray<UCameraRigAsset*>&);

/**
 * List item for the list view in SCameraRigList.
 */
class FCameraRigListItem : public TSharedFromThis<FCameraRigListItem>
{
public:

	UCameraRigAsset* CameraRigAsset = nullptr;
	FOnRequestRenameCameraRig OnRequestRename;
};

/**
 * Item widget for the list view in SCameraRigList.
 */
class SCameraRigListEntry : public STableRow<TSharedPtr<FCameraRigListItem>>
{
public:

	SLATE_BEGIN_ARGS(SCameraRigListEntry)
	{}
		SLATE_ARGUMENT(TWeakPtr<FCameraRigListItem>, Item)
		SLATE_ATTRIBUTE(FText, HighlightText)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable);

private:

	FText GetDisplayName() const;
	void OnRename();
	bool OnVerifyTextChanged(const FText& Text, FText& OutErrorMessage);
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitType);

private:

	TWeakPtr<FCameraRigListItem> WeakItem;
	TSharedPtr<SInlineEditableTextBlock> EditableTextBlock;
};

/**
 * A panel that shows a list of camera rigs on a camera asset.
 * Can add, rename, delete, etc. camera rigs in the list.
 */
class SCameraRigList 
	: public SCompoundWidget
	, public ICameraAssetEventHandler
{
public:

	SLATE_BEGIN_ARGS(SCameraRigList)
		: _CameraAsset(nullptr)
	{}
		SLATE_ARGUMENT(UCameraAsset*, CameraAsset)
		SLATE_EVENT(FOnCameraRigListChanged, OnCameraRigListChanged)
		SLATE_EVENT(FOnCameraRigEvent, OnRequestEditCameraRig)
		SLATE_EVENT(FOnMultiCameraRigEvent, OnCameraRigDeleted)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SCameraRigList();

	void RequestListRefresh();

protected:

	// SWidget interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	// ICameraAssetEventHandler interface
	virtual void OnCameraRigsChanged(UCameraAsset* InCameraAsset, const TCameraArrayChangedEvent<UCameraRigAsset*>& Event) override;

private:

	TSharedPtr<SWidget> GenerateToolbar();

	void OnEditCameraRig();
	bool CanEditCameraRig();

	void OnAddCameraRig();

	void OnRenameCameraRig();
	bool CanRenameCameraRig();

	void OnDeleteCameraRig();
	bool CanDeleteCameraRig();

	void GetEntryStrings(TSharedPtr<FCameraRigListItem> InItem, TArray<FString>& OutStrings);

	void UpdateItemSource();
	void UpdateFilteredItemSource();

	TSharedPtr<FCameraRigListItem> FindListItem(UCameraRigAsset* InCameraRig);

	TSharedRef<ITableRow> OnListGenerateItemRow(TSharedPtr<FCameraRigListItem> Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnListItemScrolledIntoView(TSharedPtr<FCameraRigListItem> Item, const TSharedPtr<ITableRow>& ItemWidget);
	void OnListMouseButtonDoubleClick(TSharedPtr<FCameraRigListItem> Item);
	TSharedPtr<SWidget> OnListContextMenuOpening();

	void OnSearchTextChanged(const FText& InFilterText);
	void OnSearchTextCommitted(const FText& InFilterText, ETextCommit::Type InCommitType);
	FText GetHighlightText() const;

private:

	TObjectPtr<UCameraAsset> CameraAsset;

	TCameraEventHandler<ICameraAssetEventHandler> EventHandler;

	FOnCameraRigListChanged OnCameraRigListChanged;
	FOnCameraRigEvent OnRequestEditCameraRig;
	FOnMultiCameraRigEvent OnCameraRigDeleted;

	TSharedPtr<FUICommandList> CommandList;

	TSharedPtr<SListView<TSharedPtr<FCameraRigListItem>>> ListView;
	TArray<TSharedPtr<FCameraRigListItem>> ItemSource;

	using FEntryTextFilter = TTextFilter<const TSharedPtr<FCameraRigListItem>>;
	TSharedPtr<FEntryTextFilter> SearchTextFilter;
	TSharedPtr<SSearchBox> SearchBox;

	TArray<TSharedPtr<FCameraRigListItem>> FilteredItemSource;
	TSharedPtr<FCameraRigListItem> DeferredRequestRenameItem;
	UCameraRigAsset* DeferredFinishAddCameraRig = nullptr;

	bool bUpdateItemSource = false;
	bool bUpdateFilteredItemSource = false;
};

}  // namespace UE::Cameras

