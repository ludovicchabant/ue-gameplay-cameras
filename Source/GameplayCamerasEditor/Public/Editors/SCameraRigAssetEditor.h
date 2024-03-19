// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "GraphEditor.h"
#include "Templates/SharedPointerFwd.h"
#include "UObject/ObjectPtr.h"
#include "Widgets/SCompoundWidget.h"

class FAssetEditorToolkit;
class IDetailsView;
class SBox;
class SObjectTreeGraphEditor;
class UCameraRigAsset;
class UEdGraphNode;
class UObjectTreeGraph;
struct FObjectTreeGraphConfig;

namespace UE::Cameras
{

enum class ECameraRigAssetEditorMode
{
	NodeGraph,
	TransitionGraph
};

class SCameraRigAssetEditor : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCameraRigAssetEditor)
	{}
		SLATE_ARGUMENT(TObjectPtr<UCameraRigAsset>, CameraRigAsset)
		SLATE_ARGUMENT(TSharedPtr<IDetailsView>, DetailsView)
		SLATE_ARGUMENT(TWeakPtr<FAssetEditorToolkit>, AssetEditorToolkit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

public:

	ECameraRigAssetEditorMode GetEditorMode() const;
	bool IsEditorMode(ECameraRigAssetEditorMode InMode) const;
	void SetEditorMode(ECameraRigAssetEditorMode InMode);

	void GetGraphs(TArray<UEdGraph*>& OutGraphs) const;

	const FObjectTreeGraphConfig& GetFocusedGraphConfig() const;

	void FocusHome();
	void JumpToNode(UEdGraphNode* InGraphNode);

protected:

	void CreateNodeGraphEditor(const FArguments& InArgs);
	void CreateTransitionGraphEditor(const FArguments& InArgs);

	FText GetCameraRigAssetName() const;

private:

	/** The asset being edited */
	TObjectPtr<UCameraRigAsset> CameraRigAsset;

	/** The node hierarchy graph */
	TObjectPtr<UObjectTreeGraph> NodeGraph;
	/** The node hierarchy graph editor */
	TSharedPtr<SObjectTreeGraphEditor> NodeGraphEditor;

	/** The transition graph */
	TObjectPtr<UObjectTreeGraph> TransitionGraph;
	/** The transition graph editor */
	TSharedPtr<SObjectTreeGraphEditor> TransitionGraphEditor;

	/** Box panel holding either the node hierarchy or transition graph editor */
	TSharedPtr<SBox> BoxPanel;

	/** The mode for the currently shown graph editor */
	ECameraRigAssetEditorMode CurrentMode;
};

}  // namespace UE::Cameras

