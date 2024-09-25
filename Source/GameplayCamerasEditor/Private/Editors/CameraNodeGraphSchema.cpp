// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraNodeGraphSchema.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraNode.h"
#include "Core/CameraNodeHierarchy.h"
#include "Core/CameraRigAsset.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/CameraNodeGraphNode.h"
#include "Editors/CameraRigInterfaceParameterGraphNode.h"
#include "Editors/CameraRigNodeGraphNode.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GameplayCamerasEditorSettings.h"
#include "Nodes/Common/CameraRigCameraNode.h"
#include "Widgets/Notifications/SNotificationList.h"

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
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigCameraNode::StaticClass())
		.GraphNodeClass(UCameraRigNodeGraphNode::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigInterfaceParameter::StaticClass())
		.SelfPinName(NAME_None)  // No self pin name, we just want the title
		.CanCreateNew(false)
		.GraphNodeClass(UCameraRigInterfaceParameterGraphNode::StaticClass());

	return GraphConfig;
}

void UCameraNodeGraphSchema::CollectAllObjects(UObjectTreeGraph* InGraph, TSet<UObject*>& OutAllObjects) const
{
	using namespace UE::Cameras;

	// Only get the graph objects from the root interface.
	CollectAllConnectableObjectsFromRootInterface(InGraph, OutAllObjects, false);

	// See if we are missing objects from AllNodeTreeObjects... if so, add them and notify the user.
	UCameraRigAsset* CameraRig = Cast<UCameraRigAsset>(InGraph->GetRootObject());
	if (CameraRig)
	{
		FCameraNodeHierarchy Hierarchy(CameraRig);

		TSet<UObject*> AllNodeTreeObjects;
		((IObjectTreeGraphRootObject*)CameraRig)->GetConnectableObjects(UCameraRigAsset::NodeTreeGraphName, AllNodeTreeObjects);

		TSet<UObject*> MissingNodeTreeObjects;
		if (Hierarchy.FindMissingConnectableObjects(AllNodeTreeObjects, MissingNodeTreeObjects))
		{
			FNotificationInfo NotificationInfo(
					FText::Format(
						LOCTEXT("AllNodeTreeObjectsMismatch", 
							"Found {0} nodes missing from the internal list. Please re-save the asset."),
						MissingNodeTreeObjects.Num()));
			NotificationInfo.ExpireDuration = 4.0f;
			FSlateNotificationManager::Get().AddNotification(NotificationInfo);

			for (UObject* MissingObject : MissingNodeTreeObjects)
			{
				((IObjectTreeGraphRootObject*)CameraRig)->AddConnectableObject(UCameraRigAsset::NodeTreeGraphName, MissingObject);
				OutAllObjects.Add(MissingObject);
			}
		}
	}
}

void UCameraNodeGraphSchema::OnCreateAllNodes(UObjectTreeGraph* InGraph, const FCreatedNodes& InCreatedNodes) const
{
	Super::OnCreateAllNodes(InGraph, InCreatedNodes);

	UObject* RootObject = InGraph->GetRootObject();
	if (!RootObject)
	{
		return;
	}

	UCameraRigAsset* CameraRig = Cast<UCameraRigAsset>(InGraph->GetRootObject());
	if (!ensure(CameraRig))
	{
		return;
	}
	
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
			const FName PropertyName = CameraNodeNode->GetCameraParameterPropertyForPin(DraggedPin);
			ensure(PropertyName != NAME_None);

			TSharedRef<FCameraNodeGraphSchemaAction_NewInterfaceParameterNode> Action = 
				MakeShared<FCameraNodeGraphSchemaAction_NewInterfaceParameterNode>(
						FText::GetEmpty(),
						LOCTEXT("NewInterfaceParameterAction", "Camera Rig Parameter"),
						LOCTEXT("NewInterfaceParameterActionToolTip", "Exposes this parameter on the camera rig"));
			Action->Target = Cast<UCameraNode>(CameraNodeNode->GetObject());
			Action->TargetPropertyName = PropertyName;
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

bool UCameraNodeGraphSchema::OnApplyConnection(UEdGraphPin* A, UEdGraphPin* B, FDelayedPinActions& Actions) const
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
	const FName PropertyName = CameraNodeNode->GetCameraParameterPropertyForPin(CameraParameterPin);
	if (PropertyName.IsNone())
	{
		return false;
	}

	// Make the connection.
	RigParameter->Modify();

	RigParameter->Target = CameraNode;
	RigParameter->TargetPropertyName = PropertyName;
	if (RigParameter->InterfaceParameterName.IsEmpty())
	{
		RigParameter->InterfaceParameterName = PropertyName.ToString();
	}

	return true;
}

bool UCameraNodeGraphSchema::OnApplyDisconnection(UEdGraphPin* TargetPin, FDelayedPinActions& Actions, bool bIsReconnecting) const
{
	// See if we have a rig parameter connection to break.
	if (TargetPin->PinType.PinCategory == PC_Self || TargetPin->PinType.PinCategory == PC_CameraParameter)
	{
		UEdGraphPin* RigParameterSelfPin = TargetPin;
		if (TargetPin->PinType.PinCategory == PC_CameraParameter)
		{
			RigParameterSelfPin = TargetPin->LinkedTo[0];
		}

		UObjectTreeGraphNode* RigParameterNode = Cast<UObjectTreeGraphNode>(RigParameterSelfPin->GetOwningNode());
		if (!RigParameterNode)
		{
			return false;
		}

		UCameraRigInterfaceParameter* RigParameter = RigParameterNode->CastObject<UCameraRigInterfaceParameter>();
		if (RigParameter)
		{
			RigParameter->Modify();

			RigParameter->Target = nullptr;
			RigParameter->TargetPropertyName = NAME_None;
			RigParameter->PrivateVariable = nullptr;

			return true;
		}
	}

	return false;
}

bool UCameraNodeGraphSchema::OnApplyDisconnection(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin, FDelayedPinActions& Actions) const
{
	UObjectTreeGraphNode* RigParameterNode = nullptr;

	if (SourcePin->PinType.PinCategory == PC_Self && TargetPin->PinType.PinCategory == PC_CameraParameter)
	{
		return OnApplyDisconnection(SourcePin, Actions, false);
	}
	else if (SourcePin->PinType.PinCategory == PC_CameraParameter && TargetPin->PinType.PinCategory == PC_Self)
	{
		return OnApplyDisconnection(TargetPin, Actions, false);
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

	const UObjectTreeGraphSchema* Schema = CastChecked<UObjectTreeGraphSchema>(ParentGraph->GetSchema());

	UCameraRigInterfaceParameter* NewInterfaceParameter = NewObject<UCameraRigInterfaceParameter>(CameraRig, NAME_None, RF_Transactional);
	// The interface parameter's properties will be set correctly inside AutowireNewNode by virtue
	// of getting connected to the dragged camera node pin.

	ObjectTreeGraph->Modify();

	UObjectTreeGraphNode* NewGraphNode = Schema->CreateObjectNode(ObjectTreeGraph, NewInterfaceParameter);

	Schema->AddConnectableObject(ObjectTreeGraph, NewGraphNode);

	NewGraphNode->NodePosX = Location.X;
	NewGraphNode->NodePosY = Location.Y;
	NewGraphNode->OnGraphNodeMoved(false);

	NewGraphNode->AutowireNewNode(FromPin);

	return NewGraphNode;
}

#undef LOCTEXT_NAMESPACE

