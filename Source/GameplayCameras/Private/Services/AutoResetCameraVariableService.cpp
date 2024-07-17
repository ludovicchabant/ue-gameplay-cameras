// Copyright Epic Games, Inc. All Rights Reserved.

#include "Services/AutoResetCameraVariableService.h"

#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableTable.h"
#include "Core/RootCameraNodeCameraRigEvent.h"

namespace UE::Cameras
{

void FAutoResetCameraVariableService::OnInitialize(const FCameraEvaluationServiceInitializeParams& Params)
{
	SetEvaluationServiceFlags(
			ECameraEvaluationServiceFlags::NeedsPreUpdate |
			ECameraEvaluationServiceFlags::NeedsRootCameraNodeEvents);
}

void FAutoResetCameraVariableService::OnPreUpdate(const FCameraEvaluationServiceUpdateParams& Params, FCameraEvaluationServiceUpdateResult& OutResult)
{
	FCameraVariableTable& VariableTable = OutResult.EvaluationResult.VariableTable;

	for (const TPair<const UCameraVariableAsset*, uint32>& Pair : AutoResetVariables)
	{
		const FCameraVariableID VariableID = Pair.Key->GetVariableID();
		const uint8* DefaultValuePtr = Pair.Key->GetDefaultValuePtr();
		const ECameraVariableType VariableType = Pair.Key->GetVariableType();
		// Using TrySetValue instead of SetValue because we only know of variables *possibly* used by camera rigs. 
		// This doesn't mean these variables have been added to the table and written to. For instance, a camera
		// parameter on a node might be configured to use a variable that isn't set, which makes it fallback to
		// the variable's default value when it's not found in the variable table. So we silently ignore variables
		// not present in the variable table here.
		VariableTable.TrySetValue(VariableID, VariableType, DefaultValuePtr);
	}
}

void FAutoResetCameraVariableService::OnRootCameraNodeEvent(const FRootCameraNodeCameraRigEvent& InEvent)
{
	const UCameraRigAsset* CameraRig = InEvent.CameraRigInfo.CameraRig;
	if (!CameraRig)
	{
		return;
	}

	const FCameraVariableTableAllocationInfo& VariableTableInfo = CameraRig->AllocationInfo.VariableTableInfo;

	switch (InEvent.EventType)
	{
		case ERootCameraNodeCameraRigEventType::Activated:
			{
				for (const UCameraVariableAsset* Variable : VariableTableInfo.AutoResetVariables)
				{
					AddAutoResetVariable(Variable);
				}
			}
			break;
		case ERootCameraNodeCameraRigEventType::Deactivated:
			{
				for (const UCameraVariableAsset* Variable : VariableTableInfo.AutoResetVariables)
				{
					RemoveAutoResetVariable(Variable);
				}
			}
			break;
	}
}

void FAutoResetCameraVariableService::AddAutoResetVariable(const UCameraVariableAsset* InVariable)
{
	uint32& RefCount = AutoResetVariables.FindOrAdd(InVariable, 0);
	++RefCount;
}

void FAutoResetCameraVariableService::RemoveAutoResetVariable(const UCameraVariableAsset* InVariable)
{
	uint32* RefCount = AutoResetVariables.Find(InVariable);
	if (ensure(RefCount))
	{
		--(*RefCount);
		if (*RefCount == 0)
		{
			AutoResetVariables.Remove(InVariable);
		}
	}
}

}  // namespace UE::Cameras

