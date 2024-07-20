// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraNodeGraphSchema.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraNode.h"
#include "Core/CameraRigAsset.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/CameraNodeGraphNode.h"
#include "Editors/CameraRigInterfaceParameterGraphNode.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "GameplayCamerasEditorSettings.h"

#include "ScopedTransaction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraNodeGraphSchema)

#define LOCTEXT_NAMESPACE "CameraNodeGraphSchema"

const FName UCameraNodeGraphSchema::PC_CameraParameter("CameraParameter");

FObjectTreeGraphConfig UCameraNodeGraphSchema::BuildGraphConfig() const
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	FObjectTreeGraphConfig GraphConfig;
	GraphConfig.GraphName = UCameraRigAsset::NodeTreeGraphName;
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigAsset::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraNode::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigInterfaceParameter::StaticClass());
	GraphConfig.NonConnectableObjectClasses.Add(UBlendCameraNode::StaticClass());
	GraphConfig.GraphDisplayInfo.PlainName = LOCTEXT("NodeGraphPlainName", "CameraNodes");
	GraphConfig.GraphDisplayInfo.DisplayName = LOCTEXT("NodeGraphDisplayName", "Camera Nodes");
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigAsset::StaticClass())
		.OnlyAsRoot()
		.HasSelfPin(false)
		.NodeTitleUsesObjectName(true)
		.NodeTitleColor(Settings->CameraRigAssetTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UCameraNode::StaticClass())
		.StripDisplayNameSuffix(TEXT("Camera Node"))
		.CreateCategoryMetaData(TEXT("CameraNodeCategories"))
		.GraphNodeClass(UCameraNodeGraphNode::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigInterfaceParameter::StaticClass())
		.SelfPinName(NAME_None)  // No self pin name, we just want the title
		.CanCreateNew(false)
		.GraphNodeClass(UCameraRigInterfaceParameterGraphNode::StaticClass());
	GraphConfig.OnFormatObjectDisplayName = FOnFormatObjectDisplayName::CreateLambda(
			[](const UObject* Object, FText& InOutDisplayNameText)
			{
				if (const UCameraRigAsset* CameraRigAsset = Cast<UCameraRigAsset>(Object))
				{
					InOutDisplayNameText = FText::FromString(CameraRigAsset->GetDisplayName());
				}
			});

	return GraphConfig;
}

void UCameraNodeGraphSchema::OnCreateAllNodes(UObjectTreeGraph* InGraph, const FCreatedNodes& InCreatedNodes) const
{
	Super::OnCreateAllNodes(InGraph, InCreatedNodes);

	UCameraRigAsset* CameraRig = Cast<UCameraRigAsset>(InGraph->GetRootObject());
	if (ensure(CameraRig))
	{
		for (UCameraRigInterfaceParameter* InterfaceParameter : CameraRig->Interface.InterfaceParameters)
		{
			UObjectTreeGraphNode* const* InterfaceParameterNode = InCreatedNodes.CreatedNodes.Find(InterfaceParameter);
			UObjectTreeGraphNode* const* CameraNodeNode = InCreatedNodes.CreatedNodes.Find(InterfaceParameter->Target);
			if (InterfaceParameterNode && CameraNodeNode)
			{
				UEdGraphPin* InterfaceParameterSelfPin = (*InterfaceParameterNode)->GetSelfPin();
				UEdGraphPin* CameraParameterPin = Cast<UCameraNodeGraphNode>(*CameraNodeNode)->GetPinForCameraParameterProperty(InterfaceParameter->TargetPropertyName);
				InterfaceParameterSelfPin->MakeLinkTo(CameraParameterPin);
			}
		}
	}
}

void UCameraNodeGraphSchema::OnAddConnectableObject(UObjectTreeGraph* InGraph, UObjectTreeGraphNode* InNewNode) const
{
	Super::OnAddConnectableObject(InGraph, InNewNode);

	if (UCameraRigInterfaceParameter* InterfaceParameter = Cast<UCameraRigInterfaceParameter>(InNewNode->GetObject()))
	{
		UCameraRigAsset* CameraRig = Cast<UCameraRigAsset>(InGraph->GetRootObject());
		if (ensure(CameraRig))
		{
			CameraRig->Modify();

			const int32 Index = CameraRig->Interface.InterfaceParameters.AddUnique(InterfaceParameter);
			ensure(Index == CameraRig->Interface.InterfaceParameters.Num() - 1);
		}
	}
}

void UCameraNodeGraphSchema::OnRemoveConnectableObject(UObjectTreeGraph* InGraph, UObjectTreeGraphNode* InRemovedNode) const
{
	Super::OnRemoveConnectableObject(InGraph, InRemovedNode);

	if (UCameraRigInterfaceParameter* InterfaceParameter = Cast<UCameraRigInterfaceParameter>(InRemovedNode->GetObject()))
	{
		UCameraRigAsset* CameraRig = Cast<UCameraRigAsset>(InGraph->GetRootObject());
		if (ensure(CameraRig))
		{
			CameraRig->Modify();

			const int32 NumRemoved = CameraRig->Interface.InterfaceParameters.Remove(InterfaceParameter);
			ensure(NumRemoved == 1);
		}
	}
}

void UCameraNodeGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// See if we were dragging a camera parameter pin.
	if (const UEdGraphPin* DraggedPin = ContextMenuBuilder.FromPin)
	{
		if (DraggedPin->PinType.PinCategory == PC_CameraParameter)
		{
			UCameraNodeGraphNode* CameraNodeNode = Cast<UCameraNodeGraphNode>(DraggedPin->GetOwningNode());
			FStructProperty* StructProperty = CameraNodeNode->GetCameraParameterPropertyForPin(DraggedPin);

			TSharedRef<FCameraNodeGraphSchemaAction_NewInterfaceParameterNode> Action = 
				MakeShared<FCameraNodeGraphSchemaAction_NewInterfaceParameterNode>(
						FText::GetEmpty(),
						LOCTEXT("NewInterfaceParameterAction", "Camera Rig Parameter"),
						LOCTEXT("NewInterfaceParameterActionToolTip", "Exposes this parameter on the camera rig"));
			Action->Target = Cast<UCameraNode>(CameraNodeNode->GetObject());
			Action->TargetPropertyName = StructProperty->GetFName();
			ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(Action.ToSharedPtr()));

			return;
		}
	}

	Super::GetGraphContextActions(ContextMenuBuilder);
}

const FPinConnectionResponse UCameraNodeGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->PinType.PinCategory == PC_CameraParameter && B->PinType.PinCategory == PC_Self)
	{
		UObjectTreeGraphNode* NodeB = Cast<UObjectTreeGraphNode>(B->GetOwningNode());
		if (NodeB && NodeB->IsObjectA<UCameraRigInterfaceParameter>())
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT("Compatible pin types"));
		}
	}
	else if (A->PinType.PinCategory == PC_Self && B->PinType.PinCategory == PC_CameraParameter)
	{
		UObjectTreeGraphNode* NodeA = Cast<UObjectTreeGraphNode>(A->GetOwningNode());
		if (NodeA && NodeA->IsObjectA<UCameraRigInterfaceParameter>())
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT("Compatible pin types"));
		}
	}

	return Super::CanCreateConnection(A, B);
}

bool UCameraNodeGraphSchema::OnCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	// Try to make a connection between a camera node's parameter pin and a camera rig interface parameter.
	// First, figure out which is which.
	UEdGraphPin* RigInterfacePin = nullptr;
	UEdGraphPin* CameraParameterPin = nullptr;

	if (A->PinType.PinCategory == PC_CameraParameter && B->PinType.PinCategory == PC_Self)
	{
		RigInterfacePin = B;
		CameraParameterPin = A;
	}
	else if (A->PinType.PinCategory == PC_Self && B->PinType.PinCategory == PC_CameraParameter)
	{
		RigInterfacePin = A;
		CameraParameterPin = B;
	}

	if (!RigInterfacePin || !CameraParameterPin)
	{
		return false;
	}

	// Now make sure both nodes are what we expect, and that they have what we need.
	UObjectTreeGraphNode* RigParameterNode = Cast<UObjectTreeGraphNode>(RigInterfacePin->GetOwningNode());
	if (!RigParameterNode)
	{
		return false;
	}
	UCameraRigInterfaceParameter* RigParameter = RigParameterNode->CastObject<UCameraRigInterfaceParameter>();
	if (!RigParameter)
	{
		return false;
	}

	UCameraNodeGraphNode* CameraNodeNode = Cast<UCameraNodeGraphNode>(CameraParameterPin->GetOwningNode());
	if (!CameraNodeNode)
	{
		return false;
	}
	UCameraNode* CameraNode = CameraNodeNode->CastObject<UCameraNode>();
	if (!CameraNode)
	{
		return false;
	}
	FStructProperty* StructProperty = CameraNodeNode->GetCameraParameterPropertyForPin(CameraParameterPin);
	if (!StructProperty)
	{
		return false;
	}

	// Make the connection.
	const FScopedTransaction Transaction(LOCTEXT("ExposeCameraRigParameter", "Expose Camera Rig Parameter"));

	RigParameter->Modify();

	RigParameter->Target = CameraNode;
	RigParameter->TargetPropertyName = StructProperty->GetFName();
	if (RigParameter->InterfaceParameterName.IsEmpty())
	{
		RigParameter->InterfaceParameterName = StructProperty->GetName();
	}

	return true;
}

bool UCameraNodeGraphSchema::OnBreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
	// See if we have a rig parameter connection to break.
	if (TargetPin.PinType.PinCategory == PC_Self || TargetPin.PinType.PinCategory == PC_CameraParameter)
	{
		UEdGraphPin* RigParameterSelfPin = &TargetPin;
		if (TargetPin.PinType.PinCategory == PC_CameraParameter)
		{
			RigParameterSelfPin = TargetPin.LinkedTo[0];
		}

		UObjectTreeGraphNode* RigParameterNode = Cast<UObjectTreeGraphNode>(RigParameterSelfPin->GetOwningNode());
		if (RigParameterNode && RigParameterNode->IsObjectA<UCameraRigInterfaceParameter>())
		{
			const FScopedTransaction Transaction(LOCTEXT("BreakPinLinks", "Break Pin Links"));

			UCameraRigInterfaceParameter* RigParameter = RigParameterNode->CastObject<UCameraRigInterfaceParameter>();

			RigParameter->Modify();

			RigParameter->Target = nullptr;
			RigParameter->TargetPropertyName = NAME_None;
			RigParameter->PrivateVariable = nullptr;

			return true;
		}
	}

	return false;
}

bool UCameraNodeGraphSchema::OnBreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	UCameraRigInterfaceParameter* RigParameter = nullptr;

	if (SourcePin->PinType.PinCategory == PC_Self && TargetPin->PinType.PinCategory == PC_CameraParameter)
	{
		UObjectTreeGraphNode* SourceNode = Cast<UObjectTreeGraphNode>(SourcePin->GetOwningNode());
		if (SourceNode)
		{
			RigParameter = SourceNode->CastObject<UCameraRigInterfaceParameter>();
		}
	}
	else if (SourcePin->PinType.PinCategory == PC_CameraParameter && TargetPin->PinType.PinCategory == PC_Self)
	{
		UObjectTreeGraphNode* TargetNode = Cast<UObjectTreeGraphNode>(TargetPin->GetOwningNode());
		if (TargetNode)
		{ 
			RigParameter = TargetNode->CastObject<UCameraRigInterfaceParameter>();
		}
	}

	if (RigParameter)
	{
		const FScopedTransaction Transaction(LOCTEXT("BreakSinglePinLink", "Break Pin Link"));

		RigParameter->Modify();

		RigParameter->Target = nullptr;
		RigParameter->TargetPropertyName = NAME_None;
		RigParameter->PrivateVariable = nullptr;

		return true;
	}
	
	return false;
}

FCameraNodeGraphSchemaAction_NewInterfaceParameterNode::FCameraNodeGraphSchemaAction_NewInterfaceParameterNode()
{
}

FCameraNodeGraphSchemaAction_NewInterfaceParameterNode::FCameraNodeGraphSchemaAction_NewInterfaceParameterNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, FText InKeywords)
	: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InKeywords)
{
}

UEdGraphNode* FCameraNodeGraphSchemaAction_NewInterfaceParameterNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UObjectTreeGraph* ObjectTreeGraph = Cast<UObjectTreeGraph>(ParentGraph);
	if (!ensure(ObjectTreeGraph))
	{
		return nullptr;
	}

	UCameraRigAsset* CameraRig = Cast<UCameraRigAsset>(ObjectTreeGraph->GetRootObject());
	if (!ensure(CameraRig))
	{
		return nullptr;
	}

	const FScopedTransaction Transaction(LOCTEXT("CreateNewNodeAction", "Create New Node"));

	const FName GraphName = ObjectTreeGraph->GetConfig().GraphName;
	const UObjectTreeGraphSchema* Schema = CastChecked<UObjectTreeGraphSchema>(ParentGraph->GetSchema());

	UCameraRigInterfaceParameter* NewInterfaceParameter = NewObject<UCameraRigInterfaceParameter>(CameraRig, NAME_None, RF_Transactional);
	NewInterfaceParameter->Target = Target;
	NewInterfaceParameter->TargetPropertyName = TargetPropertyName;
	NewInterfaceParameter->InterfaceParameterName = TargetPropertyName.ToString();

	UObjectTreeGraphNode* NewGraphNode = Schema->CreateObjectNode(ObjectTreeGraph, NewInterfaceParameter);

	Schema->AddConnectableObject(ObjectTreeGraph, NewGraphNode);

	NewGraphNode->NodePosX = Location.X;
	NewGraphNode->NodePosY = Location.Y;
	NewGraphNode->OnGraphNodeMoved(false);
	NewGraphNode->AutowireNewNode(FromPin);

	return NewGraphNode;
}

#undef LOCTEXT_NAMESPACE

