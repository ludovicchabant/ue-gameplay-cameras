// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintGraph/K2Node_SetCameraRigParameters.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableAssets.h"
#include "EdGraphSchema_K2.h"
#include "EditorCategoryUtils.h"
#include "GameFramework/BlueprintCameraVariableTable.h"
#include "GameFramework/CameraRigParameterInterop.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "K2Node_SetCameraRigParameters"

const FName UK2Node_SetCameraRigParameters::CameraRigPinName(TEXT("CameraRig"));
const FName UK2Node_SetCameraRigParameters::CameraVariableTablePinName(TEXT("CameraVariableTable"));

UK2Node_SetCameraRigParameters::UK2Node_SetCameraRigParameters(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UK2Node_SetCameraRigParameters::AllocateDefaultPins()
{
	using namespace UE::Cameras;

	// Add execution pins.
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Add evalation result pin.
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FBlueprintCameraVariableTable::StaticStruct(), CameraVariableTablePinName);

	// Add camera rig pin.
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UCameraRigAsset::StaticClass(), CameraRigPinName);
	
	Super::AllocateDefaultPins();
}

void UK2Node_SetCameraRigParameters::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) 
{
	AllocateDefaultPins();

	TArrayView<UEdGraphPin* const> PinsToSearch = OldPins;
	if (UCameraRigAsset* CameraRig = GetCameraRig(&PinsToSearch))
	{
		// The camera rig might not be loaded yet.
		PreloadObject(CameraRig);
		for (UCameraRigInterfaceParameter* InterfaceParameter : CameraRig->Interface.InterfaceParameters)
		{
			PreloadObject(InterfaceParameter);
			if (InterfaceParameter)
			{
				PreloadObject(InterfaceParameter->PrivateVariable);
			}
		}

		CreatePinsForCameraRig(CameraRig);
	}

	RestoreSplitPins(OldPins);
}

void UK2Node_SetCameraRigParameters::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

	if (UCameraRigAsset* CameraRig = GetCameraRig())
	{
		CreatePinsForCameraRig(CameraRig);
	}
}

void UK2Node_SetCameraRigParameters::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin && (Pin->PinName == CameraRigPinName))
	{
		OnCameraRigChanged();
	}
}

void UK2Node_SetCameraRigParameters::PinDefaultValueChanged(UEdGraphPin* ChangedPin) 
{
	if (ChangedPin && (ChangedPin->PinName == CameraRigPinName))
	{
		OnCameraRigChanged();
	}
}

FText UK2Node_SetCameraRigParameters::GetTooltipText() const
{
	return LOCTEXT("NodeTooltip", "Sets values for the exposed parameters on the given camera rig.");
}

FText UK2Node_SetCameraRigParameters::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("BaseNodeTitle", "Set Camera Rig Parameters");
}

void UK2Node_SetCameraRigParameters::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_SetCameraRigParameters::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Gameplay);
}

void UK2Node_SetCameraRigParameters::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UCameraRigAsset* CameraRig = GetCameraRig();

	if (!CameraRig)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingCameraRig", "SetCameraRigParameters node @@ doesn't have a camera rig set.").ToString(), this);
		BreakAllNodeLinks();
		return;
	}

	// Get all the pins that correspond to parameters we want to override.
	TArray<UEdGraphPin*> RigParameterPins;
	GetCameraRigParameterPins(RigParameterPins);

	UEdGraphPin* const CameraRigPin = FindPinChecked(CameraRigPinName);
	UEdGraphPin* const CameraVariableTablePin = FindPinChecked(CameraVariableTablePinName);

	UEdGraphPin* OriginalThenPin = GetThenPin();
	UEdGraphPin* PreviousThenPin = nullptr;

	for (UEdGraphPin* RigParameterPin : RigParameterPins)
	{
		UCameraRigInterfaceParameter* InterfaceParameter = CameraRig->Interface.FindInterfaceParameterByName(RigParameterPin->GetName());
		if (!InterfaceParameter)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingParameter", "SetCameraRigParameters node @@ is trying to set parameter @@ but camera rig @@ has no such parameter.").ToString(), this, *RigParameterPin->GetName(), CameraRig);
			continue;
		}

		if (!InterfaceParameter->PrivateVariable)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingParameterVariable", "SetCameraRigParameters node @@ needs camera rig @@ to be built.").ToString(), this, CameraRig);
			continue;
		}

		// Figure out the sort of SetXxxParameter function we want to call for this parameter.
		FName CallSetParameterFuncName;
		switch (InterfaceParameter->PrivateVariable->GetVariableType())
		{
			case ECameraVariableType::Boolean:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetBooleanParameter);
				break;
			case ECameraVariableType::Integer32:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetIntegerParameter);
				break;
			case ECameraVariableType::Float:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetFloatParameter);
				break;
			case ECameraVariableType::Double:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetDoubleParameter);
				break;
			case ECameraVariableType::Vector2d:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetVector2Parameter);
				break;
			case ECameraVariableType::Vector3d:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetVector3Parameter);
				break;
			case ECameraVariableType::Vector4d:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetVector4Parameter);
				break;
			case ECameraVariableType::Rotator3d:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetRotatorParameter);
				break;
			case ECameraVariableType::Transform3d:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetTransformParameter);
				break;
		}
		if (CallSetParameterFuncName.IsNone())
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ErrorUnsupportedParameterType", "SetCameraRigParameters node @@ is trying to set parameter @@ but it has an unsupported type.").ToString(), this, *RigParameterPin->GetName());
			continue;
		}

		// Make the SetXxxParameter function call node.
		UK2Node_CallFunction* CallSetParameter = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		CallSetParameter->FunctionReference.SetExternalMember(CallSetParameterFuncName, UCameraRigParameterInterop::StaticClass());
		CallSetParameter->AllocateDefaultPins();

		// Connect the variable table pin that specifies where the parameter should be overriden.
		UEdGraphPin* CallSetParameterVariableTablePin = CallSetParameter->FindPinChecked(TEXT("VariableTable"));
		CompilerContext.CopyPinLinksToIntermediate(*CameraVariableTablePin, *CallSetParameterVariableTablePin);

		// Connect the camera rig argument.
		UEdGraphPin* CallSetParameterCameraRigPin = CallSetParameter->FindPinChecked(TEXT("CameraRig"));
		CompilerContext.CopyPinLinksToIntermediate(*CameraRigPin, *CallSetParameterCameraRigPin);

		// Set the parameter name argument.
		UEdGraphPin* CallSetParameterNamePin = CallSetParameter->FindPinChecked(TEXT("ParameterName"));
		CallSetParameterNamePin->DefaultValue = InterfaceParameter->InterfaceParameterName;

		// Set or connect the parameter value argument.
		UEdGraphPin* CallSetParameterValuePin = CallSetParameter->FindPinChecked(TEXT("ParameterValue"));
		CallSetParameterValuePin->DefaultValue = RigParameterPin->DefaultValue;
		CallSetParameterValuePin->DefaultTextValue = RigParameterPin->DefaultTextValue;
		CallSetParameterValuePin->AutogeneratedDefaultValue = RigParameterPin->AutogeneratedDefaultValue;
		CallSetParameterValuePin->DefaultObject = RigParameterPin->DefaultObject;
		if (RigParameterPin->LinkedTo.Num() > 0)
		{
			CompilerContext.MovePinLinksToIntermediate(*RigParameterPin, *CallSetParameterValuePin);
		}

		// Connect the SetXxxParameter node to the chain of other similar nodes. The SetCameraRigParameters node
		// effectively transforms into a chain of individual setter function calls.
		UEdGraphPin* CallSetParameterExecPin = CallSetParameter->GetExecPin();
		if (PreviousThenPin)
		{
			PreviousThenPin->MakeLinkTo(CallSetParameterExecPin);
		}
		else
		{
			UEdGraphPin* ThisExecPin = GetExecPin();
			CompilerContext.MovePinLinksToIntermediate(*ThisExecPin, *CallSetParameterExecPin);
		}

		PreviousThenPin = CallSetParameter->GetThenPin();
	}

	// Connect the last node if necessary.
	if (OriginalThenPin && PreviousThenPin && OriginalThenPin->LinkedTo.Num() > 0)
	{
		CompilerContext.MovePinLinksToIntermediate(*OriginalThenPin, *PreviousThenPin);
	}

	BreakAllNodeLinks();
}

UEdGraphPin* UK2Node_SetCameraRigParameters::GetCameraRigPin(TArrayView<UEdGraphPin* const>* InPinsToSearch) const
{
	TArrayView<UEdGraphPin* const> PinsToSearch = MakeArrayView(Pins);
	if (InPinsToSearch)
	{
		PinsToSearch = *InPinsToSearch;
	}

	UEdGraphPin* CameraRigPin = nullptr;
	for (UEdGraphPin* Pin : PinsToSearch)
	{
		if (Pin && Pin->PinName == CameraRigPinName)
		{
			CameraRigPin = Pin;
			break;
		}
	}
	check(CameraRigPin == nullptr || CameraRigPin->Direction == EGPD_Input);
	return CameraRigPin;
}

UEdGraphPin* UK2Node_SetCameraRigParameters::GetCameraEvaluationResultPin() const
{
	UEdGraphPin* ResultPin = nullptr;
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->PinName == CameraRigPinName)
		{
			ResultPin = Pin;
			break;
		}
	}
	check(ResultPin == nullptr || ResultPin->Direction == EGPD_Input);
	return ResultPin;
}

void UK2Node_SetCameraRigParameters::GetCameraRigParameterPins(TArray<UEdGraphPin*>& OutParameterPins) const
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (IsCameraRigParameterPin(Pin))
		{
			OutParameterPins.Add(Pin);
		}
	}
}

bool UK2Node_SetCameraRigParameters::IsCameraRigParameterPin(UEdGraphPin* Pin) const
{
	return Pin->PinName != UEdGraphSchema_K2::PN_Execute &&
		Pin->PinName != UEdGraphSchema_K2::PN_Then &&
		Pin->PinName != UEdGraphSchema_K2::PN_ReturnValue &&
		Pin->PinName != CameraRigPinName &&
		Pin->PinName != CameraVariableTablePinName;
}

void UK2Node_SetCameraRigParameters::CreatePinsForCameraRig(UCameraRigAsset* CameraRig, TArray<UEdGraphPin*>* CreatedPins)
{
	check(CameraRig);

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	for (UCameraRigInterfaceParameter* InterfaceParameter : CameraRig->Interface.InterfaceParameters)
	{
		if (!ensure(InterfaceParameter))
		{
			continue;
		}

		if (!InterfaceParameter->PrivateVariable)
		{
			// Camera rig isn't fully built.
			continue;
		}

		FName NewPinCategory;
		FName NewPinSubCategory;
		UObject* NewPinSubCategoryObject = nullptr;
		switch (InterfaceParameter->PrivateVariable->GetVariableType())
		{
			case ECameraVariableType::Boolean:
				NewPinCategory = UEdGraphSchema_K2::PC_Boolean;
				break;
			case ECameraVariableType::Integer32:
				NewPinCategory = UEdGraphSchema_K2::PC_Int;
				break;
			case ECameraVariableType::Float:
				// We'll cast down to float.
				NewPinCategory = UEdGraphSchema_K2::PC_Real;
				NewPinSubCategory = UEdGraphSchema_K2::PC_Float;
				break;
			case ECameraVariableType::Double:
				NewPinCategory = UEdGraphSchema_K2::PC_Real;
				NewPinSubCategory = UEdGraphSchema_K2::PC_Double;
				break;
			case ECameraVariableType::Vector2d:
				NewPinCategory = UEdGraphSchema_K2::PC_Struct;
				NewPinSubCategoryObject = TBaseStructure<FVector2D>::Get();
				break;
			case ECameraVariableType::Vector3d:
				NewPinCategory = UEdGraphSchema_K2::PC_Struct;
				NewPinSubCategoryObject = TBaseStructure<FVector>::Get();
				break;
			case ECameraVariableType::Vector4d:
				NewPinCategory = UEdGraphSchema_K2::PC_Struct;
				NewPinSubCategoryObject = TBaseStructure<FVector4>::Get();
				break;
			case ECameraVariableType::Rotator3d:
				NewPinCategory = UEdGraphSchema_K2::PC_Struct;
				NewPinSubCategoryObject = TBaseStructure<FRotator>::Get();
				break;
			case ECameraVariableType::Transform3d:
				NewPinCategory = UEdGraphSchema_K2::PC_Struct;
				NewPinSubCategoryObject = TBaseStructure<FTransform>::Get();
				break;
		}
		if (NewPinCategory.IsNone())
		{
			// Unsupported type for Blueprints.
			continue;
		}

		UEdGraphPin* NewPin = CreatePin(
				EGPD_Input, 
				NewPinCategory, NewPinSubCategory, NewPinSubCategoryObject, 
				FName(InterfaceParameter->InterfaceParameterName));
		if (CreatedPins)
		{
			CreatedPins->Add(NewPin);
		}
	}
}

UCameraRigAsset* UK2Node_SetCameraRigParameters::GetCameraRig(TArrayView<UEdGraphPin* const>* InPinsToSearch) const
{
	TArrayView<UEdGraphPin* const> PinsToSearch(Pins);
	if (InPinsToSearch)
	{
		PinsToSearch = *InPinsToSearch;
	}

	UEdGraphPin* CameraRigPin = GetCameraRigPin(&PinsToSearch);
	if (CameraRigPin && CameraRigPin->DefaultObject && CameraRigPin->LinkedTo.Num() == 0)
	{
		return CastChecked<UCameraRigAsset>(CameraRigPin->DefaultObject);
	}
	else if (CameraRigPin && CameraRigPin->LinkedTo.Num() > 0)
	{
		if (UEdGraphPin* CameraRigSource = CameraRigPin->LinkedTo[0])
		{
			return Cast<UCameraRigAsset>(CameraRigSource->PinType.PinSubCategoryObject.Get());
		}
	}
	return nullptr;
}

void UK2Node_SetCameraRigParameters::OnCameraRigChanged()
{
	TArray<UEdGraphPin*> OldPins = Pins;
	TArray<UEdGraphPin*> OldCameraRigPins;

	for (UEdGraphPin* OldPin : OldPins)
	{
		if (IsCameraRigParameterPin(OldPin))
		{
			Pins.Remove(OldPin);
			OldCameraRigPins.Add(OldPin);
		}
	}

	TArray<UEdGraphPin*> NewPins;
	if (UCameraRigAsset* CameraRig = GetCameraRig())
	{
		CreatePinsForCameraRig(CameraRig, &NewPins);
	}

	RewireOldPinsToNewPins(OldCameraRigPins, Pins, nullptr);

	GetGraph()->NotifyGraphChanged();
	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

#undef LOCTEXT_NAMESPACE

