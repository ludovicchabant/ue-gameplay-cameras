// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintGraph/K2Node_SetCameraRigParameter.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintGraph/K2Node_CameraRigBase.h"
#include "BlueprintNodeSpawner.h"
#include "Core/CameraRigAsset.h"
#include "EdGraphSchema_K2.h"
#include "EditorCategoryUtils.h"
#include "GameFramework/CameraRigParameterInterop.h"
#include "K2Node_CallFunction.h"
#include "Kismet/BlueprintInstancedStructLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "Misc/EngineVersionComparison.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateIconFinder.h"

#define LOCTEXT_NAMESPACE "K2Node_SetCameraRigParameter"

UK2Node_SetCameraRigParameter::UK2Node_SetCameraRigParameter(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UK2Node_SetCameraRigParameter::Initialize(const FAssetData& UnloadedCameraRig, const FString& InCameraParameterName)
{
	UCameraRigAsset* LoadedCameraRig = Cast<UCameraRigAsset>(UnloadedCameraRig.GetAsset());
	if (ensure(LoadedCameraRig))
	{
		if (const UCameraObjectInterfaceBlendableParameter* BlendableParameter = LoadedCameraRig->Interface.FindBlendableParameterByName(InCameraParameterName))
		{
			Initialize(LoadedCameraRig, InCameraParameterName, BlendableParameter->ParameterType, BlendableParameter->BlendableStructType);
		}
		else if (const UCameraObjectInterfaceDataParameter* DataParameter = LoadedCameraRig->Interface.FindDataParameterByName(InCameraParameterName))
		{
			Initialize(LoadedCameraRig, InCameraParameterName, DataParameter->DataType, DataParameter->DataContainerType, DataParameter->DataTypeObject);
		}
		// else, no parameter of that name found...
	}
}

void UK2Node_SetCameraRigParameter::Initialize(UCameraRigAsset* InCameraRig, const FString& InCameraParameterName, ECameraVariableType InCameraVariableType, const UScriptStruct* InBlendableStructType)
{
	CameraRig = InCameraRig;
	CameraParameterName = InCameraParameterName;
	CameraParameterType = EK2Node_CameraParameterType::Blendable;
	BlendableCameraParameterType = InCameraVariableType;
	BlendableStructType = InBlendableStructType;
}

void UK2Node_SetCameraRigParameter::Initialize(UCameraRigAsset* InCameraRig, const FString& InCameraParameterName, ECameraContextDataType InCameraContextDataType, ECameraContextDataContainerType InCameraContextDataContainerType, const UObject* InCameraContextDataTypeObject)
{
	CameraRig = InCameraRig;
	CameraParameterName = InCameraParameterName;
	CameraParameterType = EK2Node_CameraParameterType::Data;
	DataCameraParameterType = InCameraContextDataType;
	DataCameraParameterContainerType = InCameraContextDataContainerType;
	DataCameraParameterTypeObject = const_cast<UObject*>(InCameraContextDataTypeObject);
}

void UK2Node_SetCameraRigParameter::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Add parameter value pin.
	FEdGraphPinType PinType;
	switch (CameraParameterType)
	{
		case EK2Node_CameraParameterType::Blendable:
			PinType = UK2Node_CameraRigBase::MakeBlendableParameterPinType(BlendableCameraParameterType, BlendableStructType);
			break;
		case EK2Node_CameraParameterType::Data:
			PinType = UK2Node_CameraRigBase::MakeDataParameterPinType(DataCameraParameterType, DataCameraParameterContainerType, DataCameraParameterTypeObject);
			break;
		default:
			PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
			break;
	}
	CreatePin(EGPD_Input, PinType, FName(CameraParameterName));
}

FText UK2Node_SetCameraRigParameter::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::Format(LOCTEXT("BaseNodeTitle", "SET on {0}"), FText::FromString(GetNameSafe(CameraRig)));
}

FText UK2Node_SetCameraRigParameter::GetTooltipText() const
{
	return FText::Format(
			LOCTEXT("NodeTooltip", "Sets the value of camera rig {0}'s parameter {1} on the given evaluation data."),
			FText::FromString(GetNameSafe(CameraRig)),
			FText::FromString(CameraParameterName));
}

FSlateIcon UK2Node_SetCameraRigParameter::GetIconAndTint(FLinearColor& OutColor) const
{
	UEdGraphPin* ParameterValuePin = FindPin(CameraParameterName);
	if (ParameterValuePin)
	{
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		OutColor = K2Schema->GetPinTypeColor(ParameterValuePin->PinType);

#if UE_VERSION_NEWER_THAN_OR_EQUAL(5,6,0)
		if (UStruct* Struct = Cast<UStruct>(ParameterValuePin->PinType.PinSubCategoryObject.Get()))
		{
			return FSlateIconFinder::FindIconForClass(Struct);
		}
#endif
	}

	return FSlateIcon(FAppStyle::GetAppStyleSetName(), "Kismet.AllClasses.VariableIcon");
}

void UK2Node_SetCameraRigParameter::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar, const FAssetData& CameraRigAssetData) const
{
	const FText BaseCategoryString = GetMenuCategory();

	for (TPair<FName, FAssetTagValueRef> It : CameraRigAssetData.TagsAndValues)
	{
		const FName ParameterName = It.Key;

		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		NodeSpawner->DefaultMenuSignature.Category = FText::Join(
				FText::FromString(TEXT("|")), BaseCategoryString, FText::FromName(CameraRigAssetData.AssetName));
		NodeSpawner->DefaultMenuSignature.MenuName = FText::Format(
				LOCTEXT("SetCameraRigParameterActionMenuName", "Set {0}"),
				FText::FromName(ParameterName));
		NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda(
				[ParameterName, CameraRigAssetData](UEdGraphNode* NewNode, bool bIsTemplateNode)
				{
					UK2Node_SetCameraRigParameter* NewSetter = CastChecked<UK2Node_SetCameraRigParameter>(NewNode);
					NewSetter->Initialize(CameraRigAssetData, ParameterName.ToString());
				});

		ActionRegistrar.AddBlueprintAction(CameraRigAssetData, NodeSpawner);
	}
}

void UK2Node_SetCameraRigParameter::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	if (!ValidateCameraRigBeforeExpandNode(CompilerContext))
	{
		BreakAllNodeLinks();
		return;
	}

	if (CameraParameterName.IsEmpty())
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingCameraParameterName", "SetCameraRigParameter node @@ doesn't have a valid camera parameter name set.").ToString(), this);
		BreakAllNodeLinks();
		return;
	}

	// Additional setup is needed for some types.
	UEnum* ExtraEnumTypePinObject = nullptr;
	bool bMakeExtraInstanceStructSetup = false;
	if (DataCameraParameterType == ECameraContextDataType::Enum)
	{
		const UEnum* EnumType = CastChecked<const UEnum>(DataCameraParameterTypeObject);
		ExtraEnumTypePinObject = const_cast<UEnum*>(EnumType);
	}
	else if (DataCameraParameterType == ECameraContextDataType::Struct)
	{
		bMakeExtraInstanceStructSetup = true;
	}

	UEdGraphPin* const CameraNodeEvaluationResultPin = GetCameraNodeEvaluationResultPin();
	UEdGraphPin* const CameraParameterValuePin = FindPinChecked(CameraParameterName);

	// Make the SetXxxParameter function call node.
	UK2Node_CallFunction* CallSetParameter = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallSetParameter->FunctionReference.SetExternalMember(TEXT("SetCameraParameter"), UCameraRigParameterInterop::StaticClass());
	CallSetParameter->AllocateDefaultPins();
	UEdGraphPin* FirstExecPin = CallSetParameter->GetExecPin();

	// Connect the camera evaluation result argument.
	UEdGraphPin* CallSetParameterResultPin = CallSetParameter->FindPinChecked(TEXT("CameraData"));
	CompilerContext.CopyPinLinksToIntermediate(*CameraNodeEvaluationResultPin, *CallSetParameterResultPin);

	// Set the camera rig argument.
	UEdGraphPin* CallSetParameterCameraRigPin = CallSetParameter->FindPinChecked(TEXT("CameraRig"));
	CallSetParameterCameraRigPin->DefaultObject = CameraRig;

	// Set the parameter name argument.
	UEdGraphPin* CallSetParameterNamePin = CallSetParameter->FindPinChecked(TEXT("ParameterName"));
	CallSetParameterNamePin->DefaultValue = CameraParameterName;

	// Set or connect the parameter value argument.
	UEdGraphPin* CallSetParameterValuePin = CallSetParameter->FindPinChecked(TEXT("NewValue"));
	CallSetParameterValuePin->PinType = CameraParameterValuePin->PinType;
	CallSetParameterValuePin->DefaultValue = CameraParameterValuePin->DefaultValue;
	CallSetParameterValuePin->DefaultTextValue = CameraParameterValuePin->DefaultTextValue;
	CallSetParameterValuePin->AutogeneratedDefaultValue = CameraParameterValuePin->AutogeneratedDefaultValue;
	CallSetParameterValuePin->DefaultObject = CameraParameterValuePin->DefaultObject;
	if (CameraParameterValuePin->LinkedTo.Num() > 0)
	{
		CompilerContext.MovePinLinksToIntermediate(*CameraParameterValuePin, *CallSetParameterValuePin);
	}

	// Set extra type pin for enums.
	if (ExtraEnumTypePinObject)
	{
		UEdGraphPin* CallSetParameterEnumTypePin = CallSetParameter->FindPinChecked(TEXT("EnumType"));
		CallSetParameterEnumTypePin->DefaultObject = ExtraEnumTypePinObject;
	}

	// Setup the execution flow.
	UEdGraphPin* ThisExecPin = GetExecPin();
	CompilerContext.MovePinLinksToIntermediate(*ThisExecPin, *FirstExecPin);

	UEdGraphPin* ThisThenPin = GetThenPin();
	UEdGraphPin* CallSetParameterThenPin = CallSetParameter->GetThenPin();
	CompilerContext.MovePinLinksToIntermediate(*ThisThenPin, *CallSetParameterThenPin);

	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE

