// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/CameraRigCameraNode.h"

#include "Core/CameraBuildLog.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigBuildContext.h"
#include "Helpers/CameraRigParameterOverrideEvaluator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigCameraNode)

#define LOCTEXT_NAMESPACE "CameraRigCameraNode"

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FCameraRigCameraNodeEvaluator)

FCameraRigCameraNodeEvaluator::FCameraRigCameraNodeEvaluator()
{
	AddNodeEvaluatorFlags(ECameraNodeEvaluatorFlags::NeedsParameterUpdate);
}

FCameraNodeEvaluatorChildrenView FCameraRigCameraNodeEvaluator::OnGetChildren()
{
	return FCameraNodeEvaluatorChildrenView({ CameraRigRootEvaluator });
}

void FCameraRigCameraNodeEvaluator::OnBuild(const FCameraNodeEvaluatorBuildParams& Params)
{
	const UCameraRigCameraNode* CameraRigNode = GetCameraNodeAs<UCameraRigCameraNode>();
	if (const UCameraRigAsset* CameraRig = CameraRigNode->CameraRigReference.GetCameraRig())
	{
		if (CameraRig->RootNode)
		{
			CameraRigRootEvaluator = Params.BuildEvaluator(CameraRig->RootNode);
		}
	}
}

void FCameraRigCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	// Apply overrides right away.
	ApplyParameterOverrides(OutResult.VariableTable, OutResult.ContextDataTable, false);
}

void FCameraRigCameraNodeEvaluator::OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult)
{
	// Keep applying overrides in case they are driven by a variable.
	const bool bDrivenOverridesOnly = true; //(Params.EvaluationParams.EvaluationType == ECameraNodeEvaluationType::Standard);
	ApplyParameterOverrides(OutResult.VariableTable, bDrivenOverridesOnly);
}

void FCameraRigCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	if (CameraRigRootEvaluator)
	{
		CameraRigRootEvaluator->Run(Params, OutResult);
	}
}

void FCameraRigCameraNodeEvaluator::ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, bool bDrivenOnly)
{
	if (bApplyParameterOverrides)
	{
		const UCameraRigCameraNode* PrefabNode = GetCameraNodeAs<UCameraRigCameraNode>();

		FCameraRigParameterOverrideEvaluator OverrideEvaluator(PrefabNode->CameraRigReference);
		OverrideEvaluator.ApplyParameterOverrides(OutVariableTable, bDrivenOnly);
	}
}

void FCameraRigCameraNodeEvaluator::ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, FCameraContextDataTable& OutContextDataTable, bool bDrivenOnly)
{
	if (bApplyParameterOverrides)
	{
		const UCameraRigCameraNode* PrefabNode = GetCameraNodeAs<UCameraRigCameraNode>();

		FCameraRigParameterOverrideEvaluator OverrideEvaluator(PrefabNode->CameraRigReference);
		OverrideEvaluator.ApplyParameterOverrides(OutVariableTable, OutContextDataTable, bDrivenOnly);
	}
}

bool FCameraRigCameraNodeEvaluator::IsApplyingParameterOverrides() const
{
	return bApplyParameterOverrides;
}

void FCameraRigCameraNodeEvaluator::SetApplyParameterOverrides(bool bShouldApply)
{
	bApplyParameterOverrides = bShouldApply;
}

}  // namespace UE::Cameras

void UCameraRigCameraNode::OnPreBuild(FCameraBuildLog& BuildLog)
{
	// Build the inner camera rig. Silently skip it if it's not set... but we will
	// report an error in OnBuild about it.
	if (UCameraRigAsset* CameraRig = CameraRigReference.GetCameraRig())
	{
		CameraRig->BuildCameraRig(BuildLog);
	}

	// Make sure the property bag of the camera rig reference is up to date.
	CameraRigReference.RebuildParametersIfNeeded();
}

void UCameraRigCameraNode::OnBuild(FCameraRigBuildContext& BuildContext)
{
	using namespace UE::Cameras;

	UCameraRigAsset* CameraRig = CameraRigReference.GetCameraRig();
	if (!CameraRig)
	{
		BuildContext.BuildLog.AddMessage(EMessageSeverity::Error, this, 
				LOCTEXT("MissingCameraRig", "No camera rig specified on camera rig node."));
		return;
	}

	// Whatever allocations our inner camera rig needs for its evaluators and
	// their camera variables, we add that to our camera rig's allocation info.
	BuildContext.AllocationInfo.Append(CameraRig->AllocationInfo);
}

void UCameraRigCameraNode::GetCustomCameraNodeParameters(FCustomCameraNodeParameterInfos& OutParameterInfos)
{
	const FInstancedPropertyBag& ParameterOverrides = CameraRigReference.GetParameters();
	const UPropertyBag* ParameterOverridesStruct = ParameterOverrides.GetPropertyBagStruct();
	if (!ParameterOverridesStruct)
	{
		return;
	}

	for (const FPropertyBagPropertyDesc& PropertyDesc : ParameterOverridesStruct->GetPropertyDescs())
	{
		switch (PropertyDesc.ValueType)
		{
			case EPropertyBagPropertyType::Struct:
				{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
					if (PropertyDesc.ValueTypeObject == F##ValueName##CameraParameter::StaticStruct())\
					{\
						using CameraParameterType = F##ValueName##CameraParameter;\
						TValueOrError<CameraParameterType*, EPropertyBagResult> PropertyValue =\
						ParameterOverrides.GetValueStruct<CameraParameterType>(PropertyDesc);\
						if (ensure(PropertyValue.HasValue() && !PropertyValue.HasError()))\
						{\
							CameraParameterType* CameraParameter = PropertyValue.GetValue();\
							check(CameraParameter);\
							const uint8* DefaultValuePtr = reinterpret_cast<const uint8*>(&CameraParameter->Value);\
							OutParameterInfos.AddBlendableParameter(\
									PropertyDesc.Name, ECameraVariableType::ValueName, DefaultValuePtr, &CameraParameter->Variable);\
						}\
					}\
					else
					UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
					{
						const UScriptStruct* DataType = CastChecked<const UScriptStruct>(PropertyDesc.ValueTypeObject);
						OutParameterInfos.AddDataParameter(PropertyDesc.Name, ECameraContextDataType::Struct, DataType, nullptr);
					}
				}
				break;
			case EPropertyBagPropertyType::Name:
				OutParameterInfos.AddDataParameter(PropertyDesc.Name, ECameraContextDataType::Name, nullptr, nullptr);
				break;
			case EPropertyBagPropertyType::String:
				OutParameterInfos.AddDataParameter(PropertyDesc.Name, ECameraContextDataType::String, nullptr, nullptr);
				break;
			case EPropertyBagPropertyType::Enum:
				{
					const UEnum* EnumType = CastChecked<const UEnum>(PropertyDesc.ValueTypeObject);
					OutParameterInfos.AddDataParameter(PropertyDesc.Name, ECameraContextDataType::Enum, EnumType, nullptr);
				}
				break;
			case EPropertyBagPropertyType::Object:
				OutParameterInfos.AddDataParameter(PropertyDesc.Name, ECameraContextDataType::Object, nullptr, nullptr);
				break;
			case EPropertyBagPropertyType::Class:
				OutParameterInfos.AddDataParameter(PropertyDesc.Name, ECameraContextDataType::Object, nullptr, nullptr);
				break;
		}
	}
}

FCameraNodeEvaluatorPtr UCameraRigCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FCameraRigCameraNodeEvaluator>();
}

#undef LOCTEXT_NAMESPACE

