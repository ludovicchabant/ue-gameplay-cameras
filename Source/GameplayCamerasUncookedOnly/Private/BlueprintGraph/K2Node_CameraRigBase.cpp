// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintGraph/K2Node_CameraRigBase.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "Core/CameraRigAsset.h"
#include "EdGraphSchema_K2.h"
#include "Editor.h"
#include "EditorCategoryUtils.h"
#include "GameFramework/BlueprintCameraNodeEvaluationResult.h"
#include "GameFramework/CameraRigParameterInterop.h"
#include "GameplayCamerasDelegates.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "Nodes/Framing/CameraFramingZone.h"
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
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FBlueprintCameraNodeEvaluationResult::StaticStruct(), CameraNodeEvaluationResultPinName);

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

FEdGraphPinType UK2Node_CameraRigBase::MakeBlendableParameterPinType(const UCameraRigBlendableParameter* BlendableParameter)
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

FEdGraphPinType UK2Node_CameraRigBase::MakeDataParameterPinType(const UCameraRigDataParameter* DataParameter)
{
	return MakeDataParameterPinType(DataParameter->DataType, DataParameter->DataTypeObject);
}

FEdGraphPinType UK2Node_CameraRigBase::MakeDataParameterPinType(ECameraContextDataType CameraContextDataType, const UObject* CameraContextDataTypeObject)
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

	FEdGraphPinType PinType;
	PinType.PinCategory = PinCategory;
	PinType.PinSubCategoryObject = PinSubCategoryObject;
	return PinType;
}

FName UK2Node_CameraRigBase::GetBlendableParameterInteropSettingFunctionName(const UCameraRigBlendableParameter* BlendableParameter)
{
	if (ensure(BlendableParameter && BlendableParameter->PrivateVariableID))
	{
		return GetBlendableParameterInteropSettingFunctionName(BlendableParameter->ParameterType);
	}
	return NAME_None;
}

FName UK2Node_CameraRigBase::GetBlendableParameterInteropSettingFunctionName(ECameraVariableType CameraVariableType)
{
	// Figure out the sort of SetXxxParameter function we want to call for this parameter.
	FName CallSetParameterFuncName;
	switch (CameraVariableType)
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
		case ECameraVariableType::BlendableStruct:
			CallSetParameterFuncName = GET_FUNCTION_NAME_CHECKED(UCameraRigParameterInterop, SetBlendableStructParameter);
			break;
	}

	return CallSetParameterFuncName;
}

FName UK2Node_CameraRigBase::GetDataParameterInteropSettingFunctionName(const UCameraRigDataParameter* DataParameter)
{
	if (ensure(DataParameter && DataParameter->PrivateDataID.IsValid()))
	{
		return GetDataParameterInteropSettingFunctionName(DataParameter->DataType, DataParameter->DataTypeObject);
	}
	return NAME_None;
}

FName UK2Node_CameraRigBase::GetDataParameterInteropSettingFunctionName(ECameraContextDataType CameraContextDataType, const UObject* CameraContextDataTypeObject)
{
	// Figure out the sort of SetXxxParameter function we want to call for this parameter.
	FName CallSetParameterFuncName;
	switch (CameraContextDataType)
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

	return CallSetParameterFuncName;
}

#undef LOCTEXT_NAMESPACE

