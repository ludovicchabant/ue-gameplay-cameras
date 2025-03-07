// Copyright Epic Games, Inc. All Rights Reserved.

#include "Build/CameraNodeHierarchyBuilder.h"

#include "Build/CameraObjectBuildContext.h"
#include "Core/BaseCameraObject.h"
#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraParameters.h"
#include "Core/CameraVariableReferences.h"
#include "Core/ICustomCameraNodeParameterProvider.h"

#define LOCTEXT_NAMESPACE "CameraNodeHierarchyBuilder"

namespace UE::Cameras
{

namespace Internal
{

void AddVariableToAllocationInfo(UCameraVariableAsset* Variable, FCameraVariableTableAllocationInfo& AllocationInfo)
{
	if (Variable)
	{
		FCameraVariableDefinition VariableDefinition = Variable->GetVariableDefinition();
		AllocationInfo.VariableDefinitions.Add(VariableDefinition);
	}
}

void AddVariableToAllocationInfo(FCameraVariableID VariableID, ECameraVariableType VariableType, const UScriptStruct* BlendableStructType, FCameraVariableTableAllocationInfo& AllocationInfo)
{
	if (VariableID)
	{
		FCameraVariableDefinition VariableDefinition;
		VariableDefinition.VariableID = VariableID;
		VariableDefinition.VariableType = VariableType;
		VariableDefinition.BlendableStructType = BlendableStructType;
		VariableDefinition.bIsPrivate = true;
		VariableDefinition.bIsInput = true;
		AllocationInfo.VariableDefinitions.Add(VariableDefinition);
	}
}

}  // namespace Internal

FCameraNodeHierarchyBuilder::FCameraNodeHierarchyBuilder(FCameraBuildLog& InBuildLog, UBaseCameraObject* InCameraObject)
	: BuildLog(InBuildLog)
	, CameraObject(InCameraObject)
{
	CameraNodeHierarchy.Build(CameraObject);
}

void FCameraNodeHierarchyBuilder::PreBuild()
{
	for (UCameraNode* CameraNode : CameraNodeHierarchy.GetFlattenedHierarchy())
	{
		CameraNode->PreBuild(BuildLog);
	}
}

void FCameraNodeHierarchyBuilder::Build()
{
	FCameraObjectBuildContext BuildContext(BuildLog);

	// Build a mock tree of evaluators.
	FCameraNodeEvaluatorTreeBuildParams BuildParams;
	BuildParams.RootCameraNode = CameraObject->GetRootNode();
	FCameraNodeEvaluatorStorage Storage;
	Storage.BuildEvaluatorTree(BuildParams);

	// Get the size of the evaluators' allocation.
	Storage.GetAllocationInfo(BuildContext.AllocationInfo.EvaluatorInfo);

	// Call Build() on all camera nodes in the hierarchy (detached/orphaned camera nodes don't get called).
	for (UCameraNode* CameraNode : CameraNodeHierarchy.GetFlattenedHierarchy())
	{
		CallBuild(BuildContext, CameraNode);
	}

	// Add parameters to the allocation info.
	BuildParametersAllocationInfo(BuildContext);

	// Set the final allocation info on the camera rig asset.
	if (CameraObject->AllocationInfo != BuildContext.AllocationInfo)
	{
		CameraObject->Modify();
		CameraObject->AllocationInfo = BuildContext.AllocationInfo;
	}
}

void FCameraNodeHierarchyBuilder::CallBuild(FCameraObjectBuildContext& BuildContext, UCameraNode* CameraNode)
{
	using namespace UE::Cameras::Internal;

	// Look for properties that are camera parameters, and gather what camera variables they reference. 
	// This is only for user-defined variable overrides. We will do the same for exposed camera rig
	// parameters later, in BuildParametersAllocationInfo.
	UClass* CameraNodeClass = CameraNode->GetClass();
	FCameraObjectAllocationInfo& AllocationInfo = BuildContext.AllocationInfo;
	for (TFieldIterator<FProperty> It(CameraNodeClass); It; ++It)
	{
		FStructProperty* StructProperty = CastField<FStructProperty>(*It);
		if (!StructProperty)
		{
			continue;
		}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		if (StructProperty->Struct == F##ValueName##CameraParameter::StaticStruct())\
		{\
			auto* CameraParameterPtr = StructProperty->ContainerPtrToValuePtr<F##ValueName##CameraParameter>(CameraNode);\
			AddVariableToAllocationInfo(CameraParameterPtr->Variable, AllocationInfo.VariableTableInfo);\
		}\
		else if (StructProperty->Struct == F##ValueName##CameraVariableReference::StaticStruct())\
		{\
			auto* CameraVariableReferencePtr = StructProperty->ContainerPtrToValuePtr<F##ValueName##CameraVariableReference>(CameraNode);\
			AddVariableToAllocationInfo(CameraVariableReferencePtr->Variable, AllocationInfo.VariableTableInfo);\
		}\
		else
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		{
			// Another struct property...
		}
	}

	// Now do the same with custom parameters handled by the node itself.
	if (ICustomCameraNodeParameterProvider* CustomParameterProvider = Cast<ICustomCameraNodeParameterProvider>(CameraNode))
	{
		FCustomCameraNodeParameterInfos CustomParameters;
		CustomParameterProvider->GetCustomCameraNodeParameters(CustomParameters);

		for (const FCustomCameraNodeParameterInfos::FBlendableParameterInfo& BlendableParameter : CustomParameters.BlendableParameters)
		{
			AddVariableToAllocationInfo(BlendableParameter.OverrideVariable, AllocationInfo.VariableTableInfo);
		}
	}

	// Let the camera node add any custom variables or extra memory.
	CameraNode->Build(BuildContext);
}

void FCameraNodeHierarchyBuilder::BuildParametersAllocationInfo(FCameraObjectBuildContext& BuildContext)
{
	// The variables and context data definitions should have already been added by the camera nodes
	// who have override variable IDs and data IDs set on them. 

	for (const UCameraObjectInterfaceBlendableParameter* BlendableParameter : CameraObject->Interface.BlendableParameters)
	{
		if (!BlendableParameter->PrivateVariableID.IsValid())
		{
			continue;
		}

		FCameraVariableDefinition Definition = BlendableParameter->GetVariableDefinition();
		BuildContext.AllocationInfo.VariableTableInfo.VariableDefinitions.Add(Definition);
	}

	for (const UCameraObjectInterfaceDataParameter* DataParameter : CameraObject->Interface.DataParameters)
	{
		if (!DataParameter->PrivateDataID.IsValid())
		{
			continue;
		}

		FCameraContextDataDefinition Definition = DataParameter->GetDataDefinition();
		BuildContext.AllocationInfo.ContextDataTableInfo.DataDefinitions.Add(Definition);
	}
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

