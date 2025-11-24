// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BaseCameraObject.h"
#include "Core/CameraEventHandler.h"
#include "EdGraphSchema_K2.h"
#include "Templates/SharedPointerFwd.h"
#include "Widgets/SCompoundWidget.h"

#include "SCameraObjectInterfaceParametersPanel.generated.h"

class ITableRow;
class STableViewBase;
template <typename ItemType> class SListView;

namespace UE::Cameras
{

class FCameraObjectInterfaceParametersToolkit;

/**
 * The overall interface parameters panel, showing two sub-panels, one for blendable parameters, and
 * one for data parameters.
 */
class SCameraObjectInterfaceParametersPanel 
	: public SCompoundWidget
	, public ICameraObjectEventHandler
{
public:

	SLATE_BEGIN_ARGS(SCameraObjectInterfaceParametersPanel)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& Args, FCameraObjectInterfaceParametersToolkit* OwnerToolkit);

	void RequestListRefresh();

protected:

	// SWidget interface.
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	// ICameraObjectEventHandler interface.
	virtual void OnCameraObjectInterfaceChanged() override;

private:

	TSharedRef<ITableRow> OnGenerateBlendableParameterRow(TObjectPtr<UCameraObjectInterfaceBlendableParameter> Item, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<ITableRow> OnGenerateDataParameterRow(TObjectPtr<UCameraObjectInterfaceDataParameter> Item, const TSharedRef<STableViewBase>& OwnerTable);

	void OnBlendableSelectionChanged(TObjectPtr<UCameraObjectInterfaceBlendableParameter> Item, ESelectInfo::Type Type);
	void OnDataParameterSelectionChanged(TObjectPtr<UCameraObjectInterfaceDataParameter> Item, ESelectInfo::Type Type);

	TSharedPtr<SWidget> OnBlendableParameterContextMenuOpening();
	TSharedPtr<SWidget> OnDataParameterContextMenuOpening();

	void OnSearchInterfaceParameter(TObjectPtr<UCameraObjectInterfaceParameterBase> Item);

	template<typename ItemType>
	TSharedPtr<SWidget> OnInterfaceParameterContextMenuOpening(
			TSharedPtr<SListView<TObjectPtr<ItemType>>> ListView, TObjectPtr<ItemType> Item);
	template<typename ItemType>
	void OnRenameInterfaceParameter(
			TSharedPtr<SListView<TObjectPtr<ItemType>>> ListView, TObjectPtr<ItemType> Item);
	template<typename ItemType>
	void OnDeleteInterfaceParameter(
			TSharedPtr<SListView<TObjectPtr<ItemType>>> ListView, TObjectPtr<ItemType> Item);

	FReply OnAddBlendableParameter();
	FReply OnAddDataParameter();

private:

	UBaseCameraObject* CameraObject = nullptr;
	FCameraObjectInterfaceParametersToolkit* Toolkit = nullptr;

	TCameraEventHandler<ICameraObjectEventHandler> EventHandler;

	TSharedPtr<SListView<TObjectPtr<UCameraObjectInterfaceBlendableParameter>>> BlendableParametersListView;
	TSharedPtr<SListView<TObjectPtr<UCameraObjectInterfaceDataParameter>>> DataParametersListView;

	bool bListRefreshRequested = false;
};

}  // namespace UE::Cameras

// Dummy Blueprint schema used only for getting proper icons and colors for parameter types.
UCLASS(MinimalAPI, Hidden)
class UEdGraphSchema_CameraNodeK2 : public UEdGraphSchema_K2
{
	GENERATED_BODY()

public:

	virtual bool SupportsPinTypeContainer(TWeakPtr<const FEdGraphSchemaAction> SchemaAction, const FEdGraphPinType& PinType, const EPinContainerType& ContainerType) const
	{
		if (ContainerType == EPinContainerType::None || ContainerType == EPinContainerType::Array)
		{
			return Super::SupportsPinTypeContainer(SchemaAction, PinType, ContainerType);
		}
		return false;
	}
};

