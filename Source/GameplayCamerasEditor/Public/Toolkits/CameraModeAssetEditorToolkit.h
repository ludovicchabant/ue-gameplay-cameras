// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tools/BaseAssetToolkit.h"
#include "UObject/GCObject.h"

class IMessageLogListing;
class UCameraMode;
class UCameraModeAssetEditor;
class SWidget;

/**
 * Editor toolkit for a camera mode asset.
 */
class FCameraModeAssetEditorToolkit 
	: public FBaseAssetToolkit
	, public FGCObject
{
public:

	FCameraModeAssetEditorToolkit(UCameraModeAssetEditor* InOwningAssetEditor);
	~FCameraModeAssetEditorToolkit();

protected:

	// FBaseAssetToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void CreateWidgets() override;
	virtual void PostInitAssetEditor() override;

	// IToolkit interface
	virtual FText GetBaseToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	// FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(CameraModeAsset);
	}
	virtual FString GetReferencerName() const override
	{
		return TEXT("FCameraModeAssetEditorToolkit");
	}

private:

	static const FName DetailsViewTabId;

	/** Message log widget */
	TSharedPtr<SWidget> Stats;

	/** Message log listing */
	TSharedPtr<IMessageLogListing> StatsListing;

	/** The asset being edited */
	TObjectPtr<UCameraMode> CameraModeAsset;
};

