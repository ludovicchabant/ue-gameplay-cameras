// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/CameraRigParameterInterop.h"

#include "Blueprint/BlueprintExceptionInfo.h"
#include "Core/CameraObjectInterfaceParameterDefinition.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraVariableTable.h"
#include "GameFramework/BlueprintCameraEvaluationDataRef.h"

#define LOCTEXT_NAMESPACE "CameraRigParameterInterop"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigParameterInterop)

UCameraRigParameterInterop::UCameraRigParameterInterop(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UCameraRigParameterInterop::GetCameraParameter(const FBlueprintCameraEvaluationDataRef& CameraData, UCameraRigAsset* CameraRig, FName ParameterName, int32& ReturnValue)
{
	checkNoEntry();
}

void UCameraRigParameterInterop::SetCameraParameter(const FBlueprintCameraEvaluationDataRef& CameraData, UCameraRigAsset* CameraRig, FName ParameterName, const int32& NewValue)
{
	checkNoEntry();
}

DEFINE_FUNCTION(UCameraRigParameterInterop::execGetCameraParameter)
{
	P_GET_STRUCT_REF(FBlueprintCameraEvaluationDataRef, CameraData);
	P_GET_OBJECT(UCameraRigAsset, CameraRig);
	P_GET_STRUCT(FName, ParameterName);

	// Read wildcard value input.
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* TargetProperty = Stack.MostRecentProperty;
	void* TargetPtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (TargetProperty == nullptr || TargetPtr == nullptr)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("InvalidGetCameraParameterReturnValue", "Failed to resolve ReturnValue for GetCameraParameter")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else
	{
		P_NATIVE_BEGIN

		using namespace UE::Cameras;

		const FCameraObjectInterfaceParameterDefinition* ParameterDefinition = CameraRig->GetParameterDefinitions()
			.FindByPredicate(
				[ParameterName](const FCameraObjectInterfaceParameterDefinition& Item)
				{
					return Item.ParameterName == ParameterName;
				});
		if (!ParameterDefinition)
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::NonFatalError,
				FText::Format(LOCTEXT("ParameterDefinitionNotFound", "No such camera parameter: {0}"), FText::FromName(ParameterName))
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
		else if (!CameraData.IsValid())
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::NonFatalError,
				LOCTEXT("InvalidCameraData", "CameraData is an invalid reference")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
		else
		{
			const FInstancedPropertyBag& DefaultParameters = CameraRig->GetDefaultParameters();
			const UPropertyBag* PropertyBag = DefaultParameters.GetPropertyBagStruct();

			if (ParameterDefinition->ParameterType == ECameraObjectInterfaceParameterType::Blendable)
			{
				const FCameraVariableTable& VariableTable = CameraData.GetResult()->VariableTable;
				const uint8* RawValue = VariableTable.TryGetValue(ParameterDefinition->VariableID, ParameterDefinition->VariableType, ParameterDefinition->BlendableStructType);
				if (RawValue)
				{
					TargetProperty->CopyCompleteValue(TargetPtr, RawValue);
				}
			}
			else if (ParameterDefinition->ParameterType == ECameraObjectInterfaceParameterType::Data)
			{
				const FCameraContextDataTable& ContextDataTable = CameraData.GetResult()->ContextDataTable;
				const uint8* RawValue = ContextDataTable.TryGetRawDataPtr(ParameterDefinition->DataID, ParameterDefinition->DataType, ParameterDefinition->DataTypeObject);
				if (RawValue)
				{
					TargetProperty->CopyCompleteValue(TargetPtr, RawValue);
				}
			}
		}

		P_NATIVE_END
	}
}

DEFINE_FUNCTION(UCameraRigParameterInterop::execSetCameraParameter)
{
	P_GET_STRUCT_REF(FBlueprintCameraEvaluationDataRef, CameraData);
	P_GET_OBJECT(UCameraRigAsset, CameraRig);
	P_GET_STRUCT(FName, ParameterName);

	// Read wildcard value input.
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* SourceProperty = Stack.MostRecentProperty;
	const uint8* SourcePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (SourceProperty == nullptr || SourcePtr == nullptr)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("InvalidSetCameraParameterNewValue", "Failed to resolve NewValue for SetCameraParameter")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else
	{
		P_NATIVE_BEGIN

		using namespace UE::Cameras;

		const FCameraObjectInterfaceParameterDefinition* ParameterDefinition = CameraRig->GetParameterDefinitions()
			.FindByPredicate(
				[ParameterName](const FCameraObjectInterfaceParameterDefinition& Item)
				{
					return Item.ParameterName == ParameterName;
				});
		if (!ParameterDefinition)
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::NonFatalError,
				FText::Format(LOCTEXT("ParameterDefinitionNotFound", "No such camera parameter: {0}"), FText::FromName(ParameterName))
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
		else if (!CameraData.IsValid())
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::NonFatalError,
				LOCTEXT("InvalidCameraData", "CameraData is an invalid reference")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
		else
		{
			const FInstancedPropertyBag& DefaultParameters = CameraRig->GetDefaultParameters();
			const UPropertyBag* PropertyBag = DefaultParameters.GetPropertyBagStruct();

			if (ParameterDefinition->ParameterType == ECameraObjectInterfaceParameterType::Blendable)
			{
				FCameraVariableTable& VariableTable = CameraData.GetResult()->VariableTable;
				VariableTable.TrySetValue(ParameterDefinition->VariableID, ParameterDefinition->VariableType, ParameterDefinition->BlendableStructType, SourcePtr);
			}
			else if (ParameterDefinition->ParameterType == ECameraObjectInterfaceParameterType::Data)
			{
				FCameraContextDataTable& ContextDataTable = CameraData.GetResult()->ContextDataTable;
				uint8* RawValue = ContextDataTable.TryGetMutableRawDataPtr(ParameterDefinition->DataID, ParameterDefinition->DataType, ParameterDefinition->DataTypeObject);
				if (RawValue)
				{
					SourceProperty->CopyCompleteValue(RawValue, SourcePtr);
				}
			}
		}

		P_NATIVE_END
	}
}

#undef LOCTEXT_NAMESPACE

