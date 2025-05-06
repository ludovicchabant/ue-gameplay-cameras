// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraNodeGraphSchema.h"

#include "Core/BaseCameraObject.h"
#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Core/CameraVariableReferences.h"
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Editors/CameraNodeGraphNode.h"
#include "Editors/CameraObjectInterfaceParameterGraphNode.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "GameplayCamerasEditorSettings.h"
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

void UCameraNodeGraphSchema::BuildBaseGraphConfig(FObjectTreeGraphConfig& OutGraphConfig) const
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	OutGraphConfig.GraphDisplayInfo.PlainName = LOCTEXT("NodeGraphPlainName", "CameraNodes");
	OutGraphConfig.GraphDisplayInfo.DisplayName = LOCTEXT("NodeGraphDisplayName", "Camera Nodes");
	OutGraphConfig.DefaultSelfPinName = NAME_None;
	OutGraphConfig.ObjectClassConfigs.Emplace(UCameraNode::StaticClass())
		.StripDisplayNameSuffix(TEXT("Camera Node"))
		.CreateCategoryMetaData(TEXT("CameraNodeCategories"))
		.NodeTitleColor(Settings->CameraNodeTitleColor)
		.GraphNodeClass(UCameraNodeGraphNode::StaticClass());

	// Note that we don't add the interface parameter types to the config, we will manage
	// them ourselves.
}

void UCameraNodeGraphSchema::CollectAllObjects(UObjectTreeGraph* InGraph, TSet<UObject*>& OutAllObjects) const
{
	using namespace UE::Cameras;

	// Only get the graph objects from the root interface.
	CollectAllConnectableObjectsFromRootInterface(InGraph, OutAllObjects, false);
}

void UCameraNodeGraphSchema::OnCreateAllNodes(UObjectTreeGraph* InGraph, const FCreatedNodes& InCreatedNodes) const
{
	Super::OnCreateAllNodes(InGraph, InCreatedNodes);

	UObject* RootObject = InGraph->GetRootObject();
	if (!RootObject)
	{
		return;
	}

	UBaseCameraObject* CameraObject = Cast<UBaseCameraObject>(InGraph->GetRootObject());
	if (!ensure(CameraObject))
	{
		return;
	}
	
	// Add nodes for all interface parameters that have been added to the graph.
	// These nodes are UObjectTreeGraphNode instances, but they are "unmanaged" by the UObjectTreeGraphSchema since
	// their object types are not in the ConnectableObjectClasses. Instead, we manage them ourselves in this schema.
	TArray<TObjectPtr<UCameraObjectInterfaceParameterBase>> InterfaceParameters;
	InterfaceParameters.Append(CameraObject->Interface.BlendableParameters);
	InterfaceParameters.Append(CameraObject->Interface.DataParameters);

	for (UCameraObjectInterfaceParameterBase* InterfaceParameter : InterfaceParameters)
	{
		if (!InterfaceParameter->bHasGraphNode)
		{
			continue;
		}
		
		UCameraObjectInterfaceParameterGraphNode* InterfaceParameterNode = CreateInterfaceParameterNode(InGraph, InterfaceParameter);
		UObjectTreeGraphNode* CameraNodeNode = Cast<UObjectTreeGraphNode>(InCreatedNodes.CreatedNodes.FindRef(InterfaceParameter->Target));
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
				if (InterfaceParameter->IsA<UCameraObjectInterfaceBlendableParameter>())
				{
					ErrorPinCategory = PC_CameraParameter;
				}
				else if (InterfaceParameter->IsA<UCameraObjectInterfaceDataParameter>())
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

UCameraObjectInterfaceParameterGraphNode* UCameraNodeGraphSchema::CreateInterfaceParameterNode(UEdGraph* InGraph, UCameraObjectInterfaceParameterBase* InterfaceParameter) const
{
	FGraphNodeCreator<UCameraObjectInterfaceParameterGraphNode> GraphNodeCreator(*InGraph);
	UCameraObjectInterfaceParameterGraphNode* InterfaceParameterNode = GraphNodeCreator.CreateNode(false);
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
						LOCTEXT("NewInterfaceParameterAction", "Camera Interface Parameter"),
						LOCTEXT("NewInterfaceParameterActionToolTip", "Exposes this parameter on the camera object"));

			if (DraggedPin->PinType.PinCategory == PC_CameraParameter ||
				DraggedPin->PinType.PinCategory == PC_CameraVariableReference)
			{
				ECameraVariableType VariableType;
				const UScriptStruct* BlendableStructType = nullptr;

				FCustomCameraNodeBlendableParameter BlendableParameter;
				if (CustomParameters.FindBlendableParameter(DraggedPin->PinName, BlendableParameter))
				{
					VariableType = BlendableParameter.ParameterType;
					BlendableStructType = BlendableParameter.BlendableStructType;
				}
				else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
				{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
					if ((StructProperty->Struct == F##ValueName##CameraParameter::StaticStruct()) ||\
						(StructProperty->Struct == F##ValueName##CameraVariableReference::StaticStruct()))\
					{\
						VariableType = ECameraVariableType::ValueName;\
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
					// Unexpected as per previous comments.
					ensure(false);
					return;
				}

				FCameraObjectInterfaceParameterDefinition NewParameterDefinition;
				NewParameterDefinition.ParameterType = ECameraObjectInterfaceParameterType::Blendable;
				NewParameterDefinition.VariableType = VariableType;
				NewParameterDefinition.BlendableStructType = BlendableStructType;
				Action->ParameterDefinition = NewParameterDefinition;
			}
			else if (DraggedPin->PinType.PinCategory == PC_CameraContextData)
			{
				ECameraContextDataType DataType;
				ECameraContextDataContainerType DataContainerType = ECameraContextDataContainerType::None;
				const UObject* DataTypeObject = nullptr;
				
				FCustomCameraNodeDataParameter DataParameter;
				if (CustomParameters.FindDataParameter(DraggedPin->PinName, DataParameter))
				{
					DataType = DataParameter.ParameterType;
					DataContainerType = DataParameter.ParameterContainerType;
					DataTypeObject = DataParameter.ParameterTypeObject;
				}
				else if (Property)
				{
					if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
					{
						DataContainerType = ECameraContextDataContainerType::Array;
						Property = ArrayProperty->Inner;
					}

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
					// Unexpected as per previous comments.
					ensure(false);
					return;
				}

				FCameraObjectInterfaceParameterDefinition NewParameterDefinition;
				NewParameterDefinition.ParameterType = ECameraObjectInterfaceParameterType::Data;
				NewParameterDefinition.DataType = DataType;
				NewParameterDefinition.DataContainerType = DataContainerType;
				NewParameterDefinition.DataTypeObject = DataTypeObject;
				Action->ParameterDefinition = NewParameterDefinition;
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
		UCameraObjectInterfaceParameterGraphNode* NodeB = Cast<UCameraObjectInterfaceParameterGraphNode>(B->GetOwningNode());
		if (NodeB)
		{
			UCameraObjectInterfaceBlendableParameter* BlendableParameter = NodeB->CastObject<UCameraObjectInterfaceBlendableParameter>();
			if (BlendableParameter && 
					A->PinType.PinSubCategory == UEnum::GetValueAsName(BlendableParameter->ParameterType))
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT("Compatible pin types"));
			}
			UCameraObjectInterfaceDataParameter* DataParameter = NodeB->CastObject<UCameraObjectInterfaceDataParameter>();
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
		UCameraObjectInterfaceParameterGraphNode* NodeA = Cast<UCameraObjectInterfaceParameterGraphNode>(A->GetOwningNode());
		if (NodeA)
		{
			UCameraObjectInterfaceBlendableParameter* BlendableParameter = NodeA->CastObject<UCameraObjectInterfaceBlendableParameter>();
			if (BlendableParameter && 
					B->PinType.PinSubCategory == UEnum::GetValueAsName(BlendableParameter->ParameterType))
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT("Compatible pin types"));
			}
			UCameraObjectInterfaceDataParameter* DataParameter = NodeA->CastObject<UCameraObjectInterfaceDataParameter>();
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
	UCameraObjectInterfaceParameterGraphNode* InterfaceParameterNode = nullptr;

	if ((A->PinType.PinCategory == PC_CameraParameter || 
				A->PinType.PinCategory == PC_CameraVariableReference ||
				A->PinType.PinCategory == PC_CameraContextData) && 
			B->PinType.PinCategory == PC_Self)
	{
		TargetPin = A;
		TargetNode = Cast<UObjectTreeGraphNode>(A->GetOwningNode());
		InterfaceParameterNode = Cast<UCameraObjectInterfaceParameterGraphNode>(B->GetOwningNode());
	}
	else if (A->PinType.PinCategory == PC_Self && 
			(B->PinType.PinCategory == PC_CameraParameter || 
			 B->PinType.PinCategory == PC_CameraVariableReference ||
			 B->PinType.PinCategory == PC_CameraContextData))
	{
		InterfaceParameterNode = Cast<UCameraObjectInterfaceParameterGraphNode>(A->GetOwningNode());
		TargetNode = Cast<UObjectTreeGraphNode>(B->GetOwningNode());
		TargetPin = B;
	}

	if (TargetNode && TargetPin && InterfaceParameterNode)
	{
		UCameraNode* Target = TargetNode->CastObject<UCameraNode>();
		UCameraObjectInterfaceParameterBase* InterfaceParameter = InterfaceParameterNode->GetInterfaceParameter();
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
	UCameraObjectInterfaceParameterGraphNode* InterfaceParameterNode = nullptr;

	if (TargetPin.PinType.PinCategory == PC_CameraParameter ||
			TargetPin.PinType.PinCategory == PC_CameraVariableReference ||
			TargetPin.PinType.PinCategory == PC_CameraContextData)
	{
		if (TargetPin.LinkedTo.Num() > 0)
		{
			InterfaceParameterNode = Cast<UCameraObjectInterfaceParameterGraphNode>(TargetPin.LinkedTo[0]->GetOwningNode());
		}
	}
	else if (TargetPin.PinType.PinCategory == PC_Self)
	{
		InterfaceParameterNode = Cast<UCameraObjectInterfaceParameterGraphNode>(TargetPin.GetOwningNode());
	}

	if (InterfaceParameterNode)
	{
		UCameraObjectInterfaceParameterBase* InterfaceParameter = InterfaceParameterNode->GetInterfaceParameter();
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
	UCameraObjectInterfaceParameterGraphNode* InterfaceParameterNode = nullptr;
	if (SourcePin->PinType.PinCategory == PC_Self)
	{
		InterfaceParameterNode = Cast<UCameraObjectInterfaceParameterGraphNode>(SourcePin->GetOwningNode());
	}
	else if (TargetPin->PinType.PinCategory == PC_Self)
	{
		InterfaceParameterNode = Cast<UCameraObjectInterfaceParameterGraphNode>(TargetPin->GetOwningNode());
	}

	if (InterfaceParameterNode)
	{
		UCameraObjectInterfaceParameterBase* InterfaceParameter = InterfaceParameterNode->GetInterfaceParameter();
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
	if (UCameraObjectInterfaceParameterGraphNode* InterfaceParameterNode = Cast<UCameraObjectInterfaceParameterGraphNode>(Node))
	{
		if (UCameraObjectInterfaceParameterBase* InterfaceParameter = InterfaceParameterNode->GetInterfaceParameter())
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

UEdGraphNode* FCameraNodeGraphSchemaAction_NewInterfaceParameterNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, FPerformGraphActionLocation Location, bool bSelectNewNode)
{
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

	const UCameraNodeGraphSchema* Schema = CastChecked<UCameraNodeGraphSchema>(ParentGraph->GetSchema());

	CameraObject->Modify();

	// Create a new interface parameter and set it up based on the pin we're creating it from, if any.
	UCameraObjectInterfaceParameterBase* NewInterfaceParameter = nullptr;
	if (ParameterDefinition.ParameterType == ECameraObjectInterfaceParameterType::Blendable)
	{
		UCameraObjectInterfaceBlendableParameter* NewBlendableParameter = NewObject<UCameraObjectInterfaceBlendableParameter>(CameraObject, NAME_None, RF_Transactional);
		NewBlendableParameter->ParameterType = ParameterDefinition.VariableType;
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

	if (ensure(NewInterfaceParameter))
	{
		NewInterfaceParameter->InterfaceParameterName = FromPin ? FromPin->GetName() : NewInterfaceParameter->GetName();
		NewInterfaceParameter->bHasGraphNode = true;
	}

	// The interface parameter's other properties will be set correctly inside AutowireNewNode by virtue
	// of getting connected to the dragged camera node pin.

	ObjectTreeGraph->Modify();

	UCameraObjectInterfaceParameterGraphNode* NewGraphNode = Schema->CreateInterfaceParameterNode(ObjectTreeGraph, NewInterfaceParameter);

	NewGraphNode->NodePosX = Location.X;
	NewGraphNode->NodePosY = Location.Y;
	NewGraphNode->OnGraphNodeMoved(false);

	NewGraphNode->AutowireNewNode(FromPin);

	CameraObject->EventHandlers.Notify(&UE::Cameras::ICameraObjectEventHandler::OnCameraObjectInterfaceChanged);

	return NewGraphNode;
}

FCameraNodeGraphSchemaAction_AddInterfaceParameterNode::FCameraNodeGraphSchemaAction_AddInterfaceParameterNode()
{
}

FCameraNodeGraphSchemaAction_AddInterfaceParameterNode::FCameraNodeGraphSchemaAction_AddInterfaceParameterNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, FText InKeywords)
	: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InKeywords)
{
}

UEdGraphNode* FCameraNodeGraphSchemaAction_AddInterfaceParameterNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, FPerformGraphActionLocation Location, bool bSelectNewNode)
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

	UBaseCameraObject* CameraObject = Cast<UBaseCameraObject>(ObjectTreeGraph->GetRootObject());
	if (!ensure(CameraObject))
	{
		return nullptr;
	}

	const FScopedTransaction Transaction(LOCTEXT("CreateNewNodeAction", "Create New Node"));

	const UCameraNodeGraphSchema* Schema = CastChecked<UCameraNodeGraphSchema>(ParentGraph->GetSchema());

	// Simply flag the interface parameter has having been added to the graph, and create a node for it.
	InterfaceParameter->Modify();
	InterfaceParameter->bHasGraphNode = true;

	ParentGraph->Modify();
	
	UCameraObjectInterfaceParameterGraphNode* NewGraphNode = Schema->CreateInterfaceParameterNode(ParentGraph, InterfaceParameter);

	NewGraphNode->NodePosX = Location.X;
	NewGraphNode->NodePosY = Location.Y;
	NewGraphNode->OnGraphNodeMoved(false);

	NewGraphNode->AutowireNewNode(FromPin);

	CameraObject->EventHandlers.Notify(&UE::Cameras::ICameraObjectEventHandler::OnCameraObjectInterfaceChanged);

	return NewGraphNode;
}

#undef LOCTEXT_NAMESPACE

