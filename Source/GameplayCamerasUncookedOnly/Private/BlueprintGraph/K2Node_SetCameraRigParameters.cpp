// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintGraph/K2Node_SetCameraRigParameters.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableAssets.h"
#include "EdGraphSchema_K2.h"
#include "EditorCategoryUtils.h"
#include "GameFramework/BlueprintCameraNodeEvaluationResult.h"
#include "GameFramework/CameraRigParameterInterop.h"
#include "K2Node_CallFunction.h"
#include "Kismet/BlueprintInstancedStructLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_SetCameraRigParameters"

const FName UK2Node_SetCameraRigParameters::CameraRigPinName(TEXT("CameraRig"));
const FName UK2Node_SetCameraRigParameters::CameraNodeEvaluationResultPinName(TEXT("CameraEvaluationResult"));

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
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FBlueprintCameraNodeEvaluationResult::StaticStruct(), CameraNodeEvaluationResultPinName);

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
		for (UCameraRigBlendableParameter* BlendableParameter : CameraRig->Interface.BlendableParameters)
		{
			PreloadObject(BlendableParameter);
			if (BlendableParameter)
			{
				PreloadObject(BlendableParameter->PrivateVariable);
			}
		}
		for (UCameraRigDataParameter* DataParameter : CameraRig->Interface.DataParameters)
		{
			PreloadObject(DataParameter);
			if (DataParameter)
			{
				PreloadObject(const_cast<UObject*>(DataParameter->DataTypeObject.Get()));
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

	UEdGraphPin* const CameraRigPin = FindPinChecked(CameraRigPinName);
	UEdGraphPin* const CameraNodeEvaluationResultPin = FindPinChecked(CameraNodeEvaluationResultPinName);

	UEdGraphPin* OriginalThenPin = GetThenPin();
	UEdGraphPin* PreviousThenPin = nullptr;
	
	// For each blendable and data parameter, we figure out the type of SetXxxParameter function to call on the UCameraRigParameterInterop
	// function library. We then make a K2Node_CallFunction node for it, and connect all its inputs, including connecting the parameter
	// value to whatever our node's corresponding parameter value pin was connected to. As we go, we chain the exec/then pins, basically
	// transforming our SetCameraRigParameters node into a chain of individual setter function calls.

	TArray<UEdGraphPin*> BlendableParameterPins;
	FindBlendableParameterPins(BlendableParameterPins);
	for (UEdGraphPin* RigParameterPin : BlendableParameterPins)
	{
		UCameraRigBlendableParameter* BlendableParameter = CameraRig->Interface.FindBlendableParameterByName(RigParameterPin->GetName());
		if (!BlendableParameter)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingParameter", "SetCameraRigParameters node @@ is trying to set parameter @@ but camera rig @@ has no such parameter.").ToString(), this, *RigParameterPin->GetName(), CameraRig);
			continue;
		}

		if (!BlendableParameter->PrivateVariable)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingParameterVariable", "SetCameraRigParameters node @@ needs camera rig @@ to be built.").ToString(), this, CameraRig);
			continue;
		}

		// Figure out the sort of SetXxxParameter function we want to call for this parameter.
		FName CallSetParameterFuncName;
		switch (BlendableParameter->PrivateVariable->GetVariableType())
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

		// Connect the camera evaluation result argument.
		UEdGraphPin* CallSetParameterResultPin = CallSetParameter->FindPinChecked(TEXT("Result"));
		CompilerContext.CopyPinLinksToIntermediate(*CameraNodeEvaluationResultPin, *CallSetParameterResultPin);

		// Connect the camera rig argument.
		UEdGraphPin* CallSetParameterCameraRigPin = CallSetParameter->FindPinChecked(TEXT("CameraRig"));
		CompilerContext.CopyPinLinksToIntermediate(*CameraRigPin, *CallSetParameterCameraRigPin);

		// Set the parameter name argument.
		UEdGraphPin* CallSetParameterNamePin = CallSetParameter->FindPinChecked(TEXT("ParameterName"));
		CallSetParameterNamePin->DefaultValue = BlendableParameter->InterfaceParameterName;

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

		// Chain the execution.
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

	TArray<UEdGraphPin*> DataParameterPins;
	FindDataParameterPins(DataParameterPins);
	for (UEdGraphPin* RigParameterPin : DataParameterPins)
	{
		UCameraRigDataParameter* DataParameter = CameraRig->Interface.FindDataParameterByName(RigParameterPin->GetName());
		if (!DataParameter)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingParameter", "SetCameraRigParameters node @@ is trying to set parameter @@ but camera rig @@ has no such parameter.").ToString(), this, *RigParameterPin->GetName(), CameraRig);
			continue;
		}

		if (!DataParameter->PrivateDataID.IsValid())
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingParameterVariable", "SetCameraRigParameters node @@ needs camera rig @@ to be built.").ToString(), this, CameraRig);
			continue;
		}

		// Figure out the sort of SetXxxParameter function we want to call for this parameter.
		FName CallSetParameterFuncName;
		switch (DataParameter->DataType)
		{
			case ECameraContextDataType::Name:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetNameParameter);
				break;
			case ECameraContextDataType::String:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetStringParameter);
				break;
			case ECameraContextDataType::Enum:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetEnumParameter);
				break;
			case ECameraContextDataType::Struct:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetStructParameter);
				break;
			case ECameraContextDataType::Object:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetObjectParameter);
				break;
			case ECameraContextDataType::Class:
				CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetClassParameter);
				break;
		}
		if (CallSetParameterFuncName.IsNone())
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ErrorUnsupportedParameterType", "SetCameraRigParameters node @@ is trying to set parameter @@ but it has an unsupported type.").ToString(), this, *RigParameterPin->GetName());
			continue;
		}

		// Make the SetXxxData function call node.
		UK2Node_CallFunction* CallSetParameter = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		CallSetParameter->FunctionReference.SetExternalMember(CallSetParameterFuncName, UCameraRigParameterInterop::StaticClass());
		CallSetParameter->AllocateDefaultPins();
		UEdGraphPin* CurExecPin = CallSetParameter->GetExecPin();

		// Connect the camera evaluation result argument.
		UEdGraphPin* CallSetParameterResultPin = CallSetParameter->FindPinChecked(TEXT("Result"));
		CompilerContext.CopyPinLinksToIntermediate(*CameraNodeEvaluationResultPin, *CallSetParameterResultPin);

		// Connect the camera rig argument.
		UEdGraphPin* CallSetParameterCameraRigPin = CallSetParameter->FindPinChecked(TEXT("CameraRig"));
		CompilerContext.CopyPinLinksToIntermediate(*CameraRigPin, *CallSetParameterCameraRigPin);

		// Set the parameter name argument.
		UEdGraphPin* CallSetParameterNamePin = CallSetParameter->FindPinChecked(TEXT("ParameterName"));
		CallSetParameterNamePin->DefaultValue = DataParameter->InterfaceParameterName;

		// Set or connect the parameter value argument.
		UEdGraphPin* CallSetParameterValuePin = CallSetParameter->FindPinChecked(TEXT("ParameterValue"));
		if (DataParameter->DataType == ECameraContextDataType::Struct)
		{
			// If we are setting a struct, first turn it into an FInstancedStruct. Insert this node in the
			// execution chain, and use that node's Value pin as the pin to set the incoming struct value.
			UK2Node_CallFunction* CallMakeInstancedStruct = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
			CallMakeInstancedStruct->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UBlueprintInstancedStructLibrary, MakeInstancedStruct), UBlueprintInstancedStructLibrary::StaticClass());
			CallMakeInstancedStruct->AllocateDefaultPins();

			CallMakeInstancedStruct->GetReturnValuePin()->MakeLinkTo(CallSetParameterValuePin);
			CallMakeInstancedStruct->GetThenPin()->MakeLinkTo(CurExecPin);
			CurExecPin = CallMakeInstancedStruct->GetExecPin();

			UEdGraphPin* CallMakeInstancedStructValuePin = CallMakeInstancedStruct->FindPinChecked(TEXT("Value"));
			CompilerContext.MovePinLinksToIntermediate(*RigParameterPin, *CallMakeInstancedStructValuePin);
			CallMakeInstancedStruct->PinConnectionListChanged(CallMakeInstancedStructValuePin);
		}
		else
		{
			CallSetParameterValuePin->DefaultValue = RigParameterPin->DefaultValue;
			CallSetParameterValuePin->DefaultTextValue = RigParameterPin->DefaultTextValue;
			CallSetParameterValuePin->AutogeneratedDefaultValue = RigParameterPin->AutogeneratedDefaultValue;
			CallSetParameterValuePin->DefaultObject = RigParameterPin->DefaultObject;
			if (RigParameterPin->LinkedTo.Num() > 0)
			{
				CompilerContext.MovePinLinksToIntermediate(*RigParameterPin, *CallSetParameterValuePin);
			}
		}

		// Set extra type pin for enums.
		if (DataParameter->DataType == ECameraContextDataType::Enum)
		{
			const UEnum* EnumType = CastChecked<const UEnum>(DataParameter->DataTypeObject);
			UEdGraphPin* CallSetParameterEnumTypePin = CallSetParameter->FindPinChecked(TEXT("EnumType"));
			CallSetParameterEnumTypePin->DefaultObject = const_cast<UEnum*>(EnumType);
		}

		// Chain the execution.
		if (PreviousThenPin)
		{
			PreviousThenPin->MakeLinkTo(CurExecPin);
		}
		else
		{
			UEdGraphPin* ThisExecPin = GetExecPin();
			CompilerContext.MovePinLinksToIntermediate(*ThisExecPin, *CurExecPin);
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

UEdGraphPin* UK2Node_SetCameraRigParameters::GetCameraNodeEvaluationResultPin() const
{
	UEdGraphPin* ResultPin = nullptr;
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->PinName == CameraNodeEvaluationResultPinName)
		{
			ResultPin = Pin;
			break;
		}
	}
	check(ResultPin == nullptr || ResultPin->Direction == EGPD_Input);
	return ResultPin;
}

void UK2Node_SetCameraRigParameters::CreatePinsForCameraRig(UCameraRigAsset* CameraRig, TArray<UEdGraphPin*>* CreatedPins)
{
	check(CameraRig);

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	BlendableParameterPinNames.Reset();

	for (const UCameraRigBlendableParameter* BlendableParameter : CameraRig->Interface.BlendableParameters)
	{
		if (!ensure(BlendableParameter))
		{
			continue;
		}

		if (!BlendableParameter->PrivateVariable)
		{
			// Camera rig isn't fully built.
			continue;
		}

		FName NewPinCategory;
		FName NewPinSubCategory;
		UObject* NewPinSubCategoryObject = nullptr;
		switch (BlendableParameter->PrivateVariable->GetVariableType())
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
				FName(BlendableParameter->InterfaceParameterName));
		BlendableParameterPinNames.Add(NewPin->PinName);
		if (CreatedPins)
		{
			CreatedPins->Add(NewPin);
		}
	}

	DataParameterPinNames.Reset();

	for (const UCameraRigDataParameter* DataParameter : CameraRig->Interface.DataParameters)
	{
		if (!ensure(DataParameter))
		{
			continue;
		}
		
		if (!DataParameter->PrivateDataID.IsValid())
		{
			// Camera rig isn't fully built.
			continue;
		}

		FName NewPinCategory;
		UObject* NewPinSubCategoryObject = const_cast<UObject*>(DataParameter->DataTypeObject.Get());
		switch (DataParameter->DataType)
		{
			case ECameraContextDataType::Name:
				NewPinCategory = UEdGraphSchema_K2::PC_Name;
				break;
			case ECameraContextDataType::String:
				NewPinCategory = UEdGraphSchema_K2::PC_String;
				break;
			case ECameraContextDataType::Enum:
				NewPinCategory = UEdGraphSchema_K2::PC_Enum;
				break;
			case ECameraContextDataType::Struct:
				NewPinCategory = UEdGraphSchema_K2::PC_Struct;
				break;
			case ECameraContextDataType::Object:
				NewPinCategory = UEdGraphSchema_K2::PC_Object;
				break;
			case ECameraContextDataType::Class:
				NewPinCategory = UEdGraphSchema_K2::PC_Class;
				break;
		}
		if (NewPinCategory.IsNone())
		{
			// Unsupported type for Blueprints.
			continue;
		}

		UEdGraphPin* NewPin = CreatePin(
				EGPD_Input, 
				NewPinCategory, NAME_None, NewPinSubCategoryObject, 
				FName(DataParameter->InterfaceParameterName));
		DataParameterPinNames.Add(NewPin->PinName);
		if (CreatedPins)
		{
			CreatedPins->Add(NewPin);
		}
	}
}

void UK2Node_SetCameraRigParameters::FindBlendableParameterPins(TArray<UEdGraphPin*>& OutPins) const
{
	for (const FName& PinName : BlendableParameterPinNames)
	{
		UEdGraphPin* Pin = FindPin(PinName);
		if (ensure(Pin))
		{
			OutPins.Add(Pin);
		}
	}
}

void UK2Node_SetCameraRigParameters::FindDataParameterPins(TArray<UEdGraphPin*>& OutPins) const
{
	for (const FName& PinName : DataParameterPinNames)
	{
		UEdGraphPin* Pin = FindPin(PinName);
		if (ensure(Pin))
		{
			OutPins.Add(Pin);
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
	TArray<FName> OldCameraRigPinNames;
	OldCameraRigPinNames.Append(BlendableParameterPinNames);
	OldCameraRigPinNames.Append(DataParameterPinNames);

	TArray<UEdGraphPin*> OldPins = Pins;
	TArray<UEdGraphPin*> OldCameraRigPins;

	for (UEdGraphPin* OldPin : OldPins)
	{
		if (OldCameraRigPinNames.Contains(OldPin->PinName))
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

