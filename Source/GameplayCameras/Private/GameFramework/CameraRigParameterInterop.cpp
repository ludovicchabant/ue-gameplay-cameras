// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/CameraRigParameterInterop.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraVariableTable.h"
#include "GameFramework/BlueprintCameraContextDataTable.h"
#include "GameFramework/BlueprintCameraNodeEvaluationResult.h"
#include "GameFramework/BlueprintCameraVariableTable.h"
#include "Templates/UnrealTypeTraits.h"

#define LOCTEXT_NAMESPACE "CameraRigParameterInterop"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigParameterInterop)

namespace UE::Cameras::Private
{

template<typename VariableAssetType>
void SetCameraRigBlendableParameter(FBlueprintCameraVariableTable VariableTable, VariableAssetType* PrivateVariable, typename VariableAssetType::ValueType Value)
{
	if (!VariableTable.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid camera variable table was passed."), ELogVerbosity::Error);
		return;
	}
	if (PrivateVariable == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("No camera rig was passed."), ELogVerbosity::Error);
		return;
	}

	VariableTable.GetVariableTable()->SetValue(PrivateVariable, Value, true);
}

UCameraVariableAsset* GetParameterPrivateVariable(UCameraRigAsset* CameraRig, const FString& ParameterName)
{
	UCameraRigBlendableParameter* BlendableParameter = CameraRig->Interface.FindBlendableParameterByName(ParameterName);
	if (!BlendableParameter)
	{
		const FText Text = LOCTEXT("NoSuchBlendableParameter", "No parameter '{0}' found on camera rig '{1}'. Setting this camera variable table value will most probably accomplish nothing.");
		FFrame::KismetExecutionMessage(*FText::Format(Text, FText::FromString(ParameterName), FText::FromString(CameraRig->GetPathName())).ToString(), ELogVerbosity::Warning);
		return nullptr;
	}

	if (!BlendableParameter->PrivateVariable)
	{
		const FText Text = LOCTEXT("CameraRigNeedsBuilding", "Parameter '{0}' isn't built. Please build camera rig '{1}'.");
		FFrame::KismetExecutionMessage(*FText::Format(Text, FText::FromString(ParameterName), FText::FromString(CameraRig->GetPathName())).ToString(), ELogVerbosity::Warning);
		return nullptr;
	}

	return BlendableParameter->PrivateVariable;
}

bool ValidateSetCameraRigDataParameter(FBlueprintCameraContextDataTable& ContextDataTable, FCameraContextDataID DataID)
{
	if (!ContextDataTable.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid camera variable table was passed."), ELogVerbosity::Error);
		return false;
	}
	if (!DataID.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("No camera rig was passed."), ELogVerbosity::Error);
		return false;
	}
	return true;
}

FCameraContextDataID GetParameterPrivateDataID(UCameraRigAsset* CameraRig, const FString& ParameterName)
{
	UCameraRigDataParameter* DataParameter = CameraRig->Interface.FindDataParameterByName(ParameterName);
	if (!DataParameter)
	{
		const FText Text = LOCTEXT("NoSuchDataParameter", "No parameter '{0}' found on camera rig '{1}'. Setting this data will most probably accomplish nothing.");
		FFrame::KismetExecutionMessage(*FText::Format(Text, FText::FromString(ParameterName), FText::FromString(CameraRig->GetPathName())).ToString(), ELogVerbosity::Warning);
		return FCameraContextDataID();
	}

	if (!DataParameter->PrivateDataID)
	{
		const FText Text = LOCTEXT("CameraRigNeedsBuilding", "Parameter '{0}' isn't built. Please build camera rig '{1}'.");
		FFrame::KismetExecutionMessage(*FText::Format(Text, FText::FromString(ParameterName), FText::FromString(CameraRig->GetPathName())).ToString(), ELogVerbosity::Warning);
		return FCameraContextDataID();
	}

	return DataParameter->PrivateDataID;
}

}  // namespace UE::Cameras::Private

UCameraRigParameterInterop::UCameraRigParameterInterop(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UCameraRigParameterInterop::SetBooleanParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, bool ParameterValue)
{
	UE::Cameras::Private::SetCameraRigBlendableParameter(
			Result.GetVariableTable(), 
			Cast<UBooleanCameraVariable>(UE::Cameras::Private::GetParameterPrivateVariable(CameraRig, ParameterName)), 
			ParameterValue);
}

void UCameraRigParameterInterop::SetIntegerParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, int32 ParameterValue)
{
	UE::Cameras::Private::SetCameraRigBlendableParameter(
			Result.GetVariableTable(), 
			Cast<UInteger32CameraVariable>(UE::Cameras::Private::GetParameterPrivateVariable(CameraRig, ParameterName)), 
			ParameterValue);
}

void UCameraRigParameterInterop::SetFloatParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, double ParameterValue)
{
	UE::Cameras::Private::SetCameraRigBlendableParameter(
			Result.GetVariableTable(), 
			Cast<UFloatCameraVariable>(UE::Cameras::Private::GetParameterPrivateVariable(CameraRig, ParameterName)),
			(float)ParameterValue);
}

void UCameraRigParameterInterop::SetDoubleParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, double ParameterValue)
{
	UE::Cameras::Private::SetCameraRigBlendableParameter(
			Result.GetVariableTable(), 
			Cast<UDoubleCameraVariable>(UE::Cameras::Private::GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetVector2Parameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector2D ParameterValue)
{
	UE::Cameras::Private::SetCameraRigBlendableParameter(
			Result.GetVariableTable(), 
			Cast<UVector2dCameraVariable>(UE::Cameras::Private::GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetVector3Parameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector ParameterValue)
{
	UE::Cameras::Private::SetCameraRigBlendableParameter(
			Result.GetVariableTable(), 
			Cast<UVector3dCameraVariable>(UE::Cameras::Private::GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetVector4Parameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector4 ParameterValue)
{
	UE::Cameras::Private::SetCameraRigBlendableParameter(
			Result.GetVariableTable(), 
			Cast<UVector4dCameraVariable>(UE::Cameras::Private::GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetRotatorParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, FRotator ParameterValue)
{
	UE::Cameras::Private::SetCameraRigBlendableParameter(
			Result.GetVariableTable(), 
			Cast<URotator3dCameraVariable>(UE::Cameras::Private::GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetTransformParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, FTransform ParameterValue)
{
	UE::Cameras::Private::SetCameraRigBlendableParameter(
			Result.GetVariableTable(), 
			Cast<UTransform3dCameraVariable>(UE::Cameras::Private::GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetNameParameter(UPARAM(Ref) FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, FName ParameterValue)
{
	const FCameraContextDataID DataID(UE::Cameras::Private::GetParameterPrivateDataID(CameraRig, ParameterName));
	FBlueprintCameraContextDataTable ContextDataTable(Result.GetContextDataTable());
	if (UE::Cameras::Private::ValidateSetCameraRigDataParameter(ContextDataTable, DataID))
	{
		ContextDataTable.GetContextDataTable()->SetNameData(DataID, ParameterValue);
	}
}

void UCameraRigParameterInterop::SetStringParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, FString ParameterValue)
{
	const FCameraContextDataID DataID(UE::Cameras::Private::GetParameterPrivateDataID(CameraRig, ParameterName));
	FBlueprintCameraContextDataTable ContextDataTable(Result.GetContextDataTable());
	if (UE::Cameras::Private::ValidateSetCameraRigDataParameter(ContextDataTable, DataID))
	{
		ContextDataTable.GetContextDataTable()->SetStringData(DataID, ParameterValue);
	}
}

void UCameraRigParameterInterop::SetEnumParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, const UEnum* EnumType, uint8 ParameterValue)
{
	const FCameraContextDataID DataID(UE::Cameras::Private::GetParameterPrivateDataID(CameraRig, ParameterName));
	FBlueprintCameraContextDataTable ContextDataTable(Result.GetContextDataTable());
	if (UE::Cameras::Private::ValidateSetCameraRigDataParameter(ContextDataTable, DataID))
	{
		ContextDataTable.GetContextDataTable()->SetEnumData(DataID, EnumType, ParameterValue);
	}
}

void UCameraRigParameterInterop::SetStructParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, const FInstancedStruct& ParameterValue)
{
	const FCameraContextDataID DataID(UE::Cameras::Private::GetParameterPrivateDataID(CameraRig, ParameterName));
	FBlueprintCameraContextDataTable ContextDataTable(Result.GetContextDataTable());
	if (UE::Cameras::Private::ValidateSetCameraRigDataParameter(ContextDataTable, DataID))
	{
		ContextDataTable.GetContextDataTable()->SetInstancedStructData(DataID, ParameterValue);
	}
}

void UCameraRigParameterInterop::SetObjectParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, UObject* ParameterValue)
{
	const FCameraContextDataID DataID(UE::Cameras::Private::GetParameterPrivateDataID(CameraRig, ParameterName));
	FBlueprintCameraContextDataTable ContextDataTable(Result.GetContextDataTable());
	if (UE::Cameras::Private::ValidateSetCameraRigDataParameter(ContextDataTable, DataID))
	{
		ContextDataTable.GetContextDataTable()->SetObjectData(DataID, ParameterValue);
	}
}

void UCameraRigParameterInterop::SetClassParameter(FBlueprintCameraNodeEvaluationResult& Result, UCameraRigAsset* CameraRig, const FString& ParameterName, UClass* ParameterValue)
{
	const FCameraContextDataID DataID(UE::Cameras::Private::GetParameterPrivateDataID(CameraRig, ParameterName));
	FBlueprintCameraContextDataTable ContextDataTable(Result.GetContextDataTable());
	if (UE::Cameras::Private::ValidateSetCameraRigDataParameter(ContextDataTable, DataID))
	{
		ContextDataTable.GetContextDataTable()->SetClassData(DataID, ParameterValue);
	}
}

#undef LOCTEXT_NAMESPACE

