// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Misc/NotifyHook.h"
#include "Tools/BaseAssetToolkit.h"
#include "UObject/GCObject.h"

#include "CameraRigAssetEditorToolkit.generated.h"

class IMessageLogListing;
class SFindInObjectTreeGraph;
class SObjectTreeGraphToolbox;
class SWidget;
class UCameraRigAsset;
class UCameraRigAssetEditor;
class UEdGraphNode;

namespace UE::Cameras
{

class IGameplayCamerasLiveEditManager;
class SCameraRigAssetEditor;

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
		Collector.AddReferencedObject(CameraRigAsset);
	}
	virtual FString GetReferencerName() const override
	{
		return TEXT("FCameraRigAssetEditorToolkit");
	}

	// FNotifyHook interface
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

private:

	TSharedRef<SDockTab> SpawnTab_Toolbox(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_CameraRigEditor(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Search(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Messages(const FSpawnTabArgs& Args);

	FSlateIcon GetBuildButtonIcon() const;
	FText GetBuildButtonTooltip() const;

	void OnBuild();
	void OnFindInCameraRig();

private:

	static const FName ToolboxTabId;
	static const FName CameraRigEditorTabId;
	static const FName SearchTabId;
	static const FName MessagesTabId;
	static const FName DetailsViewTabId;

	/** The asset being edited */
	TObjectPtr<UCameraRigAsset> CameraRigAsset;

	/** Command bindings */
	TSharedRef<FUICommandList> CommandBindings;

	/** Message log listing */
	TSharedPtr<IMessageLogListing> MessageListing;

	/** Camera rig editor widget */
	TSharedPtr<SCameraRigAssetEditor> CameraRigEditorWidget;

	/** Toolbox widget */
	TSharedPtr<SObjectTreeGraphToolbox> ToolboxWidget;

	/** Message log widget */
	TSharedPtr<SWidget> MessagesWidget;

	/** Search widget */
	TSharedPtr<SFindInObjectTreeGraph> SearchWidget;

	/** Live edit manager for updating the assets in the runtime */
	TSharedPtr<UE::Cameras::IGameplayCamerasLiveEditManager> LiveEditManager;
};

}  // namespace UE::Cameras

UCLASS()
class UCameraRigAssetEditorMenuContext : public UObject
{
	GENERATED_BODY()

public:

	TWeakPtr<UE::Cameras::FCameraRigAssetEditorToolkit> CameraRigAssetEditorToolkit;
};

