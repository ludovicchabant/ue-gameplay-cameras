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

		if (!ParameterOverride.PrivateVariableGuid.IsValid())
		{
#if WITH_EDITOR
			// Ignore un-built parameter overrides in the editor since the user could have just added
			// an override while PIE is running. They need to hit the Build button for the override
			// to apply.
			continue;
#else
			UE_LOG(LogCameraSystem, Error, 
					TEXT("Invalid parameter override '%s' in camera rig '%s'. Was it built/cooked?"),
					*ParameterOverride.InterfaceParameterName,
					*GetPathNameSafe(CameraRig));
#endif
		}

		FCameraVariableID InterfaceParameterID(FCameraVariableID::FromHashValue(GetTypeHash(ParameterOverride.PrivateVariableGuid)));

		if (ParameterOverride.Value.Variable != nullptr)
		{
			// The override is driven by a variable... read its value and set it as the value for the
			// prefab's variable. Basically, we forward the value from one variable to the next.
			FCameraVariableDefinition OverrideDefinition(ParameterOverride.Value.Variable->GetVariableDefinition());

			const ValueType* OverrideValuePtr = OutVariableTable.FindValue<ValueType>(OverrideDefinition.VariableID);
			if (OverrideValuePtr)
			{
				OutVariableTable.SetValue<ValueType>(InterfaceParameterID, *OverrideValuePtr);
			}
			else
			{
				// Once again, ignore un-built data. Only emit errors when running outside of the editor.
#if !WITH_EDITOR
				UE_LOG(LogCameraSystem, Error, 
						TEXT("Camera variable '%s' for parameter override '%s' in camera rig '%s' isn't in the variable "
							"table. Was it built/cooked?"),
						*GetNameSafe(ParameterOverride.Value.Variable),
						*ParameterOverride.InterfaceParameterName,
						*GetPathNameSafe(CameraRig));
#endif
			}
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

	const UCameraRigAsset* CameraRig = PrefabNode->CameraRigReference.GetCameraRig();
	const FCameraRigParameterOverrides& ParameterOverrides = PrefabNode->CameraRigReference.GetParameterOverrides();

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	Internal::ApplyParameterOverrides(\
			CameraRig,\
			ParameterOverrides.Get##ValueName##Overrides(),\
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

