// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/CameraParameterGetterGraphNodeBase.h"

#include "CameraVariableAssetGraphNode.generated.h"

class UCameraVariableAsset;

/**
 * Custom graph editor node for a camera rig parameter getter.
 */
UCLASS()
class UCameraVariableAssetGraphNode : public UCameraParameterGetterGraphNodeBase
{
	GENERATED_BODY()

public:

	/** Creates a new graph node. */
	UCameraVariableAssetGraphNode(const FObjectInitializer& ObjInit);

	/** Gets the referenced variable asset. */
	UCameraVariableAsset* GetVariableAsset() const;

public:

	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

	// UCameraParameterGetterGraphNodeBase interface.
	virtual FEdGraphPinType GetParameterPinType() const override;
};

