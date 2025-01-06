// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraNodeGraphSchema.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraNode.h"
#include "Core/CameraNodeHierarchy.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableReferences.h"
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Editors/CameraNodeGraphNode.h"
#include "Editors/CameraRigInterfaceParameterGraphNode.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GameplayCamerasEditorSettings.h"
#include "Nodes/Common/ArrayCameraNode.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "ScopedTransaction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraNodeGraphSchema)

#define LOCTEXT_NAMESPACE "CameraNodeGraphSchema"

const FName UCameraNodeGraphSchema::PC_CameraParameter("CameraParameter");
const FName UCameraNodeGraphSchema::PC_CameraVariableReference("CameraVariableReference");
const FName UCameraNodeGraphSchema::PC_CameraContextData("CameraContextData");

UCameraNodeGraphSchema::UCameraNodeGraphSchema(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
	PinColors.Initialize();
}

FObjectTreeGraphConfig UCameraNodeGraphSchema::BuildGraphConfig() const
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	FObjectTreeGraphConfig GraphConfig;
	GraphConfig.GraphName = UCameraRigAsset::NodeTreeGraphName;
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigAsset::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraNode::StaticClass());
	GraphConfig.NonConnectableObjectClasses.Add(UBlendCameraNode::StaticClass());
	GraphConfig.GraphDisplayInfo.PlainName = LOCTEXT("NodeGraphPlainName", "CameraNodes");
	GraphConfig.GraphDisplayInfo.DisplayName = LOCTEXT("NodeGraphDisplayName", "Camera Nodes");
	GraphConfig.DefaultSelfPinName = NAME_None;
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigAsset::StaticClass())
		.OnlyAsRoot()
		.HasSelfPin(false)
		.NodeTitleUsesObjectName(true)
		.NodeTitleColor(Settings->CameraRigAssetTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UCameraNode::StaticClass())
		.StripDisplayNameSuffix(TEXT("Camera Node"))
		.CreateCategoryMetaData(TEXT("CameraNodeCategories"))
		.NodeTitleColor(Settings->CameraNodeTitleColor)
		.GraphNodeClass(UCameraNodeGraphNode::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UArrayCameraNode::StaticClass())
		.OnSetupNewObject(FOnSetupNewObject::CreateLambda([](UObject* NewObject)
				{
					// Add two new pins by default.
					UArrayCameraNode* ArrayNode = CastChecked<UArrayCameraNode>(NewObject);
					ArrayNode->Children.AddDefaulted();
					ArrayNode->Children.AddDefaulted();
				}));

	// Note that we don't add the interface parameter types to the config, we will manage
	// them ourselves.

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
	
	// Add nodes for all interface parameters that have been added to the graph.
	// These nodes are UObjectTreeGraphNode instances, but they are "unmanaged" by the UObjectTreeGraphSchema since
	// their object types are not in the ConnectableObjectClasses. Instead, we manage them ourselves in this schema.
	TArray<TObjectPtr<UCameraRigInterfaceParameterBase>> InterfaceParameters;
	InterfaceParameters.Append(CameraRig->Interface.BlendableParameters);
	InterfaceParameters.Append(CameraRig->Interface.DataParameters);

	for (UCameraRigInterfaceParameterBase* InterfaceParameter : InterfaceParameters)
	{
		if (!InterfaceParameter->bHasGraphNode)
		{
			continue;
		}
		
		UCameraRigInterfaceParameterGraphNode* InterfaceParameterNode = CreateInterfaceParameterNode(InGraph, InterfaceParameter);
		UObjectTreeGraphNode* CameraNodeNode = InCreatedNodes.CreatedNodes.FindRef(InterfaceParameter->Target);
		if (CameraNodeNode)
		{
			UEdGraphPin* InterfaceParameterSelfPin = InterfaceParameterNode->GetSelfPin();
			UEdGraphPin* NodePin = FindPin(CameraNodeNode, InterfaceParameter->TargetPropertyName, NAME_None);
			if (NodePin &&
					(NodePin->PinType.PinCategory == PC_CameraParameter ||
					 NodePin->PinType.PinCategory == PC_CameraVariableReference ||
					 NodePin->PinType.PinCategory == PC_CameraContextData))
			{
				InterfaceParameterSelfPin->MakeLinkTo(NodePin);
			}
			else
			{
				FName ErrorPinCategory = NAME_None;
				if (InterfaceParameter->IsA<UCameraRigBlendableParameter>())
				{
					ErrorPinCategory = PC_CameraParameter;
				}
				else if (InterfaceParameter->IsA<UCameraRigDataParameter>())
				{
					ErrorPinCategory = PC_CameraContextData;
				}

				UEdGraphPin* ErrorPin = CameraNodeNode->CreatePin(
						EGPD_Input, ErrorPinCategory, InterfaceParameter->TargetPropertyName);
				InterfaceParameterSelfPin->MakeLinkTo(ErrorPin);
				ErrorPin->bOrphanedPin = true;
			}
		}
	}
}

UCameraRigInterfaceParameterGraphNode* UCameraNodeGraphSchema::CreateInterfaceParameterNode(UEdGraph* InGraph, UCameraRigInterfaceParameterBase* InterfaceParameter) const
{
	FGraphNodeCreator<UCameraRigInterfaceParameterGraphNode> GraphNodeCreator(*InGraph);
	UCameraRigInterfaceParameterGraphNode* InterfaceParameterNode = GraphNodeCreator.CreateNode(false);
	InterfaceParameterNode->Initialize(InterfaceParameter);
	GraphNodeCreator.Finalize();
	return InterfaceParameterNode;
}

void UCameraNodeGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// See if we were dragging a camera parameter pin or camera variable reference pin.
	if (const UEdGraphPin* DraggedPin = ContextMenuBuilder.FromPin)
	{
		UCameraNodeGraphNode* CameraNodeNode = Cast<UCameraNodeGraphNode>(DraggedPin->GetOwningNode());

		if (DraggedPin->PinType.PinCategory == PC_CameraParameter ||
				DraggedPin->PinType.PinCategory == PC_CameraVariableReference ||
				DraggedPin->PinType.PinCategory == PC_CameraContextData)
		{
			ensure(DraggedPin->PinName != NAME_None);

			// If this is an invalid parameter/data pin, don't show any actions.
			if (DraggedPin->bOrphanedPin)
			{
				return;
			}

			// Find the property being dragged, so we know what kind of parameter to create.
			const UClass* CameraNodeClass = CameraNodeNode->GetObject()->GetClass();
			FProperty* Property = CameraNodeClass->FindPropertyByName(DraggedPin->PinName);

			FCustomCameraNodeParameterInfos CustomParameters;
			ICustomCameraNodeParameterProvider* CustomParameterProvider = Cast<ICustomCameraNodeParameterProvider>(CameraNodeNode->GetObject());
			if (CustomParameterProvider)
			{
				CustomParameterProvider->GetCustomCameraNodeParameters(CustomParameters);
			}

			TSharedRef<FCameraNodeGraphSchemaAction_NewInterfaceParameterNode> Action = 
				MakeShared<FCameraNodeGraphSchemaAction_NewInterfaceParameterNode>(
						FText::GetEmpty(),
						LOCTEXT("NewInterfaceParameterAction", "Camera Rig Parameter"),
						LOCTEXT("NewInterfaceParameterActionToolTip", "Exposes this parameter on the camera rig"));
			Action->Target = Cast<UCameraNode>(CameraNodeNode->GetObject());
			Action->TargetPropertyName = DraggedPin->PinName;

			if (DraggedPin->PinType.PinCategory == PC_CameraParameter ||
				DraggedPin->PinType.PinCategory == PC_CameraVariableReference)
			{
				ECameraVariableType ParameterType;

				if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
				{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
					if ((StructProperty->Struct == F##ValueName##CameraParameter::StaticStruct()) ||\
						(StructProperty->Struct == F##ValueName##CameraVariableReference::StaticStruct()))\
					{\
						ParameterType = ECameraVariableType::ValueName;\
					}\
					else
					UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
					{
						// Unexpected: if there was a camera parameter pin or a variable reference pin, we should
						// have had a camera parameter property or variable reference property!
						ensure(false);
						return;
					}
				}
				else
				{
					FCustomCameraNodeBlendableParameter BlendableParameter;
					if (CustomParameters.FindBlendableParameter(DraggedPin->PinName, BlendableParameter))
					{
						ParameterType = BlendableParameter.ParameterType;
					}
					else
					{
						// Unexpected: a parameter pin was created, but we found no property or custom
						// parameter for it!
						ensure(false);
						return;
					}
				}

				Action->NewNodeType = EInterfaceParameterCreateNodeType::BlendableParameter;
				Action->BlendableParameterType = ParameterType;
			}
			else if (DraggedPin->PinType.PinCategory == PC_CameraContextData)
			{
				ECameraContextDataType DataType;
				const UObject* DataTypeObject = nullptr;
				
				if (Property)
				{
					if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
					{
						DataType = ECameraContextDataType::Name;
					}
					else if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
					{
						DataType = ECameraContextDataType::String;
					}
					else if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
					{
						DataType = ECameraContextDataType::Enum;
						DataTypeObject = EnumProperty->GetEnum();
					}
					else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
					{
						DataType = ECameraContextDataType::Struct;
						DataTypeObject = StructProperty->Struct;
					}
					else if (FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
					{
						DataType = ECameraContextDataType::Class;
						DataTypeObject = ClassProperty->PropertyClass;
					}
					else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
					{
						DataType = ECameraContextDataType::Object;
						DataTypeObject = ObjectProperty->PropertyClass;
					}
					else
					{
						// Unexpected as per previous comments.
						ensure(false);
						return;
					}
				}
				else
				{
					FCustomCameraNodeDataParameter DataParameter;
					if (CustomParameters.FindDataParameter(DraggedPin->PinName, DataParameter))
					{
						DataType = DataParameter.ParameterType;
						DataTypeObject = DataParameter.ParameterTypeObject;
					}
					else
					{
						// Unexpected as per previous comments.
						ensure(false);
						return;
					}
				}

				Action->NewNodeType = EInterfaceParameterCreateNodeType::DataParameter;
				Action->DataParameterType = DataType;
				Action->DataParameterTypeObject = DataTypeObject;
			}

			ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(Action.ToSharedPtr()));

			return;
		}
	}

	Super::GetGraphContextActions(ContextMenuBuilder);
}

const FPinConnectionResponse UCameraNodeGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Check if we are connecting parameter pins of compatible types.
	if ((A->PinType.PinCategory == PC_CameraParameter || 
				A->PinType.PinCategory == PC_CameraVariableReference ||
				A->PinType.PinCategory == PC_CameraContextData) && 
			B->PinType.PinCategory == PC_Self &&
			!A->bOrphanedPin)
	{
		UCameraRigInterfaceParameterGraphNode* NodeB = Cast<UCameraRigInterfaceParameterGraphNode>(B->GetOwningNode());
		if (NodeB)
		{
			UCameraRigBlendableParameter* BlendableParameter = NodeB->CastObject<UCameraRigBlendableParameter>();
			if (BlendableParameter && 
					A->PinType.PinSubCategory == UEnum::GetValueAsName(BlendableParameter->ParameterType))
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT("Compatible pin types"));
			}
			UCameraRigDataParameter* DataParameter = NodeB->CastObject<UCameraRigDataParameter>();
			if (DataParameter && 
					A->PinType.PinSubCategory == UEnum::GetValueAsName(DataParameter->DataType) &&
					A->PinType.PinSubCategoryObject == DataParameter->DataTypeObject)
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT("Compatible pin types"));
			}
		}
	}
	else if (A->PinType.PinCategory == PC_Self && 
			(B->PinType.PinCategory == PC_CameraParameter || 
			 B->PinType.PinCategory == PC_CameraVariableReference ||
			 B->PinType.PinCategory == PC_CameraContextData) &&
			!B->bOrphanedPin)
	{
		UCameraRigInterfaceParameterGraphNode* NodeA = Cast<UCameraRigInterfaceParameterGraphNode>(A->GetOwningNode());
		if (NodeA)
		{
			UCameraRigBlendableParameter* BlendableParameter = NodeA->CastObject<UCameraRigBlendableParameter>();
			if (BlendableParameter && 
					B->PinType.PinSubCategory == UEnum::GetValueAsName(BlendableParameter->ParameterType))
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT("Compatible pin types"));
			}
			UCameraRigDataParameter* DataParameter = NodeA->CastObject<UCameraRigDataParameter>();
			if (DataParameter && 
					B->PinType.PinSubCategory == UEnum::GetValueAsName(DataParameter->DataType) &&
					B->PinType.PinSubCategoryObject == DataParameter->DataTypeObject)
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT("Compatible pin types"));
			}
		}
	}

	return Super::CanCreateConnection(A, B);
}

bool UCameraNodeGraphSchema::OnTryCreateCustomConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	// See if we are in the situation of connecting an interface parameter to a camera node property.
	UEdGraphPin* TargetPin = nullptr;
	UObjectTreeGraphNode* TargetNode = nullptr;
	UCameraRigInterfaceParameterGraphNode* InterfaceParameterNode = nullptr;

	if ((A->PinType.PinCategory == PC_CameraParameter || 
				A->PinType.PinCategory == PC_CameraVariableReference ||
				A->PinType.PinCategory == PC_CameraContextData) && 
			B->PinType.PinCategory == PC_Self)
	{
		TargetPin = A;
		TargetNode = Cast<UObjectTreeGraphNode>(A->GetOwningNode());
		InterfaceParameterNode = Cast<UCameraRigInterfaceParameterGraphNode>(B->GetOwningNode());
	}
	else if (A->PinType.PinCategory == PC_Self && 
			(B->PinType.PinCategory == PC_CameraParameter || 
			 B->PinType.PinCategory == PC_CameraVariableReference ||
			 B->PinType.PinCategory == PC_CameraContextData))
	{
		InterfaceParameterNode = Cast<UCameraRigInterfaceParameterGraphNode>(A->GetOwningNode());
		TargetNode = Cast<UObjectTreeGraphNode>(B->GetOwningNode());
		TargetPin = B;
	}

	if (TargetNode && TargetPin && InterfaceParameterNode)
	{
		UCameraNode* Target = TargetNode->CastObject<UCameraNode>();
		UCameraRigInterfaceParameterBase* InterfaceParameter = InterfaceParameterNode->GetInterfaceParameter();
		if (Target && InterfaceParameter)
		{
			InterfaceParameter->Modify();

			InterfaceParameter->Target = Target;
			InterfaceParameter->TargetPropertyName = TargetPin->PinName;
		}

		return true;
	}

	return false;
}

bool UCameraNodeGraphSchema::OnBreakCustomPinLinks(UEdGraphPin& TargetPin) const
{
	// See if we are in the situation of an interface parameter node being disconnected from 
	// a camera node property pin.
	UCameraRigInterfaceParameterGraphNode* InterfaceParameterNode = nullptr;

	if (TargetPin.PinType.PinCategory == PC_CameraParameter ||
			TargetPin.PinType.PinCategory == PC_CameraVariableReference ||
			TargetPin.PinType.PinCategory == PC_CameraContextData)
	{
		if (TargetPin.LinkedTo.Num() > 0)
		{
			InterfaceParameterNode = Cast<UCameraRigInterfaceParameterGraphNode>(TargetPin.LinkedTo[0]->GetOwningNode());
		}
	}
	else if (TargetPin.PinType.PinCategory == PC_Self)
	{
		InterfaceParameterNode = Cast<UCameraRigInterfaceParameterGraphNode>(TargetPin.GetOwningNode());
	}

	if (InterfaceParameterNode)
	{
		UCameraRigInterfaceParameterBase* InterfaceParameter = InterfaceParameterNode->GetInterfaceParameter();
		if (InterfaceParameter)
		{
			InterfaceParameter->Modify();

			InterfaceParameter->Target = nullptr;
			InterfaceParameter->TargetPropertyName = NAME_None;
		}

		return true;
	}

	return false;
}

bool UCameraNodeGraphSchema::OnBreakSingleCustomPinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	// See if we are in the situation of an interface parameter node being disconnected from 
	// a camera node property pin.
	UCameraRigInterfaceParameterGraphNode* InterfaceParameterNode = nullptr;
	if (SourcePin->PinType.PinCategory == PC_Self)
	{
		InterfaceParameterNode = Cast<UCameraRigInterfaceParameterGraphNode>(SourcePin->GetOwningNode());
	}
	else if (TargetPin->PinType.PinCategory == PC_Self)
	{
		InterfaceParameterNode = Cast<UCameraRigInterfaceParameterGraphNode>(TargetPin->GetOwningNode());
	}

	if (InterfaceParameterNode)
	{
		UCameraRigInterfaceParameterBase* InterfaceParameter = InterfaceParameterNode->GetInterfaceParameter();
		if (InterfaceParameter)
		{
			InterfaceParameter->Modify();

			InterfaceParameter->Target = nullptr;
			InterfaceParameter->TargetPropertyName = NAME_None;
		}

		return true;
	}

	return false;
}

FLinearColor UCameraNodeGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	if (PinType.PinCategory == PC_CameraParameter || PinType.PinCategory == PC_CameraVariableReference)
	{
		const FName TypeName = PinType.PinSubCategory;
		return PinColors.GetPinColor(TypeName);
	}
	if (PinType.PinCategory == PC_CameraContextData)
	{
		return PinColors.GetStructPinColor();
	}

	return UObjectTreeGraphSchema::GetPinTypeColor(PinType);
}

bool UCameraNodeGraphSchema::SafeDeleteNodeFromGraph(UEdGraph* Graph, UEdGraphNode* Node) const
{
	Super::SafeDeleteNodeFromGraph(Graph, Node);

	// Deleting an interface parameter node simply removes its bHasGraphNode flag.
	// To actually delete the parameter, the user needs to remove it from the "parameters" panel.
	if (UCameraRigInterfaceParameterGraphNode* InterfaceParameterNode = Cast<UCameraRigInterfaceParameterGraphNode>(Node))
	{
		if (UCameraRigInterfaceParameterBase* InterfaceParameter = InterfaceParameterNode->GetInterfaceParameter())
		{
			InterfaceParameter->Modify();
			InterfaceParameter->bHasGraphNode = false;
		}
	}

	return true;
}

UEdGraphPin* UCameraNodeGraphSchema::FindPin(UEdGraphNode* InNode, const FName& InPinName, const FName& InPinCategoryName) const
{
	UEdGraphPin* const* FoundItem = InNode->Pins.FindByPredicate(
			[InPinName, InPinCategoryName](UEdGraphPin* Item)
			{ 
				return Item->GetFName() == InPinName &&
					(InPinCategoryName.IsNone() || Item->PinType.PinCategory == InPinCategoryName);
			});
	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
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

	const UCameraNodeGraphSchema* Schema = CastChecked<UCameraNodeGraphSchema>(ParentGraph->GetSchema());

	CameraRig->Modify();

	// Create a new interface parameter and set it up based on the pin we're creating it from, if any.
	UCameraRigInterfaceParameterBase* NewInterfaceParameter = nullptr;
	if (NewNodeType == EInterfaceParameterCreateNodeType::BlendableParameter)
	{
		UCameraRigBlendableParameter* NewBlendableParameter = NewObject<UCameraRigBlendableParameter>(CameraRig, NAME_None, RF_Transactional);
		if (FromPin)
		{
			NewBlendableParameter->ParameterType = (ECameraVariableType)StaticEnum<ECameraVariableType>()->GetValueByName(FromPin->PinType.PinSubCategory);
		}
		CameraRig->Interface.BlendableParameters.Add(NewBlendableParameter);
		NewInterfaceParameter = NewBlendableParameter;
	}
	else if (NewNodeType == EInterfaceParameterCreateNodeType::DataParameter)
	{
		UCameraRigDataParameter* NewDataParameter = NewObject<UCameraRigDataParameter>(CameraRig, NAME_None, RF_Transactional);
		if (FromPin)
		{
			const UEnum* DataTypeEnum = StaticEnum<ECameraContextDataType>();
			NewDataParameter->DataType = (ECameraContextDataType)DataTypeEnum->GetValueByName(FromPin->PinType.PinSubCategory);
			NewDataParameter->DataTypeObject = FromPin->PinType.PinSubCategoryObject.Get();
		}
		CameraRig->Interface.DataParameters.Add(NewDataParameter);
		NewInterfaceParameter = NewDataParameter;
	}

	if (ensure(NewInterfaceParameter))
	{
		NewInterfaceParameter->InterfaceParameterName = FromPin ? FromPin->GetName() : NewInterfaceParameter->GetName();
		NewInterfaceParameter->bHasGraphNode = true;
	}

	// The interface parameter's other properties will be set correctly inside AutowireNewNode by virtue
	// of getting connected to the dragged camera node pin.

	ObjectTreeGraph->Modify();

	UCameraRigInterfaceParameterGraphNode* NewGraphNode = Schema->CreateInterfaceParameterNode(ObjectTreeGraph, NewInterfaceParameter);

	NewGraphNode->NodePosX = Location.X;
	NewGraphNode->NodePosY = Location.Y;
	NewGraphNode->OnGraphNodeMoved(false);

	NewGraphNode->AutowireNewNode(FromPin);

	CameraRig->EventHandlers.Notify(&UE::Cameras::ICameraRigAssetEventHandler::OnCameraRigInterfaceChanged);

	return NewGraphNode;
}

FCameraNodeGraphSchemaAction_AddInterfaceParameterNode::FCameraNodeGraphSchemaAction_AddInterfaceParameterNode()
{
}

FCameraNodeGraphSchemaAction_AddInterfaceParameterNode::FCameraNodeGraphSchemaAction_AddInterfaceParameterNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, FText InKeywords)
	: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InKeywords)
{
}

UEdGraphNode* FCameraNodeGraphSchemaAction_AddInterfaceParameterNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	if (!InterfaceParameter || InterfaceParameter->bHasGraphNode)
	{
		return nullptr;
	}

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

	const UCameraNodeGraphSchema* Schema = CastChecked<UCameraNodeGraphSchema>(ParentGraph->GetSchema());

	// Simply flag the interface parameter has having been added to the graph, and create a node for it.
	InterfaceParameter->Modify();
	InterfaceParameter->bHasGraphNode = true;

	ParentGraph->Modify();
	
	UCameraRigInterfaceParameterGraphNode* NewGraphNode = Schema->CreateInterfaceParameterNode(ParentGraph, InterfaceParameter);

	NewGraphNode->NodePosX = Location.X;
	NewGraphNode->NodePosY = Location.Y;
	NewGraphNode->OnGraphNodeMoved(false);

	NewGraphNode->AutowireNewNode(FromPin);

	CameraRig->EventHandlers.Notify(&UE::Cameras::ICameraRigAssetEventHandler::OnCameraRigInterfaceChanged);

	return NewGraphNode;
}

#undef LOCTEXT_NAMESPACE

