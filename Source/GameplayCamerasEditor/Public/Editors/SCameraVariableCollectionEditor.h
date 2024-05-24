// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Misc/TextFilter.h"
#include "Templates/SharedPointerFwd.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"
#include "Widgets/SCompoundWidget.h"

class FAssetEditorToolkit;
class IDetailsView;
class ITableRow;
class SSearchBox;
class STableViewBase;
class UCameraVariableAsset;
class UCameraVariableCollection;
struct FToolMenuContext;
template<typename> class SListView;

namespace UE::Cameras
{

/**
 * An editor widget for a camera variable collection.
 */
class SCameraVariableCollectionEditor : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCameraVariableCollectionEditor)
	{}
		/** The camera variable collection to edit. */
		SLATE_ARGUMENT(TObjectPtr<UCameraVariableCollection>, VariableCollection)
		/** The details view to synchronize with the variable list selection. */
		SLATE_ARGUMENT(TWeakPtr<IDetailsView>, DetailsView)
		/** The toolkit inside which this editor lives, if any. */
		SLATE_ARGUMENT(TWeakPtr<FAssetEditorToolkit>, AssetEditorToolkit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SCameraVariableCollectionEditor();

public:

	/** Gets the selected variables in the list view. */
	void GetSelectedVariables(TArray<UCameraVariableAsset*>& OutSelection) const;

	/** Requests that the list view be refreshed by the next tick. */
	void RequestListRefresh();

protected:

	//~ SWidget interface.
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:

	void UpdateFilteredItemSource();
	void SetDetailsViewObject(UObject* InObject) const;

	TSharedRef<ITableRow> OnListGenerateRow(UCameraVariableAsset* Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnListSectionChanged(UCameraVariableAsset* Item, ESelectInfo::Type SelectInfo) const;

	void GetEntryStrings(const UCameraVariableAsset* InItem, TArray<FString>& OutStrings);
	void OnSearchTextChanged(const FText& InFilterText);
	void OnSearchTextCommitted(const FText& InFilterText, ETextCommit::Type InCommitType);
	FText GetHighlightText() const;

private:

	TObjectPtr<UCameraVariableCollection> VariableCollection;

	TWeakPtr<IDetailsView> WeakDetailsView;

	TSharedPtr<SListView<UCameraVariableAsset*>> ListView;

	TArray<UCameraVariableAsset*> FilteredItemSource;

	using FEntryTextFilter = TTextFilter<const UCameraVariableAsset*>;
	TSharedPtr<FEntryTextFilter> SearchTextFilter;
	TSharedPtr<SSearchBox> SearchBox;

	bool bUpdateFilteredItemSource = false;
};

}  // namespace UE::Cameras

