// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/CameraRigCameraNode.h"

#include "Core/CameraBuildLog.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigBuildContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigCameraNode)

#define LOCTEXT_NAMESPACE "CameraRigCameraNode"

namespace UE::Cameras
{

namespace Internal
{

template<typename ParameterOverrideType>
void ApplyParameterOverrides(
		const UCameraRigAsset* CameraRig, 
		TArrayView<const ParameterOverrideType> ParameterOverrides, 
		FCameraVariableTable& OutVariableTable)
{
	for (const ParameterOverrideType& ParameterOverride : ParameterOverrides)
	{
		using ParameterType = decltype(ParameterOverrideType::Value);
		using ValueType = typename ParameterType::ValueType;

		FCameraVariableID InterfaceParameterID(FCameraVariableID::FromHashValue(GetTypeHash(ParameterOverride.PrivateVariableGuid)));

		if (ParameterOverride.Value.Variable != nullptr)
		{
			// The override is driven by a variable... read its value and set it as the value for the
			// prefab's variable. Basically, we forward the value from one variable to the next.
			FCameraVariableDefinition OverrideDefinition(ParameterOverride.Value.Variable->GetVariableDefinition());

			OutVariableTable.SetValue<ValueType>(
					InterfaceParameterID,
					OutVariableTable.GetValue<typename ParameterType::ValueType>(OverrideDefinition.VariableID));
		}
		else
		{
			// The override is a fixed value. Just set that on the prefab's variable.
			OutVariableTable.SetValue<ValueType>(
					InterfaceParameterID,
					ParameterOverride.Value.Value);
		}
	}
}

}  // namespace Internal

class FCameraRigCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FCameraRigCameraNodeEvaluator)

protected:

	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnBuild(const FCameraNodeEvaluatorBuildParams& Params) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	void ApplyParameterOverrides(FCameraVariableTable& OutVariableTable);

private:

	FCameraNodeEvaluator* CameraRigRootEvaluator = nullptr;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FCameraRigCameraNodeEvaluator)

FCameraNodeEvaluatorChildrenView FCameraRigCameraNodeEvaluator::OnGetChildren()
{
	return FCameraNodeEvaluatorChildrenView({ CameraRigRootEvaluator });
}

void FCameraRigCameraNodeEvaluator::OnBuild(const FCameraNodeEvaluatorBuildParams& Params)
{
	const UCameraRigCameraNode* CameraRigNode = GetCameraNodeAs<UCameraRigCameraNode>();
	if (CameraRigNode->CameraRig)
	{
		if (CameraRigNode->CameraRig->RootNode)
		{
			CameraRigRootEvaluator = Params.BuildEvaluator(CameraRigNode->CameraRig->RootNode);
		}
	}
}

void FCameraRigCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	// Apply overrides right away.
	ApplyParameterOverrides(OutResult.VariableTable);
}

void FCameraRigCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	// Keep applying overrides in case they are driven by a variable.
	// TODO: we could skip this step for constant overrides.
	ApplyParameterOverrides(OutResult.VariableTable);

	if (CameraRigRootEvaluator)
	{
		CameraRigRootEvaluator->Run(Params, OutResult);
	}
}

void FCameraRigCameraNodeEvaluator::ApplyParameterOverrides(FCameraVariableTable& OutVariableTable)
{
	const UCameraRigCameraNode* PrefabNode = GetCameraNodeAs<UCameraRigCameraNode>();

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	Internal::ApplyParameterOverrides(\
			PrefabNode->CameraRig,\
			TArrayView<const F##ValueName##CameraRigParameterOverride>(PrefabNode->ValueName##Overrides),\
			OutVariableTable);
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
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

	void Setup(const UCameraRigAsset* InCameraRig)
	{
		ParametersByGuid.Reset();
		for (TObjectPtr<UCameraRigInterfaceParameter> InterfaceParameter : InCameraRig->Interface.InterfaceParameters)
		{
			ParametersByGuid.Add(InterfaceParameter->Guid, InterfaceParameter);
		}
	}

	const UCameraRigInterfaceParameter* FindInterfaceParameter(const FGuid& InterfaceParameterGuid)
	{
		if (UCameraRigInterfaceParameter** FoundItem = ParametersByGuid.Find(InterfaceParameterGuid))
		{
			return *FoundItem;
		}
		return nullptr;
	}

	template<typename ParameterOverrideType>
	void BuildCameraRigParameterOverride(ParameterOverrideType& ParameterOverride);

private:

	TMap<FGuid, UCameraRigInterfaceParameter*> ParametersByGuid;
};

template<typename ParameterOverrideType>
void FCameraRigCameraNodeBuilder::BuildCameraRigParameterOverride(ParameterOverrideType& ParameterOverride)
{
	const UCameraRigInterfaceParameter* InterfaceParameter = FindInterfaceParameter(ParameterOverride.InterfaceParameterGuid);
	if (!InterfaceParameter)
	{
		BuildContext.BuildLog.AddMessage(EMessageSeverity::Error, CameraNode,
				FText::Format(
					LOCTEXT("MissingInterfaceParameter", "No camera rig interface parameter named '{0}' exists on '{1}'."),
					FText::FromString(ParameterOverride.InterfaceParameterName),
					FText::FromString(GetNameSafe(CameraNode->CameraRig))));
		return;
	}

	if (!InterfaceParameter->PrivateVariable)
	{
		BuildContext.BuildLog.AddMessage(EMessageSeverity::Error, CameraNode,
				FText::Format(
					LOCTEXT("UnbuiltInterfaceParameter", "Camera rig interface parameter '{0}' was not built correctly on '{1}'."),
					FText::FromString(ParameterOverride.InterfaceParameterName),
					FText::FromString(GetNameSafe(CameraNode->CameraRig))));
		return;
	}

	ParameterOverride.PrivateVariableGuid = InterfaceParameter->PrivateVariable->GetGuid();
}

}  // namespace Internal

}  // namespace UE::Cameras

void UCameraRigCameraNode::OnBuild(FCameraRigBuildContext& BuildContext)
{
	using namespace UE::Cameras;
	using namespace UE::Cameras::Internal;

	if (!CameraRig)
	{
		BuildContext.BuildLog.AddMessage(EMessageSeverity::Error, this, LOCTEXT("MissingCameraRig", "No camera rig specified on camera rig node."));
		return;
	}

	// Build the inner camera rig. Whatever allocations it needs for its evaluators and
	// their camera variables, we add that to our camera rig's allocation info.
	CameraRig->BuildCameraRig(BuildContext.BuildLog);
	BuildContext.AllocationInfo.Append(CameraRig->AllocationInfo);

	// Next, we set things up for the runtime. Mostly, we want to get the camera variable 
	// Guids that we need to write the override values to.
	FCameraRigCameraNodeBuilder InternalBuilder(this, BuildContext);
	InternalBuilder.Setup(CameraRig);

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	{\
		for (F##ValueName##CameraRigParameterOverride& ParameterOverride : ValueName##Overrides)\
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

#undef LOCTEXT_NAMESPACE

