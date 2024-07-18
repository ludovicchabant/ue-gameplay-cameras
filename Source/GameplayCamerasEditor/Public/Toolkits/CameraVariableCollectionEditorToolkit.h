// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Tools/BaseAssetToolkit.h"
#include "UObject/GCObject.h"

#include "CameraVariableCollectionEditorToolkit.generated.h"

class UCameraVariableAsset;
class UCameraVariableCollection;
class UCameraVariableCollectionEditor;

namespace UE::Cameras
{

class SCameraVariableCollectionEditor;

/**
 * Editor toolkit for a camera variable collection.
 */
class FCameraVariableCollectionEditorToolkit
	: public FBaseAssetToolkit
	, public FGCObject
{
public:

	FCameraVariableCollectionEditorToolkit(UCameraVariableCollectionEditor* InOwningAssetEditor);
	~FCameraVariableCollectionEditorToolkit();

protected:

	// FBaseAssetToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void CreateWidgets() override;
	virtual void RegisterToolbar() override;
	virtual void InitToolMenuContext(FToolMenuContext& MenuContext) override;
	virtual void PostInitAssetEditor() override;

	// IToolkit interface
	virtual FText GetBaseToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	// FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(VariableCollection);
	}
	virtual FString GetReferencerName() const override
	{
		return TEXT("FCameraVariableCollectionEditorToolkit");
	}

private:

	TSharedRef<SDockTab> SpawnTab_VariableCollectionEditor(const FSpawnTabArgs& Args);

	static void GenerateAddNewVariableMenu(UToolMenu* InMenu);

	void OnCreateVariable(TSubclassOf<UCameraVariableAsset> InVariableClass);

	void OnRenameVariable();
	bool CanRenameVariable();

	void OnDeleteVariable();
	bool CanDeleteVariable();

private:

	static const FName VariableCollectionEditorTabId;
	static const FName DetailsViewTabId;

	/** The asset being edited */
	TObjectPtr<UCameraVariableCollection> VariableCollection;

	/** Command bindings */
	TSharedRef<FUICommandList> CommandBindings;

	/** Camera variable collection editor widget */
	TSharedPtr<SCameraVariableCollectionEditor> VariableCollectionEditorWidget;
};

}  // namespace UE::Cameras

UCLASS()
class UCameraVariableCollectionEditorMenuContext : public UObject
{
	GENERATED_BODY()

public:

	TWeakPtr<UE::Cameras::FCameraVariableCollectionEditorToolkit> EditorToolkit;
};

