// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/CameraRigParameterInterop.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraVariableTable.h"
#include "GameFramework/CameraEvaluationResultInterop.h"

#define LOCTEXT_NAMESPACE "CameraRigParameterInterop"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigParameterInterop)

namespace UE::Cameras::Private
{

template<typename VariableAssetType>
void SetCameraRigParameter(UCameraEvaluationResultInterop* ResultInterop, VariableAssetType* PrivateVariable, typename VariableAssetType::ValueType Value)
{
	if (ResultInterop == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("No camera evaluation result was passed."), ELogVerbosity::Error);
		return;
	}
	if (PrivateVariable == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("No camera rig was passed."), ELogVerbosity::Error);
		return;
	}

	FCameraNodeEvaluationResult* Result = ResultInterop->GetEvaluationResult();
	if (Result == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("The given camera evaluation result is invalid."), ELogVerbosity::Error);
		return;
	}

	Result->VariableTable.SetValue(PrivateVariable, Value, true);
}

}  // namespace UE::Cameras::Private

UCameraRigParameterInterop::UCameraRigParameterInterop(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UCameraRigParameterInterop::SetBooleanParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, bool bParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			ResultInterop, 
			Cast<UBooleanCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)), 
			bParameterValue);
}

void UCameraRigParameterInterop::SetIntegerParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, int32 ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			ResultInterop, 
			Cast<UInteger32CameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)), 
			ParameterValue);
}

void UCameraRigParameterInterop::SetFloatParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, double ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			ResultInterop, 
			Cast<UFloatCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			(float)ParameterValue);
}

void UCameraRigParameterInterop::SetDoubleParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, double ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			ResultInterop, 
			Cast<UDoubleCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetVector2Parameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector2D ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			ResultInterop, 
			Cast<UVector2dCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetVector3Parameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			ResultInterop, 
			Cast<UVector3dCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetVector4Parameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, FVector4 ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			ResultInterop, 
			Cast<UVector4dCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetRotatorParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, FRotator ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			ResultInterop, 
			Cast<URotator3dCameraVariable>(GetParameterPrivateVariable(CameraRig, ParameterName)),
			ParameterValue);
}

void UCameraRigParameterInterop::SetTransformParameter(UCameraEvaluationResultInterop* ResultInterop, UCameraRigAsset* CameraRig, const FString& ParameterName, FTransform ParameterValue)
{
	UE::Cameras::Private::SetCameraRigParameter(
			ResultInterop, 
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

