// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/NotifyHook.h"
#include "Tools/BaseAssetToolkit.h"
#include "UObject/GCObject.h"

class IGameplayCamerasLiveEditManager;
class IMessageLogListing;
class UCameraRigAsset;
class UCameraRigAssetEditor;
class SWidget;

/**
 * Editor toolkit for a camera rig asset.
 */
class FCameraRigAssetEditorToolkit 
	: public FBaseAssetToolkit
	, public FGCObject
	, public FNotifyHook
{
public:

	FCameraRigAssetEditorToolkit(UCameraRigAssetEditor* InOwningAssetEditor);
	~FCameraRigAssetEditorToolkit();

protected:

	// FBaseAssetToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void CreateWidgets() override;
	virtual void RegisterToolbar() override;
	virtual void PostInitAssetEditor() override;

	// IToolkit interface
	virtual FText GetBaseToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	// FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(CameraRigAsset);
	}
	virtual FString GetReferencerName() const override
	{
		return TEXT("FCameraRigAssetEditorToolkit");
	}

	// FNotifyHook interface
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

private:

	FSlateIcon GetBuildButtonIcon() const;
	FText GetBuildButtonTooltip() const;

	void OnBuild();

private:

	static const FName DetailsViewTabId;

	/** Command bindings */
	TSharedRef<FUICommandList> CommandBindings;

	/** Message log widget */
	TSharedPtr<SWidget> Stats;

	/** Message log listing */
	TSharedPtr<IMessageLogListing> StatsListing;

	/** The asset being edited */
	TObjectPtr<UCameraRigAsset> CameraRigAsset;

	/** Live edit manager for updating the assets in the runtime */
	TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager;
};

