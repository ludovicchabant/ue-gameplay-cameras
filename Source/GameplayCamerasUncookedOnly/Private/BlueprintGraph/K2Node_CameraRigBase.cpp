// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintGraph/K2Node_CameraRigBase.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "Core/CameraRigAsset.h"
#include "EdGraphSchema_K2.h"
#include "Editor.h"
#include "EditorCategoryUtils.h"
#include "GameFramework/BlueprintCameraEvaluationDataRef.h"
#include "GameFramework/CameraRigParameterInterop.h"
#include "GameplayCamerasDelegates.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "Subsystems/AssetEditorSubsystem.h"

#define LOCTEXT_NAMESPACE "K2Node_CameraRigBase"

const FName UK2Node_CameraRigBase::CameraNodeEvaluationResultPinName(TEXT("CameraData"));

UK2Node_CameraRigBase::UK2Node_CameraRigBase(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	using namespace UE::Cameras;
	
	FGameplayCamerasDelegates::OnCameraRigAssetBuilt().AddUObject(this, &UK2Node_CameraRigBase::OnCameraRigAssetBuilt);
}

void UK2Node_CameraRigBase::BeginDestroy()
{
	using namespace UE::Cameras;
	
	FGameplayCamerasDelegates::OnCameraRigAssetBuilt().RemoveAll(this);

	Super::BeginDestroy();
}

void UK2Node_CameraRigBase::AllocateDefaultPins()
{
	using namespace UE::Cameras;

	// Add execution pins.
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Add evalation result pin.
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FBlueprintCameraEvaluationDataRef::StaticStruct(), CameraNodeEvaluationResultPinName);

	Super::AllocateDefaultPins();
}

void UK2Node_CameraRigBase::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	if (!CameraRig)
	{
		const FText MessageText = LOCTEXT("MissingCameraRig", "Invalid camera rig reference inside node @@");
		MessageLog.Error(*MessageText.ToString(), this);
	}
}

bool UK2Node_CameraRigBase::CanJumpToDefinition() const
{
	return CameraRig != nullptr;
}

void UK2Node_CameraRigBase::JumpToDefinition() const
{
	if (CameraRig)
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(CameraRig);
	}
}

FText UK2Node_CameraRigBase::GetMenuCategory() const
{
	const FText BaseCategoryString = FEditorCategoryUtils::BuildCategoryString(
			FCommonEditorCategory::Gameplay, 
			LOCTEXT("CameraRigAssetsEditorCategory", "Camera Rigs"));
	return BaseCategoryString;
}

void UK2Node_CameraRigBase::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		FARFilter Filter;
		Filter.ClassPaths.Add(UCameraRigAsset::StaticClass()->GetClassPathName());

		TArray<FAssetData> CameraRigAssetDatas;
		IAssetRegistry& AssetRegistry = FAssetRegistryModule::GetRegistry();
		AssetRegistry.GetAssets(Filter, CameraRigAssetDatas);

		for (const FAssetData& CameraRigAssetData : CameraRigAssetDatas)
		{
			GetMenuActions(ActionRegistrar, CameraRigAssetData);
		}
	}
	else if (const UCameraRigAsset* CameraRigKeyFilter = Cast<const UCameraRigAsset>(ActionRegistrar.GetActionKeyFilter()))
	{
		const FAssetData CameraRigAssetData(CameraRigKeyFilter);
		GetMenuActions(ActionRegistrar, CameraRigAssetData);
	}
}

UEdGraphPin* UK2Node_CameraRigBase::GetCameraNodeEvaluationResultPin() const
{
	return FindPinChecked(CameraNodeEvaluationResultPinName);
}

bool UK2Node_CameraRigBase::ValidateCameraRigBeforeExpandNode(FKismetCompilerContext& CompilerContext) const
{
	if (!CameraRig)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("ErrorMissingCameraRig", "SetCameraRigParameter node @@ doesn't have a valid camera rig set.").ToString(), this);
		return false;
	}
	return true;
}

void UK2Node_CameraRigBase::OnCameraRigAssetBuilt(const UCameraRigAsset* InBuiltCameraRig)
{
	if (CameraRig && CameraRig == InBuiltCameraRig)
	{
		ReconstructNode();
	}
}

FEdGraphPinType UK2Node_CameraRigBase::MakeBlendableParameterPinType(const UCameraObjectInterfaceBlendableParameter* BlendableParameter)
{
	return MakeBlendableParameterPinType(BlendableParameter->ParameterType, BlendableParameter->BlendableStructType);
}

FEdGraphPinType UK2Node_CameraRigBase::MakeBlendableParameterPinType(ECameraVariableType CameraVariableType, const UScriptStruct* BlendableStructType)
{
	FName PinCategory;
	FName PinSubCategory;
	UObject* PinSubCategoryObject = nullptr;
	switch (CameraVariableType)
	{
		case ECameraVariableType::Boolean:
			PinCategory = UEdGraphSchema_K2::PC_Boolean;
			break;
		case ECameraVariableType::Integer32:
			PinCategory = UEdGraphSchema_K2::PC_Int;
			break;
		case ECameraVariableType::Float:
			// We'll cast down to float.
			PinCategory = UEdGraphSchema_K2::PC_Real;
			PinSubCategory = UEdGraphSchema_K2::PC_Float;
			break;
		case ECameraVariableType::Double:
			PinCategory = UEdGraphSchema_K2::PC_Real;
			PinSubCategory = UEdGraphSchema_K2::PC_Double;
			break;
		case ECameraVariableType::Vector2d:
			PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinSubCategoryObject = TBaseStructure<FVector2D>::Get();
			break;
		case ECameraVariableType::Vector3d:
			PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinSubCategoryObject = TBaseStructure<FVector>::Get();
			break;
		case ECameraVariableType::Vector4d:
			PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinSubCategoryObject = TBaseStructure<FVector4>::Get();
			break;
		case ECameraVariableType::Rotator3d:
			PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinSubCategoryObject = TBaseStructure<FRotator>::Get();
			break;
		case ECameraVariableType::Transform3d:
			PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinSubCategoryObject = TBaseStructure<FTransform>::Get();
			break;
		case ECameraVariableType::BlendableStruct:
			PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinSubCategoryObject = const_cast<UScriptStruct*>(BlendableStructType);
			break;
	}

	FEdGraphPinType PinType;
	PinType.PinCategory = PinCategory;
	PinType.PinSubCategory = PinSubCategory;
	PinType.PinSubCategoryObject = PinSubCategoryObject;
	return PinType;
}

FEdGraphPinType UK2Node_CameraRigBase::MakeDataParameterPinType(const UCameraObjectInterfaceDataParameter* DataParameter)
{
	return MakeDataParameterPinType(DataParameter->DataType, DataParameter->DataContainerType, DataParameter->DataTypeObject);
}

FEdGraphPinType UK2Node_CameraRigBase::MakeDataParameterPinType(ECameraContextDataType CameraContextDataType, ECameraContextDataContainerType CameraContextDataContainerType, const UObject* CameraContextDataTypeObject)
{
	FName PinCategory;
	UObject* PinSubCategoryObject = const_cast<UObject*>(CameraContextDataTypeObject);
	switch (CameraContextDataType)
	{
		case ECameraContextDataType::Name:
			PinCategory = UEdGraphSchema_K2::PC_Name;
			break;
		case ECameraContextDataType::String:
			PinCategory = UEdGraphSchema_K2::PC_String;
			break;
		case ECameraContextDataType::Enum:
			PinCategory = UEdGraphSchema_K2::PC_Enum;
			break;
		case ECameraContextDataType::Struct:
			PinCategory = UEdGraphSchema_K2::PC_Struct;
			break;
		case ECameraContextDataType::Object:
			PinCategory = UEdGraphSchema_K2::PC_Object;
			break;
		case ECameraContextDataType::Class:
			PinCategory = UEdGraphSchema_K2::PC_Class;
			break;
	}

	EPinContainerType PinContainerType = EPinContainerType::None;
	switch (CameraContextDataContainerType)
	{
		case ECameraContextDataContainerType::Array:
			PinContainerType = EPinContainerType::Array;
		default:
			break;
	}

	FEdGraphPinType PinType;
	PinType.PinCategory = PinCategory;
	PinType.PinSubCategoryObject = PinSubCategoryObject;
	PinType.ContainerType = PinContainerType;
	return PinType;
}

#undef LOCTEXT_NAMESPACE

