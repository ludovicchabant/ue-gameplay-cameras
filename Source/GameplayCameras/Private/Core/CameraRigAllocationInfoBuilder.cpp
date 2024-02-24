// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAllocationInfoBuilder.h"

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraParameters.h"
#include "Core/CameraVariableAssets.h"
#include "Templates/UnrealTemplate.h"

namespace UE::Cameras
{

bool FCameraRigAllocationInfoBuilder::bParameterHandlersInitialized;
FCameraRigAllocationInfoBuilder::FParameterHandlerMap FCameraRigAllocationInfoBuilder::ParameterHandlers;

void FCameraRigAllocationInfoBuilder::BuildAllocationInfo(const UCameraRigAsset* CameraRigAsset, FCameraRigAllocationInfo& AllocationInfo)
{
	if (!ensureMsgf(CameraRigAsset, TEXT("The provided camera rig is null!")))
	{
		return;
	}
	if (!ensureMsgf(CameraRigAsset->RootNode, TEXT("The provided camera rig has no root node!")))
	{
		return;
	}

	EnsureParameterHandlersInitialized();
	TGuardValue<FCameraRigAllocationInfo*> CurrentAllocationInfoGuard(CurrentAllocationInfo, &AllocationInfo);

	// Build a mock tree of evaluators.
	FCameraNodeEvaluatorTreeBuildParams BuildParams;
	BuildParams.RootCameraNode = CameraRigAsset->RootNode;
	BuildParams.bInitialize = false;
	FCameraNodeEvaluatorStorage Storage;
	Storage.BuildEvaluatorTree(BuildParams);

	// Get the size of the allocation.
	Storage.GetAllocationInfo(AllocationInfo.EvaluatorInfo);

	// Compute the rest, especially the variable table allocation info.
	TArray<UCameraNode*> NodeStack;
	NodeStack.Add(CameraRigAsset->RootNode);
	while (!NodeStack.IsEmpty())
	{
		UCameraNode* CurrentNode = NodeStack.Pop();
		BuildAllocationInfo(CurrentNode);

		for (UCameraNode* Child : ReverseIterate(CurrentNode->GetChildren()))
		{
			if (Child)
			{
				NodeStack.Add(Child);
			}
		}
	}
}

void FCameraRigAllocationInfoBuilder::EnsureParameterHandlersInitialized()
{
	if (!bParameterHandlersInitialized)
	{
		bParameterHandlersInitialized = true;

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		ParameterHandlers.Add(\
				F##ValueName##CameraParameter::StaticStruct(),\
				[](UObject* Node, FStructProperty* Property, FCameraRigAllocationInfo& AllocationInfo)\
				{\
					check(Property->Struct == F##ValueName##CameraParameter::StaticStruct());\
					auto* ParameterPtr = Property->ContainerPtrToValuePtr<F##ValueName##CameraParameter>(Node);\
					FCameraRigAllocationInfoBuilder::GatherVariable(ParameterPtr->Variable, AllocationInfo.VariableTableInfo);\
				});
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
	}
}

void FCameraRigAllocationInfoBuilder::BuildAllocationInfo(const UCameraNode* CameraNode)
{
	checkSlow(CurrentAllocationInfo);

	// Look for properties that are camera parameters, and gather what camera variables
	// they reference.
	UClass* CameraNodeClass = CameraNode->GetClass();
	for (TFieldIterator<FProperty> It(CameraNodeClass); It; ++It)
	{
		FStructProperty* StructProperty = CastField<FStructProperty>(*It);
		if (!StructProperty)
		{
			continue;
		}

		if (FParameterHandler* ParameterHandler = ParameterHandlers.Find(StructProperty->Struct))
		{
			UObject* CameraNodeObj = const_cast<UObject*>(static_cast<const UObject*>(CameraNode));
			(*ParameterHandler)(CameraNodeObj, StructProperty, *CurrentAllocationInfo);
		}
	}

	// Let the camera node add any custom variables or extra memory.
	CameraNode->BuildAllocationInfo(*CurrentAllocationInfo);
}

void FCameraRigAllocationInfoBuilder::GatherVariable(const UCameraVariableAsset* InVariable, FCameraVariableTableAllocationInfo& AllocationInfo)
{
	if (InVariable)
	{
		FCameraVariableDefinition VariableDefinition = InVariable->GetVariableDefinition();
		AllocationInfo.VariableDefinitions.Add(VariableDefinition);
	}
}

}  // namespace UE::Cameras

