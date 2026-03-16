// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraObjectGraphSchemaBase.h"

#include "AssetRegistry/ARFilter.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Build/CameraObjectInterfaceParameterBuilder.h"
#include "Core/BaseCameraObject.h"
#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"  // IWYU pragma: keep
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableCollection.h"
#include "Core/CameraVariableReferences.h"  // IWYU pragma: keep
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Editors/CameraNodeGraphNode.h"
#include "Editors/CameraObjectInterfaceParameterGraphNode.h"
#include "Editors/CameraVariableAssetGraphNode.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "GameplayCamerasEditorSettings.h"
#include "ScopedTransaction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraObjectGraphSchemaBase)

#define LOCTEXT_NAMESPACE "CameraObjectGraphSchemaBase"

const FName UCameraObjectGraphSchemaBase::PC_CameraParameter("CameraParameter");
const FName UCameraObjectGraphSchemaBase::PC_CameraContextData("CameraContextData");

const FName UCameraObjectGraphSchemaBase::PSC_Boolean("Boolean");
const FName UCameraObjectGraphSchemaBase::PSC_Integer("Integer");
const FName UCameraObjectGraphSchemaBase::PSC_Real("Real");
const FName UCameraObjectGraphSchemaBase::PSC_Vector2("Vector2");
const FName UCameraObjectGraphSchemaBase::PSC_Vector3("Vector3");
const FName UCameraObjectGraphSchemaBase::PSC_Vector4("Vector4");
const FName UCameraObjectGraphSchemaBase::PSC_Rotator("Rotator");
const FName UCameraObjectGraphSchemaBase::PSC_Transform("Transform");
const FName UCameraObjectGraphSchemaBase::PSC_BlendableStruct("BlendableStruct");

FName UCameraObjectGraphSchemaBase::GetVariablePinSubCategory(ECameraVariableType VariableType)
{
	// Here we bundle together single- and double-precision types, so that they are connectable. This MUST be kept in
	// sync with the camera variable type conversions! That is: variable types placed in the same pin sub-category
	// below must be convertible to one another!
	switch (VariableType)
	{
		case ECameraVariableType::Boolean:
			return PSC_Boolean;
		case ECameraVariableType::Integer32:
			return PSC_Integer;
		case ECameraVariableType::Float:
		case ECameraVariableType::Double:
			return PSC_Real;
		case ECameraVariableType::Vector2f:
		case ECameraVariableType::Vector2d:
			return PSC_Vector2;
		case ECameraVariableType::Vector3f:
		case ECameraVariableType::Vector3d:
			return PSC_Vector3;
		case ECameraVariableType::Vector4f:
		case ECameraVariableType::Vector4d:
			return PSC_Vector4;
		case ECameraVariableType::Rotator3f:
		case ECameraVariableType::Rotator3d:
			return PSC_Rotator;
		case ECameraVariableType::Transform3f:
		case ECameraVariableType::Transform3d:
			return PSC_Transform;
		case ECameraVariableType::BlendableStruct:
			return PSC_BlendableStruct;
	}
	return NAME_None;
}

FName UCameraObjectGraphSchemaBase::GetDataPinSubCategory(ECameraContextDataType DataType)
{
	static UEnum* DataTypeEnum = StaticEnum<ECameraContextDataType>();
	return DataTypeEnum->GetNameByValue((int64)DataType);
}

UCameraObjectGraphSchemaBase::UCameraObjectGraphSchemaBase(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
	PinColors.Initialize();
}

FObjectTreeGraphConfig UCameraObjectGraphSchemaBase::BuildGraphConfig() const
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	FObjectTreeGraphConfig GraphConfig;

	GraphConfig.DefaultSelfPinName = NAME_None;
	GraphConfig.ConnectableObjectClasses.Add(UCameraObjectInterfaceParameterGetter::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraVariableAssetGetter::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UCameraObjectInterfaceParameterGetter::StaticClass())
		.CanCreateNew(false)  // Only created via a new connection or drag and drop.
		.NodeTitleColor(Settings->CameraRigParameterGetterNodeTitleColor)
		.GraphNodeClass(UCameraObjectInterfaceParameterGraphNode::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UCameraVariableAssetGetter::StaticClass())
		.CanCreateNew(false)  // Only created via a new connection or drag and drop.
		.NodeTitleColor(Settings->CameraVariableGetterNodeTitleColor)
		.GraphNodeClass(UCameraVariableAssetGraphNode::StaticClass());

	OnBuildGraphConfig(GraphConfig);

	return GraphConfig;
}

void UCameraObjectGraphSchemaBase::OnCreateAllNodes(UObjectTreeGraph* InGraph, const FCreatedNodes& InCreatedNodes) const
{
	Super::OnCreateAllNodes(InGraph, InCreatedNodes);

	// Once all nodes are created, add connections between them.
	CreateValueFlowConnections(InGraph, InCreatedNodes);
}

void UCameraObjectGraphSchemaBase::CreateValueFlowConnections(UObjectTreeGraph* InGraph, const FCreatedNodes& InCreatedNodes) const
{
	UBaseCameraObject* CameraObject = Cast<UBaseCameraObject>(InGraph->GetRootObject());
	if (!CameraObject)
	{
		return;
	}

	const FName SelfPinName = UObjectTreeGraphSchema::PC_Self;

	// Add connections for connected objects that are in this graph.
	for (const FCameraObjectConnection& Connection : CameraObject->Connections.Connections)
	{
		if (!Connection.Source || !Connection.Target)
		{
			continue;
		}

		UEdGraphNode* SourceNode = InCreatedNodes.CreatedNodes.FindRef(Connection.Source);
		UEdGraphNode* TargetNode = InCreatedNodes.CreatedNodes.FindRef(Connection.Target);
		// Either both nodes in the connection are in this graph, or none of them are. We shouldn't have
		// one node from one graph connected to a node from another graph, or nodes belonging to two graphs.
		ensure((SourceNode && TargetNode) || (!SourceNode && !TargetNode));
		if (!SourceNode || !TargetNode)
		{
			continue;
		}

		UEdGraphPin* SourcePin = Connection.SourcePropertyName.IsNone() ?
			FindPinByType(SourceNode, PC_Self) : 
			SourceNode->FindPin(Connection.SourcePropertyName);
		if (!SourcePin)
		{
			SourcePin = SourceNode->CreatePin(EGPD_Output, NAME_None, Connection.SourcePropertyName);
			SourcePin->bOrphanedPin = true;
		}

		UEdGraphPin* TargetPin = Connection.TargetPropertyName.IsNone() ? 
			FindPinByType(TargetNode, PC_Self) : 
			TargetNode->FindPin(Connection.TargetPropertyName);
		if (!TargetPin)
		{
			TargetPin = TargetNode->CreatePin(EGPD_Input, NAME_None, Connection.TargetPropertyName);
			TargetPin->bOrphanedPin = true;
		}

		SourcePin->MakeLinkTo(TargetPin);
	}
}

UEdGraphPin* UCameraObjectGraphSchemaBase::FindPinByType(UEdGraphNode* InNode, const FName& InPinCategory) const
{
	UEdGraphPin* const* FoundItem = InNode->Pins.FindByPredicate(
			[InPinCategory](UEdGraphPin* Item)
			{ 
				return Item->PinType.PinCategory == InPinCategory;
			});
	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
}

void UCameraObjectGraphSchemaBase::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	using namespace UE::Cameras;

	// See if we were dragging a camera parameter pin or camera variable reference pin.
	if (const UEdGraphPin* DraggedPin = ContextMenuBuilder.FromPin)
	{
		UCameraNodeGraphNode* CameraNodeNode = Cast<UCameraNodeGraphNode>(DraggedPin->GetOwningNode());

		if (DraggedPin->PinType.PinCategory == PC_CameraParameter ||
				DraggedPin->PinType.PinCategory == PC_CameraContextData)
		{
			ensure(DraggedPin->PinName != NAME_None);

			// If this is an invalid parameter/data pin, don't show any actions.
			if (DraggedPin->bOrphanedPin)
			{
				return;
			}

			// Get information about the pin being dragged. We need to build the whole camera node information.
			FCameraNodeParameterInfos CameraNodeParameters;
			UCameraNode* CameraNode = CameraNodeNode->CastObject<UCameraNode>();
			CameraNodeParameters.BuildFrom(CameraNode);

			// If the pin corresponds to something that can be driven by a camera variable, add actions for creating
			// a getter node for such a variable.
			if (DraggedPin->PinType.PinCategory == PC_CameraParameter)
			{
				if (const FCameraNodeBlendableParameterInfo* BlendableParameter = CameraNodeParameters.FindBlendableParameter(DraggedPin->PinName))
				{
					FARFilter Filter;
					Filter.ClassPaths.Add(UCameraVariableCollection::StaticClass()->GetClassPathName());

					TArray<FAssetData> CameraVariableCollectionAssetDatas;
					IAssetRegistry& AssetRegistry = FAssetRegistryModule::GetRegistry();
					AssetRegistry.GetAssets(Filter, CameraVariableCollectionAssetDatas);

					for (const FAssetData& CameraVariableCollectionAssetData : CameraVariableCollectionAssetDatas)
					{
						TArray<FSoftCameraVariablePtr> SoftVariables;
						UCameraVariableCollection::GetVariablesForType(CameraVariableCollectionAssetData, BlendableParameter->VariableType, SoftVariables);

						const FText CategoryText = FText::Format(
								LOCTEXT("NewCameraVariableGetterActionCategory", "Get variable from {0}"),
								FText::FromName(CameraVariableCollectionAssetData.AssetName));
						for (const FSoftCameraVariablePtr& SoftVariable : SoftVariables)
						{
							TSharedRef<FCameraObjectGraphSchemaAction_AddVariableAssetGetterNode> VariableGetterAction = 
								MakeShared<FCameraObjectGraphSchemaAction_AddVariableAssetGetterNode>(
										CategoryText,
										FText::FromString(SoftVariable.VariableDisplayName),
										LOCTEXT("NewCameraVariableGetterActionToolTip", "Adds a getter node for the given camera variable"));
							VariableGetterAction->SoftVariable = SoftVariable;
							ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(VariableGetterAction.ToSharedPtr()));
						}
					}
				}
			}

			// For all kinds of blendable and data pins, add an action for exposing the parameter as a camera rig
			// interface parameter.
			bool bAddExposeParameterAction = false;
			TSharedRef<FCameraObjectGraphSchemaAction_NewInterfaceParameterNode> ExposeParameterAction = 
				MakeShared<FCameraObjectGraphSchemaAction_NewInterfaceParameterNode>(
						FText::GetEmpty(),
						LOCTEXT("NewInterfaceParameterAction", "Camera Interface Parameter"),
						LOCTEXT("NewInterfaceParameterActionToolTip", "Exposes this parameter on the camera object"));

			if (DraggedPin->PinType.PinCategory == PC_CameraParameter)
			{
				if (const FCameraNodeBlendableParameterInfo* BlendableParameter = CameraNodeParameters.FindBlendableParameter(DraggedPin->PinName))
				{
					FCameraObjectInterfaceParameterDefinition NewParameterDefinition;
					NewParameterDefinition.ParameterType = ECameraObjectInterfaceParameterType::Blendable;
					NewParameterDefinition.VariableType = BlendableParameter->VariableType;
					NewParameterDefinition.BlendableStructType = BlendableParameter->BlendableStructType;
					ExposeParameterAction->ParameterDefinition = NewParameterDefinition;

					bAddExposeParameterAction = true;
				}
			}
			else if (DraggedPin->PinType.PinCategory == PC_CameraContextData)
			{
				if (const FCameraNodeDataParameterInfo* DataParameter = CameraNodeParameters.FindDataParameter(DraggedPin->PinName))
				{
					FCameraObjectInterfaceParameterDefinition NewParameterDefinition;
					NewParameterDefinition.ParameterType = ECameraObjectInterfaceParameterType::Data;
					NewParameterDefinition.DataType = DataParameter->DataType;
					NewParameterDefinition.DataContainerType = DataParameter->DataContainerType;
					NewParameterDefinition.DataTypeObject = DataParameter->DataTypeObject;
					ExposeParameterAction->ParameterDefinition = NewParameterDefinition;

					bAddExposeParameterAction = true;
				}
			}

			if (bAddExposeParameterAction)
			{
				ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(ExposeParameterAction.ToSharedPtr()));
			}

			return;
		}
	}

	Super::GetGraphContextActions(ContextMenuBuilder);
}

const FPinConnectionResponse UCameraObjectGraphSchemaBase::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Check if we are connecting parameter pins of compatible types.
	if ((A->PinType.PinCategory == PC_CameraParameter || A->PinType.PinCategory == PC_CameraContextData) && 
			B->PinType.PinCategory == PC_Self &&
			!A->bOrphanedPin)
	{
		UCameraParameterGetterGraphNodeBase* NodeB = Cast<UCameraParameterGetterGraphNodeBase>(B->GetOwningNode());
		if (NodeB)
		{
			const FEdGraphPinType ParameterGetterPinType = NodeB->GetParameterPinType();
			if (A->PinType.PinSubCategory == ParameterGetterPinType.PinSubCategory &&
					A->PinType.PinSubCategoryObject == ParameterGetterPinType.PinSubCategoryObject)
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Compatible pin types"));
			}
		}
	}
	else if (A->PinType.PinCategory == PC_Self && 
			(B->PinType.PinCategory == PC_CameraParameter || B->PinType.PinCategory == PC_CameraContextData) &&
			!B->bOrphanedPin)
	{
		UCameraParameterGetterGraphNodeBase* NodeA = Cast<UCameraParameterGetterGraphNodeBase>(A->GetOwningNode());
		if (NodeA)
		{
			const FEdGraphPinType ParameterGetterPinType = NodeA->GetParameterPinType();
			if (B->PinType.PinSubCategory == ParameterGetterPinType.PinSubCategory &&
					B->PinType.PinSubCategoryObject == ParameterGetterPinType.PinSubCategoryObject)
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Compatible pin types"));
			}
		}
	}

	return Super::CanCreateConnection(A, B);
}

bool UCameraObjectGraphSchemaBase::OnTryCreateCustomConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	UEdGraphPin* SourcePin = nullptr;
	UEdGraphPin* TargetPin = nullptr;
	if (A->Direction == EGPD_Output && B->Direction == EGPD_Input)
	{
		SourcePin = A;
		TargetPin = B;
	}
	else if (B->Direction == EGPD_Output && A->Direction == EGPD_Input)
	{
		SourcePin = B;
		TargetPin = A;
	}
	if (!ensure(SourcePin && TargetPin))
	{
		return false;
	}

	UObjectTreeGraphNode* SourceNode = Cast<UObjectTreeGraphNode>(SourcePin->GetOwningNode());
	UObjectTreeGraphNode* TargetNode = Cast<UObjectTreeGraphNode>(TargetPin->GetOwningNode());
	if (!SourceNode || !TargetNode)
	{
		return false;
	}

	ensure(SourceNode->GetGraph() == TargetNode->GetGraph());
	UObjectTreeGraph* Graph = Cast<UObjectTreeGraph>(SourceNode->GetGraph());
	if (!ensure(Graph))
	{
		return false;
	}

	UBaseCameraObject* CameraObject = Cast<UBaseCameraObject>(Graph->GetRootObject());
	if (!CameraObject)
	{
		return false;
	}

	// See if we are connecting a parameter getter to a camera node's input property.
	if (UCameraParameterGetterGraphNodeBase* ParameterNode = Cast<UCameraParameterGetterGraphNodeBase>(SourceNode))
	{
		if (SourcePin->PinType.PinCategory == PC_Self && 
			(TargetPin->PinType.PinCategory == PC_CameraParameter || TargetPin->PinType.PinCategory == PC_CameraContextData))
		{
			CameraObject->Modify();

			FCameraObjectConnection NewConnection;
			NewConnection.Source = SourceNode->GetObject();
			NewConnection.Target = TargetNode->GetObject();
			NewConnection.TargetPropertyName = TargetPin->PinName;
			CameraObject->Connections.Connections.Add(NewConnection);

			return true;
		}
	}

	// See if we are connecting one node's output to another node's input.
	bool bDoFlowConnection = false;
	if (SourcePin->PinType.PinCategory == PC_CameraParameter && TargetPin->PinType.PinCategory == PC_CameraParameter)
	{
		bDoFlowConnection = true;
	}
	else if (SourcePin->PinType.PinCategory == PC_CameraContextData &&
			TargetPin->PinType.PinCategory == PC_CameraContextData)
	{
		bDoFlowConnection = true;
	}
	if (bDoFlowConnection)
	{
		CameraObject->Modify();

		FCameraObjectConnection NewConnection;
		NewConnection.Source = SourceNode->GetObject();
		NewConnection.SourcePropertyName = SourcePin->PinName;
		NewConnection.Target = TargetNode->GetObject();
		NewConnection.TargetPropertyName = TargetPin->PinName;
		CameraObject->Connections.Connections.Add(NewConnection);

		return true;
	}

	return false;
}

bool UCameraObjectGraphSchemaBase::OnBreakCustomPinLinks(UEdGraphPin& TargetPin) const
{
	UObjectTreeGraphNode* Node = Cast<UObjectTreeGraphNode>(TargetPin.GetOwningNode());
	if (!Node)
	{
		return false;
	}

	UObjectTreeGraph* Graph = Cast<UObjectTreeGraph>(Node->GetGraph());
	if (!ensure(Graph))
	{
		return false;
	}

	UBaseCameraObject* CameraObject = Cast<UBaseCameraObject>(Graph->GetRootObject());
	if (!CameraObject)
	{
		return false;
	}

	UObject* Object = Node->GetObject();
	if (!Object)
	{
		return false;
	}

	bool bFoundAny = false;
	for (auto It = CameraObject->Connections.Connections.CreateIterator(); It; ++It)
	{
		FCameraObjectConnection& Connection(*It);
		if (
				(Connection.Source == Object && TargetPin.PinName == Connection.SourcePropertyName) ||
				(Connection.Target == Object && TargetPin.PinName == Connection.TargetPropertyName)
		   )
		{
			if (!bFoundAny)
			{
				CameraObject->Modify();
				bFoundAny = true;
			}

			It.RemoveCurrent();
		}
	}
	return bFoundAny;
}

bool UCameraObjectGraphSchemaBase::OnBreakSingleCustomPinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	UObjectTreeGraphNode* SourceNode = Cast<UObjectTreeGraphNode>(SourcePin->GetOwningNode());
	UObjectTreeGraphNode* TargetNode = Cast<UObjectTreeGraphNode>(TargetPin->GetOwningNode());
	if (!SourceNode || !TargetNode)
	{
		return false;
	}

	UObjectTreeGraph* Graph = Cast<UObjectTreeGraph>(SourceNode->GetGraph());
	if (!ensure(Graph))
	{
		return false;
	}

	UBaseCameraObject* CameraObject = Cast<UBaseCameraObject>(Graph->GetRootObject());
	if (!CameraObject)
	{
		return false;
	}

	UObject* SourceObject = SourceNode->GetObject();
	UObject* TargetObject = TargetNode->GetObject();

	bool bFoundAny = false;
	for (auto It = CameraObject->Connections.Connections.CreateIterator(); It; ++It)
	{
		FCameraObjectConnection& Connection(*It);
		if (
				(Connection.Source == SourceObject && SourcePin->PinName == Connection.SourcePropertyName) &&
				(Connection.Target == TargetObject && TargetPin->PinName == Connection.TargetPropertyName)
		   )
		{
			if (!bFoundAny)
			{
				CameraObject->Modify();
				bFoundAny = true;
			}

			It.RemoveCurrent();
		}
	}
	return bFoundAny;
}

FLinearColor UCameraObjectGraphSchemaBase::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	if (PinType.PinCategory == PC_CameraParameter)
	{
		const FName TypeName = PinType.PinSubCategory;
		return PinColors.GetVariablePinColor(TypeName);
	}
	if (PinType.PinCategory == PC_CameraContextData)
	{
		const FName DataTypeName = PinType.PinSubCategory;
		return PinColors.GetContextDataPinColor(DataTypeName);
	}

	return UObjectTreeGraphSchema::GetPinTypeColor(PinType);
}

FCameraObjectGraphSchemaAction_NewInterfaceParameterNode::FCameraObjectGraphSchemaAction_NewInterfaceParameterNode()
{
}

FCameraObjectGraphSchemaAction_NewInterfaceParameterNode::FCameraObjectGraphSchemaAction_NewInterfaceParameterNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, FText InKeywords)
	: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InKeywords)
{
}

UEdGraphNode* FCameraObjectGraphSchemaAction_NewInterfaceParameterNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, FPerformGraphActionLocation Location, bool bSelectNewNode)
{
	using namespace UE::Cameras;

	UObjectTreeGraph* ObjectTreeGraph = Cast<UObjectTreeGraph>(ParentGraph);
	if (!ensure(ObjectTreeGraph))
	{
		return nullptr;
	}

	UBaseCameraObject* CameraObject = Cast<UBaseCameraObject>(ObjectTreeGraph->GetRootObject());
	if (!ensure(CameraObject))
	{
		return nullptr;
	}

	const FScopedTransaction Transaction(LOCTEXT("CreateNewNodeAction", "Create New Node"));

	const UCameraObjectGraphSchemaBase* Schema = CastChecked<UCameraObjectGraphSchemaBase>(ParentGraph->GetSchema());

	CameraObject->Modify();

	// Create a new interface parameter and set it up based on the pin we're creating it from, if any.
	UCameraObjectInterfaceParameterBase* NewInterfaceParameter = nullptr;
	if (ParameterDefinition.ParameterType == ECameraObjectInterfaceParameterType::Blendable)
	{
		UCameraObjectInterfaceBlendableParameter* NewBlendableParameter = NewObject<UCameraObjectInterfaceBlendableParameter>(CameraObject, NAME_None, RF_Transactional);
		NewBlendableParameter->VariableType = ParameterDefinition.VariableType;
		NewBlendableParameter->BlendableStructType = ParameterDefinition.BlendableStructType;
		CameraObject->Interface.BlendableParameters.Add(NewBlendableParameter);
		NewInterfaceParameter = NewBlendableParameter;
	}
	else if (ParameterDefinition.ParameterType == ECameraObjectInterfaceParameterType::Data)
	{
		UCameraObjectInterfaceDataParameter* NewDataParameter = NewObject<UCameraObjectInterfaceDataParameter>(CameraObject, NAME_None, RF_Transactional);
		NewDataParameter->DataType = ParameterDefinition.DataType;
		NewDataParameter->DataContainerType = ParameterDefinition.DataContainerType;
		NewDataParameter->DataTypeObject = ParameterDefinition.DataTypeObject;
		CameraObject->Interface.DataParameters.Add(NewDataParameter);
		NewInterfaceParameter = NewDataParameter;
	}

	if (!ensure(NewInterfaceParameter))
	{
		return nullptr;
	}

	NewInterfaceParameter->InterfaceParameterName = FromPin ? FromPin->GetName() : NewInterfaceParameter->GetName();

	// Set the value on the default parameters property bag to be the same as the value from which
	// we pulled a connection.
	if (FromPin)
	{
		if (UObjectTreeGraphNode* FromGraphNode = Cast<UObjectTreeGraphNode>(FromPin->GetOwningNode()))
		{
			if (UCameraNode* FromCameraNode = FromGraphNode->CastObject<UCameraNode>())
			{
				const FName TargetPropertyName = FromPin->GetFName();
				FCameraObjectInterfaceParameterDefinition NewParameterDefinition;
				NewInterfaceParameter->GetParameterDefinition(NewParameterDefinition);
				FCameraObjectInterfaceParameterBuilder::SetDefaultParameterValue(
						CameraObject, NewParameterDefinition, FromCameraNode, TargetPropertyName, true);
			}
		}
	}

	// Create a getter node for this parameter. It will then get connected inside AutowireNewNode.
	UCameraObjectInterfaceParameterGetter* NewGetter = NewObject<UCameraObjectInterfaceParameterGetter>(CameraObject, NAME_None, RF_Transactional);
	NewGetter->ParameterGuid = NewInterfaceParameter->GetGuid();
	ensure(NewGetter->ParameterGuid.IsValid());

	ObjectTreeGraph->Modify();

	UObjectTreeGraphNode* NewGetterNode = CastChecked<UObjectTreeGraphNode>(Schema->CreateObjectNode(ObjectTreeGraph, NewGetter));

	Schema->AddConnectableObject(ObjectTreeGraph, NewGetter);

	NewGetterNode->NodePosX = Location.X;
	NewGetterNode->NodePosY = Location.Y;
	NewGetterNode->OnGraphNodeMoved(false);

	NewGetterNode->AutowireNewNode(FromPin);

	CameraObject->EventHandlers.Notify(&UE::Cameras::ICameraObjectEventHandler::OnCameraObjectInterfaceChanged);

	return NewGetterNode;
}

FCameraObjectGraphSchemaAction_AddInterfaceParameterGetterNode::FCameraObjectGraphSchemaAction_AddInterfaceParameterGetterNode()
{
}

FCameraObjectGraphSchemaAction_AddInterfaceParameterGetterNode::FCameraObjectGraphSchemaAction_AddInterfaceParameterGetterNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, FText InKeywords)
	: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InKeywords)
{
}

UEdGraphNode* FCameraObjectGraphSchemaAction_AddInterfaceParameterGetterNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, FPerformGraphActionLocation Location, bool bSelectNewNode)
{
	if (!ensure(InterfaceParameter))
	{
		return nullptr;
	}

	UObjectTreeGraph* ObjectTreeGraph = Cast<UObjectTreeGraph>(ParentGraph);
	if (!ensure(ObjectTreeGraph))
	{
		return nullptr;
	}

	UBaseCameraObject* CameraObject = Cast<UBaseCameraObject>(ObjectTreeGraph->GetRootObject());
	if (!ensure(CameraObject))
	{
		return nullptr;
	}

	const FScopedTransaction Transaction(LOCTEXT("CreateNewNodeAction", "Create New Node"));

	const UCameraObjectGraphSchemaBase* Schema = CastChecked<UCameraObjectGraphSchemaBase>(ParentGraph->GetSchema());

	CameraObject->Modify();

	// Create a new getter for the given parameter.
	UCameraObjectInterfaceParameterGetter* NewGetter = NewObject<UCameraObjectInterfaceParameterGetter>(CameraObject, NAME_None, RF_Transactional);
	NewGetter->ParameterGuid = InterfaceParameter->GetGuid();
	ensure(NewGetter->ParameterGuid.IsValid());

	ParentGraph->Modify();
	
	UObjectTreeGraphNode* NewGetterNode = CastChecked<UObjectTreeGraphNode>(Schema->CreateObjectNode(ObjectTreeGraph, NewGetter));

	Schema->AddConnectableObject(ObjectTreeGraph, NewGetter);

	NewGetterNode->NodePosX = Location.X;
	NewGetterNode->NodePosY = Location.Y;
	NewGetterNode->OnGraphNodeMoved(false);

	NewGetterNode->AutowireNewNode(FromPin);

	CameraObject->EventHandlers.Notify(&UE::Cameras::ICameraObjectEventHandler::OnCameraObjectInterfaceChanged);

	return NewGetterNode;
}

FCameraObjectGraphSchemaAction_AddVariableAssetGetterNode::FCameraObjectGraphSchemaAction_AddVariableAssetGetterNode()
{
}

FCameraObjectGraphSchemaAction_AddVariableAssetGetterNode::FCameraObjectGraphSchemaAction_AddVariableAssetGetterNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, FText InKeywords)
	: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InKeywords)
{
}

UEdGraphNode* FCameraObjectGraphSchemaAction_AddVariableAssetGetterNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, FPerformGraphActionLocation Location, bool bSelectNewNode)
{
	UCameraVariableAsset* Variable = SoftVariable.Get();
	if (!ensure(Variable))
	{
		return nullptr;
	}

	UObjectTreeGraph* ObjectTreeGraph = Cast<UObjectTreeGraph>(ParentGraph);
	if (!ensure(ObjectTreeGraph))
	{
		return nullptr;
	}

	UBaseCameraObject* CameraObject = Cast<UBaseCameraObject>(ObjectTreeGraph->GetRootObject());
	if (!ensure(CameraObject))
	{
		return nullptr;
	}

	const FScopedTransaction Transaction(LOCTEXT("CreateNewNodeAction", "Create New Node"));

	const UCameraObjectGraphSchemaBase* Schema = CastChecked<UCameraObjectGraphSchemaBase>(ParentGraph->GetSchema());

	CameraObject->Modify();

	// Create a new getter for the given variable.
	UCameraVariableAssetGetter* NewGetter = NewObject<UCameraVariableAssetGetter>(CameraObject, NAME_None, RF_Transactional);
	NewGetter->Variable = Variable;

	ParentGraph->Modify();
	
	UObjectTreeGraphNode* NewGetterNode = CastChecked<UObjectTreeGraphNode>(Schema->CreateObjectNode(ObjectTreeGraph, NewGetter));

	Schema->AddConnectableObject(ObjectTreeGraph, NewGetter);

	NewGetterNode->NodePosX = Location.X;
	NewGetterNode->NodePosY = Location.Y;
	NewGetterNode->OnGraphNodeMoved(false);

	NewGetterNode->AutowireNewNode(FromPin);

	return NewGetterNode;
}

#undef LOCTEXT_NAMESPACE

