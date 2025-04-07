// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintGraph/K2Node_SetCameraRigParameters.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Core/CameraRigAsset.h"
#include "EdGraphSchema_K2.h"
#include "GameFramework/CameraRigParameterInterop.h"
#include "K2Node_CallFunction.h"
#include "Kismet/BlueprintInstancedStructLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_SetCameraRigParameters"

UK2Node_SetCameraRigParameters::UK2Node_SetCameraRigParameters(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UK2Node_SetCameraRigParameters::Initialize(const FAssetData& UnloadedCameraRig)
{
	CameraRig = Cast<UCameraRigAsset>(UnloadedCameraRig.GetAsset());
}

void UK2Node_SetCameraRigParameters::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	CreateParameterPins();
}

void UK2Node_SetCameraRigParameters::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) 
{
	// Remove old camera rig pin name. Do this before calling AllocateDefaultPins() because
	// we need our CameraRig property set to (re)create our parameter pins.
	const FName OldCameraRigPinName = TEXT("CameraRig");
	UEdGraphPin** OldCameraRigPin = OldPins.FindByPredicate([&OldCameraRigPinName](UEdGraphPin* OldPin)
			{
				return OldPin->PinName == OldCameraRigPinName;
			});
	if (OldCameraRigPin)
	{
		CameraRig = Cast<UCameraRigAsset>((*OldCameraRigPin)->DefaultObject);
		(*OldCameraRigPin)->DefaultObject = nullptr;
		(*OldCameraRigPin)->DefaultValue.Reset();
	}

	// The camera rig might not be loaded yet when we are rebuilt on startup.
	if (CameraRig)
	{
		PreloadObject(CameraRig);
		for (UCameraObjectInterfaceBlendableParameter* BlendableParameter : CameraRig->Interface.BlendableParameters)
		{
			PreloadObject(BlendableParameter);
			if (BlendableParameter)
			{
				PreloadObject(const_cast<UScriptStruct*>(BlendableParameter->BlendableStructType.Get()));
			}
		}
		for (UCameraObjectInterfaceDataParameter* DataParameter : CameraRig->Interface.DataParameters)
		{
			PreloadObject(DataParameter);
			if (DataParameter)
			{
				PreloadObject(const_cast<UObject*>(DataParameter->DataTypeObject.Get()));
			}
		}
	}

	// Create all default pins, including the parameter pins.
	AllocateDefaultPins();

	// Upgrade old result pin name to new pin name.
	const FName OldResultPinName = TEXT("CameraEvaluationResult");
	UEdGraphPin** OldResultPin = OldPins.FindByPredicate([&OldResultPinName](UEdGraphPin* OldPin)
			{
				return OldPin->PinName == OldResultPinName;
			});
	if (OldResultPin)
	{
		UEdGraphPin* NewResultPin = FindPin(CameraNodeEvaluationResultPinName);
		check(NewResultPin);
		NewResultPin->MovePersistentDataFromOldPin(**OldResultPin);
	}

	RestoreSplitPins(OldPins);
}

FText UK2Node_SetCameraRigParameters::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::Format(
			LOCTEXT("BaseNodeTitle", "SET on {0}"),
			FText::FromString(GetNameSafe(CameraRig)));
}

FText UK2Node_SetCameraRigParameters::GetTooltipText() const
{
	return FText::Format(
			LOCTEXT("NodeTooltip", "Sets the values of all camera rig parameters on {0} on the given evaluation data."),
			FText::FromString(GetNameSafe(CameraRig)));
}

void UK2Node_SetCameraRigParameters::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar, const FAssetData& CameraRigAssetData) const
{
	const FText BaseCategoryString = GetMenuCategory();

	UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
	NodeSpawner->DefaultMenuSignature.Category = BaseCategoryString;
	NodeSpawner->DefaultMenuSignature.MenuName = FText::Format(
			LOCTEXT("SetCameraRigParameterActionMenuName", "Set all parameters on {0}"),
			FText::FromName(CameraRigAssetData.AssetName));
	NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda(
			[CameraRigAssetData](UEdGraphNode* NewNode, bool bIsTemplateNode)
			{
				UK2Node_SetCameraRigParameters* NewSetter = CastChecked<UK2Node_SetCameraRigParameters>(NewNode);
				NewSetter->Initialize(CameraRigAssetData);
			});

	ActionRegistrar.AddBlueprintAction(CameraRigAssetData, NodeSpawner);
}

void UK2Node_SetCameraRigParameters::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	if (!ValidateCameraRigBeforeExpandNode(CompilerContext))
	{
		BreakAllNodeLinks();
		return;
	}

	UEdGraphPin* const CameraNodeEvaluationResultPin = GetCameraNodeEvaluationResultPin();

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
		const UCameraObjectInterfaceBlendableParameter* BlendableParameter = CameraRig->Interface.FindBlendableParameterByName(RigParameterPin->GetName());
		if (!BlendableParameter)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingParameter", "SetCameraRigParameters node @@ is trying to set parameter @@ but camera rig @@ has no such parameter.").ToString(), this, *RigParameterPin->GetName(), CameraRig);
			continue;
		}

		if (!BlendableParameter->PrivateVariableID)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingParameterVariable", "SetCameraRigParameters node @@ needs camera rig @@ to be built.").ToString(), this, CameraRig);
			continue;
		}

		// Make the SetXxxParameter function call node.
		UK2Node_CallFunction* CallSetParameter = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		CallSetParameter->FunctionReference.SetExternalMember(TEXT("SetCameraParameter"), UCameraRigParameterInterop::StaticClass());
		CallSetParameter->AllocateDefaultPins();
		CompilerContext.MessageLog.NotifyIntermediateObjectCreation(CallSetParameter, SourceGraph);

		// Connect the camera evaluation result argument.
		UEdGraphPin* CallSetParameterResultPin = CallSetParameter->FindPinChecked(TEXT("CameraData"));
		CompilerContext.CopyPinLinksToIntermediate(*CameraNodeEvaluationResultPin, *CallSetParameterResultPin);

		// Set the camera rig argument.
		UEdGraphPin* CallSetParameterCameraRigPin = CallSetParameter->FindPinChecked(TEXT("CameraRig"));
		CallSetParameterCameraRigPin->DefaultObject = CameraRig;

		// Set the parameter name argument.
		UEdGraphPin* CallSetParameterNamePin = CallSetParameter->FindPinChecked(TEXT("ParameterName"));
		CallSetParameterNamePin->DefaultValue = BlendableParameter->InterfaceParameterName;

		// Set or connect the parameter value argument.
		UEdGraphPin* CallSetParameterValuePin = CallSetParameter->FindPinChecked(TEXT("NewValue"));
		CallSetParameterValuePin->PinType = RigParameterPin->PinType;
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
		const UCameraObjectInterfaceDataParameter* DataParameter = CameraRig->Interface.FindDataParameterByName(RigParameterPin->GetName());
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

		// Make the SetXxxData function call node.
		UK2Node_CallFunction* CallSetParameter = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		CallSetParameter->FunctionReference.SetExternalMember(TEXT("SetCameraParameter"), UCameraRigParameterInterop::StaticClass());
		CallSetParameter->AllocateDefaultPins();
		CompilerContext.MessageLog.NotifyIntermediateObjectCreation(CallSetParameter, SourceGraph);

		UEdGraphPin* CurExecPin = CallSetParameter->GetExecPin();

		// Connect the camera evaluation result argument.
		UEdGraphPin* CallSetParameterResultPin = CallSetParameter->FindPinChecked(TEXT("CameraData"));
		CompilerContext.CopyPinLinksToIntermediate(*CameraNodeEvaluationResultPin, *CallSetParameterResultPin);

		// Set the camera rig argument.
		UEdGraphPin* CallSetParameterCameraRigPin = CallSetParameter->FindPinChecked(TEXT("CameraRig"));
		CallSetParameterCameraRigPin->DefaultObject = CameraRig;

		// Set the parameter name argument.
		UEdGraphPin* CallSetParameterNamePin = CallSetParameter->FindPinChecked(TEXT("ParameterName"));
		CallSetParameterNamePin->DefaultValue = DataParameter->InterfaceParameterName;

		// Set or connect the parameter value argument.
		UEdGraphPin* CallSetParameterValuePin = CallSetParameter->FindPinChecked(TEXT("NewValue"));
		CallSetParameterValuePin->PinType = RigParameterPin->PinType;
		CallSetParameterValuePin->DefaultValue = RigParameterPin->DefaultValue;
		CallSetParameterValuePin->DefaultTextValue = RigParameterPin->DefaultTextValue;
		CallSetParameterValuePin->AutogeneratedDefaultValue = RigParameterPin->AutogeneratedDefaultValue;
		CallSetParameterValuePin->DefaultObject = RigParameterPin->DefaultObject;
		if (RigParameterPin->LinkedTo.Num() > 0)
		{
			CompilerContext.MovePinLinksToIntermediate(*RigParameterPin, *CallSetParameterValuePin);
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

void UK2Node_SetCameraRigParameters::CreateParameterPins()
{
	BlendableParameterPinNames.Reset();
	DataParameterPinNames.Reset();

	if (!CameraRig)
	{
		return;
	}

	for (const UCameraObjectInterfaceBlendableParameter* BlendableParameter : CameraRig->Interface.BlendableParameters)
	{
		if (!ensure(BlendableParameter))
		{
			continue;
		}

		if (!BlendableParameter->PrivateVariableID)
		{
			// Camera rig isn't fully built.
			continue;
		}
		
		FEdGraphPinType PinType = MakeBlendableParameterPinType(BlendableParameter);
		if (PinType.PinCategory.IsNone())
		{
			// Unsupported type for Blueprints.
			continue;
		}

		UEdGraphPin* NewPin = CreatePin(EGPD_Input, PinType, FName(BlendableParameter->InterfaceParameterName));
		BlendableParameterPinNames.Add(NewPin->PinName);
	}

	for (const UCameraObjectInterfaceDataParameter* DataParameter : CameraRig->Interface.DataParameters)
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

		FEdGraphPinType PinType = MakeDataParameterPinType(DataParameter);
		if (PinType.PinCategory.IsNone())
		{
			// Unsupported type for Blueprints.
			continue;
		}

		UEdGraphPin* NewPin = CreatePin(EGPD_Input, PinType, FName(DataParameter->InterfaceParameterName));
		DataParameterPinNames.Add(NewPin->PinName);
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

#undef LOCTEXT_NAMESPACE

