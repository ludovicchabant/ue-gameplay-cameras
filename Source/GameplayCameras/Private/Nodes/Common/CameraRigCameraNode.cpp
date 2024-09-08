// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/CameraRigCameraNode.h"

#include "Core/CameraBuildLog.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigBuildContext.h"
#include "Core/CameraRigParameterOverrideEvaluator.h"

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
	ApplyParameterOverrides(OutResult.VariableTable, false);
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

bool FCameraRigCameraNodeEvaluator::IsApplyingParameterOverrides() const
{
	return bApplyParameterOverrides;
}

void FCameraRigCameraNodeEvaluator::SetApplyParameterOverrides(bool bShouldApply)
{
	bApplyParameterOverrides = bShouldApply;
}

namespace Internal
{

struct FCameraRigCameraNodeBuilder
{
	UCameraRigCameraNode* CameraNode;
	FCameraRigBuildContext& BuildContext;

	FCameraRigCameraNodeBuilder(UCameraRigCameraNode* InCameraNode, FCameraRigBuildContext& InBuildContext)
		: CameraNode(InCameraNode)
		, BuildContext(InBuildContext)
	{}

	void Setup()
	{
		// Build a map matching each of our inner camera rig's interface parameter to its Guid.
		ParametersByGuid.Reset();
		const UCameraRigAsset* CameraRig = CameraNode->CameraRigReference.GetCameraRig();
		for (TObjectPtr<UCameraRigInterfaceParameter> InterfaceParameter : CameraRig->Interface.InterfaceParameters)
		{
			ParametersByGuid.Add(InterfaceParameter->Guid, InterfaceParameter);
		}
	}

	template<typename ParameterOverrideType>
	void BuildCameraRigParameterOverride(ParameterOverrideType& ParameterOverride);

private:

	const UCameraRigInterfaceParameter* FindInterfaceParameter(const FGuid& InterfaceParameterGuid)
	{
		if (UCameraRigInterfaceParameter** FoundItem = ParametersByGuid.Find(InterfaceParameterGuid))
		{
			return *FoundItem;
		}
		return nullptr;
	}

private:

	TMap<FGuid, UCameraRigInterfaceParameter*> ParametersByGuid;
};

template<typename ParameterOverrideType>
void FCameraRigCameraNodeBuilder::BuildCameraRigParameterOverride(ParameterOverrideType& ParameterOverride)
{
	const UCameraRigAsset* CameraRig = CameraNode->CameraRigReference.GetCameraRig();

	// Each parameter override should point to a valid interface parameter on the inner rig, via its Guid.
	const UCameraRigInterfaceParameter* InterfaceParameter = FindInterfaceParameter(ParameterOverride.InterfaceParameterGuid);
	if (!InterfaceParameter)
	{
		BuildContext.BuildLog.AddMessage(EMessageSeverity::Error, CameraNode,
				FText::Format(
					LOCTEXT("MissingInterfaceParameter", "No camera rig interface parameter named '{0}' exists on '{1}'."),
					FText::FromString(ParameterOverride.InterfaceParameterName),
					FText::FromString(GetNameSafe(CameraRig))));
		return;
	}

	// The inner rig's interface parameter should have been built, i.e. it should have a private camera variable
	// assigned for driving its value.
	if (!InterfaceParameter->PrivateVariable)
	{
		BuildContext.BuildLog.AddMessage(EMessageSeverity::Error, CameraNode,
				FText::Format(
					LOCTEXT("UnbuiltInterfaceParameter", "Camera rig interface parameter '{0}' was not built correctly on '{1}'."),
					FText::FromString(ParameterOverride.InterfaceParameterName),
					FText::FromString(GetNameSafe(CameraRig))));
		return;
	}

	// The inner rig's interface parameter is driven by this private variable. Let's remember its Guid so we
	// can override its value in the variable table at runtime.
	ParameterOverride.PrivateVariableGuid = InterfaceParameter->PrivateVariable->GetGuid();
	// Update the last known name for this interface parameter.
	ParameterOverride.InterfaceParameterName = InterfaceParameter->InterfaceParameterName;

	// The build process automatically gathers variables that drive amera parameters on a camera node, but
	// nothing else for now. We therefore need to help it out by manually reporting the variables that
	// drive our parameter overrides.
	FCameraVariableTableAllocationInfo& VariableTableAllocationInfo = BuildContext.AllocationInfo.VariableTableInfo;
	if (ParameterOverride.Value.Variable)
	{
		VariableTableAllocationInfo.VariableDefinitions.Add(ParameterOverride.Value.Variable->GetVariableDefinition());
	}
}

}  // namespace Internal

}  // namespace UE::Cameras

void UCameraRigCameraNode::OnPreBuild(FCameraBuildLog& BuildLog)
{
	// Build the inner camera rig. Silently skip it if it's not set... but we will
	// report an error in OnBuild about it.
	if (UCameraRigAsset* CameraRig = CameraRigReference.GetCameraRig())
	{
		CameraRig->BuildCameraRig(BuildLog);
	}
}

void UCameraRigCameraNode::OnBuild(FCameraRigBuildContext& BuildContext)
{
	using namespace UE::Cameras;
	using namespace UE::Cameras::Internal;

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

	// Next, we set things up for the runtime. Mostly, we want to get the camera variable 
	// Guids that we need to write the override values to.
	FCameraRigCameraNodeBuilder InternalBuilder(this, BuildContext);
	InternalBuilder.Setup();

	FCameraRigParameterOverrides& ParameterOverrides = CameraRigReference.GetParameterOverrides();
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	{\
		for (F##ValueName##CameraRigParameterOverride& ParameterOverride : ParameterOverrides.Get##ValueName##Overrides())\
		{\
			InternalBuilder.BuildCameraRigParameterOverride(ParameterOverride);\
		}\
	}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
}

FCameraNodeEvaluatorPtr UCameraRigCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FCameraRigCameraNodeEvaluator>();
}

void UCameraRigCameraNode::PostLoad()
{
	Super::PostLoad();

	if (CameraRig_DEPRECATED)
	{
		CameraRigReference.SetCameraRig(CameraRig_DEPRECATED);
		CameraRig_DEPRECATED = nullptr;
	}

	FCameraRigParameterOverrides& ParameterOverrides = CameraRigReference.GetParameterOverrides();
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	if (ValueName##Overrides_DEPRECATED.Num() > 0)\
	{\
		ParameterOverrides.AppendParameterOverrides<F##ValueName##CameraRigParameterOverride>(ValueName##Overrides_DEPRECATED);\
		ValueName##Overrides_DEPRECATED.Reset();\
	}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
}

#undef LOCTEXT_NAMESPACE

