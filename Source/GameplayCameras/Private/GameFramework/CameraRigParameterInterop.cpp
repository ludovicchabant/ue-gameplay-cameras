// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/CameraRigParameterInterop.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraVariableTable.h"
#include "GameFramework/BlueprintCameraVariableTable.h"

#define LOCTEXT_NAMESPACE "CameraRigParameterInterop"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigParameterInterop)

namespace UE::Cameras::Private
{

template<typename VariableAssetType>
void SetCameraRigParameter(FBlueprintCameraVariableTable& VariableTable, VariableAssetType* PrivateVariable, typename VariableAssetType::ValueType Value)
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

}  // namespace UE::Cameras::Private

UCameraRigParameterInterop::UCameraRigParameterInterop(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UCameraRigParameterInterop::SetBooleanParameter(FBlueprintCameraVariableTable& VariableTable, UCameraRigAsset* CameraRig, const FString& ParameterName, bool bParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			VariableTable, 
			Cast<UBooleanCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)), 
			bParameterValue);
}

void UCameraRigParameterInterop::SetIntegerParameter(FBlueprintCameraVariableTable& VariableTable, UCameraRigAsset* CameraRig, const FString& ParameterName, int32 ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			VariableTable, 
			Cast<UInteger32CameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)), 
			ParameterValue);
}

void UCameraRigParameterInterop::SetFloatParameter(FBlueprintCameraVariableTable& VariableTable, UCameraRigAsset* CameraRig, const FString& ParameterName, double ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			VariableTable, 
			Cast<UFloatCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			(float)ParameterValue);
}

void UCameraRigParameterInterop::SetDoubleParameter(FBlueprintCameraVariableTable& VariableTable, UCameraRigAsset* CameraRig, const FString& ParameterName, double ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			VariableTable, 
			Cast<UDoubleCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetVector2Parameter(FBlueprintCameraVariableTable& VariableTable, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector2D ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			VariableTable, 
			Cast<UVector2dCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetVector3Parameter(FBlueprintCameraVariableTable& VariableTable, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			VariableTable, 
			Cast<UVector3dCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetVector4Parameter(FBlueprintCameraVariableTable& VariableTable, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector4 ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			VariableTable, 
			Cast<UVector4dCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetRotatorParameter(FBlueprintCameraVariableTable& VariableTable, UCameraRigAsset* CameraRig, const FString& ParameterName, FRotator ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			VariableTable, 
			Cast<URotator3dCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetTransformParameter(FBlueprintCameraVariableTable& VariableTable, UCameraRigAsset* CameraRig, const FString& ParameterName, FTransform ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			VariableTable, 
			Cast<UTransform3dCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

UCameraVariableAsset* UCameraRigParameterInterop::GetParameterPrivateVariable(UCameraRigAsset* CameraRig, const FString& ParameterName)
{
	UCameraRigInterfaceParameter* InterfaceParameter = CameraRig->Interface.FindInterfaceParameterByName(ParameterName);
	if (!InterfaceParameter)
	{
		const FText Text = LOCTEXT("NoSuchParameter", "No parameter '{0}' found on camera rig '{1}'. Setting this camera variable table value will most probably accomplish nothing.");
		FFrame::KismetExecutionMessage(*FText::Format(Text, FText::FromString(ParameterName), FText::FromString(CameraRig->GetPathName())).ToString(), ELogVerbosity::Warning);
		return nullptr;
	}

	if (!InterfaceParameter->PrivateVariable)
	{
		const FText Text = LOCTEXT("CameraRigNeedsBuilding", "Parameter '{0}' isn't built. Please build camera rig '{1}'.");
		FFrame::KismetExecutionMessage(*FText::Format(Text, FText::FromString(ParameterName), FText::FromString(CameraRig->GetPathName())).ToString(), ELogVerbosity::Warning);
		return nullptr;
	}

	return InterfaceParameter->PrivateVariable;
}

#undef LOCTEXT_NAMESPACE

