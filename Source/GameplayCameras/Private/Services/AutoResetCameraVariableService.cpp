// Copyright Epic Games, Inc. All Rights Reserved.

#include "Services/AutoResetCameraVariableService.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraEvaluationContextStack.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableTable.h"
#include "Core/RootCameraNodeCameraRigEvent.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"

namespace UE::Cameras
{

UE_DEFINE_CAMERA_EVALUATION_SERVICE(FAutoResetCameraVariableService)

FAutoResetCameraVariableService::FAutoResetCameraVariableService()
{
	SetEvaluationServiceFlags(ECameraEvaluationServiceFlags::NeedsRootCameraNodeEvents);
}

void FAutoResetCameraVariableService::PerformVariableResets(FCameraVariableTable& RootVariableTable)
{
	DoPerformVariableResets(RootVariableTable, false);
}

void FAutoResetCameraVariableService::PerformVariableResets(FCameraVariableTable& RootVariableTable, const FCameraEvaluationContextStack& ContextStack)
{
	// Reset all variables in the root table.
	DoPerformVariableResets(RootVariableTable, false);

	// For evaluation contexts, only reset what wasn't set this frame by arbitrary code/logic (see the
	// comment later in DoPerformVariableResets).
	TArray<TSharedPtr<FCameraEvaluationContext>> Contexts;
	ContextStack.GetAllContexts(Contexts);
	for (TSharedPtr<FCameraEvaluationContext> Context : Contexts)
	{
		DoPerformVariableResets(Context);
	}

	RemoveMarkedVariablesAndClearFlags();
}

void FAutoResetCameraVariableService::DoPerformVariableResets(FCameraVariableTable& VariableTable, bool bOnlyNotWrittenThisFrame)
{
	for (auto It = AutoResetVariables.CreateIterator(); It; ++It)
	{
		FEntry& Entry = It.Value();
		const UCameraVariableAsset* Variable = It.Key().Get();

		// If the variable was GC'ed, remove it from the list.
		if (!Variable)
		{
			Entry.bMarkedForRemoval = true;
			continue;
		}

		// Don't reset variables that were written this frame. For evaluation contexts' initial result, these
		// variables may have been written by gameplay systems or Blueprint logic or whatever. We only want
		// to reset them if they haven't been touched this frame.
		const bool bValueWasWrittenLastFrame = VariableTable.IsValueWrittenThisFrame(Variable->GetVariableID());
		if (bOnlyNotWrittenThisFrame && bValueWasWrittenLastFrame)
		{
			continue;
		}

		const FCameraVariableID VariableID = Variable->GetVariableID();
		const uint8* DefaultValuePtr = Variable->GetDefaultValuePtr();
		const ECameraVariableType VariableType = Variable->GetVariableType();

		// We use TrySetValue instead of SetValue because we only know of variables *possibly* used by camera rigs. 
		// This doesn't mean these variables have been added to the table and written to. For instance, a camera
		// parameter on a node might be configured to use a variable that isn't set, which makes it fallback to
		// the variable's default value when it's not found in the variable table. So we silently ignore variables
		// not present in the variable table here.
		//
		// Also, note that we don't mark the variable as written this frame. We're resetting it to its default 
		// value so that doesn't qualify.
		VariableTable.TrySetValue(VariableID, VariableType, DefaultValuePtr, false);

		// See if this variable is still used by anybody. If not, we remove it from our list.
		if (Entry.RefCount == 0 && !Entry.bUsedByScripting)
		{
			Entry.bMarkedForRemoval = true;
		}
	}
}

void FAutoResetCameraVariableService::DoPerformVariableResets(TSharedPtr<FCameraEvaluationContext> EvaluationContext)
{
	if (EvaluationContext)
	{
		FCameraNodeEvaluationResult& ContextResult = EvaluationContext->GetInitialResult();
		DoPerformVariableResets(ContextResult.VariableTable, true);

		for (TSharedPtr<FCameraEvaluationContext> ChildContext : EvaluationContext->GetChildrenContexts())
		{
			DoPerformVariableResets(ChildContext);
		}
	}
}

void FAutoResetCameraVariableService::RemoveMarkedVariablesAndClearFlags()
{
	for (auto It = AutoResetVariables.CreateIterator(); It; ++It)
	{
		FEntry& Entry = It.Value();

#if UE_GAMEPLAY_CAMERAS_DEBUG
		Entry.bDebugWasUsedByScripting = Entry.bUsedByScripting;
#endif
		Entry.bUsedByScripting = false;

		if (Entry.bMarkedForRemoval)
		{
			It.RemoveCurrent();
		}
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

void FAutoResetCameraVariableService::RegisterVariableUseFromScripting(const UCameraVariableAsset* InVariable)
{
	if (ensure(InVariable))
	{
		FEntry& Entry = AutoResetVariables.FindOrAdd(InVariable);
		Entry.bUsedByScripting = true;
	}
}

void FAutoResetCameraVariableService::AddAutoResetVariable(const UCameraVariableAsset* InVariable)
{
	if (ensure(InVariable))
	{
		FEntry& Entry = AutoResetVariables.FindOrAdd(InVariable);
		Entry.RefCount += 1;
	}
}

void FAutoResetCameraVariableService::RemoveAutoResetVariable(const UCameraVariableAsset* InVariable)
{
	if (ensure(InVariable))
	{
		FEntry* Entry = AutoResetVariables.Find(InVariable);
		if (ensure(Entry && Entry->RefCount > 0))
		{
			Entry->RefCount -= 1;
		}
	}
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

class FAutoResetCameraVariableDebugBlock : public FCameraDebugBlock
{
	UE_DECLARE_CAMERA_DEBUG_BLOCK(GAMEPLAYCAMERAS_API, FAutoResetCameraVariableDebugBlock)

public:

	FAutoResetCameraVariableDebugBlock() = default;
	FAutoResetCameraVariableDebugBlock(const FAutoResetCameraVariableService& InService);
	
protected:

	// FCameraDebugBlock interface.
	virtual void OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override;
	virtual void OnSerialize(FArchive& Ar) override;

private:

	struct FDebugEntry
	{
		FString VariableName;
		uint32 RefCount = 0;
		bool bUsedByScripting;
	};
	TArray<FDebugEntry> AutoResetVariables;

	friend FArchive& operator<< (FArchive& Ar, FDebugEntry& Entry);
};

UE_DEFINE_CAMERA_DEBUG_BLOCK(FAutoResetCameraVariableDebugBlock)

void FAutoResetCameraVariableService::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	Builder.AttachDebugBlock<FAutoResetCameraVariableDebugBlock>(*this);
}

FAutoResetCameraVariableDebugBlock::FAutoResetCameraVariableDebugBlock(const FAutoResetCameraVariableService& InService)
{
	using FEntry = FAutoResetCameraVariableService::FEntry;
	for (const TPair<TWeakObjectPtr<const UCameraVariableAsset>, FEntry>& Pair : InService.AutoResetVariables)
	{
		const UCameraVariableAsset* Variable = Pair.Key.Get();
		const FEntry& Entry = Pair.Value;
		const FString VariableName = Variable ? 
#if WITH_EDITORONLY_DATA
			Variable->GetDisplayName() : 
#else
			Variable->GetName() :
#endif  // WITH_EDITORONLY_DATA
			TEXT("<None>");
		AutoResetVariables.Add({ VariableName, Entry.RefCount, Entry.bDebugWasUsedByScripting });
	}
}

void FAutoResetCameraVariableDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	Renderer.AddText(TEXT("%d auto-reset variables\n"), AutoResetVariables.Num());
	Renderer.AddIndent();
	{
		for (const FDebugEntry& DebugEntry : AutoResetVariables)
		{
			Renderer.AddText(TEXT("{cam_notice}%s{cam_default} :"), *DebugEntry.VariableName);
			if (DebugEntry.RefCount > 0)
			{
				Renderer.AddText(TEXT(" used {cam_notice2}%d{cam_default} times"), DebugEntry.RefCount);
				if (DebugEntry.bUsedByScripting)
				{
					Renderer.AddText(TEXT(" ;"));
				}
			}
			if (DebugEntry.bUsedByScripting)
			{
				Renderer.AddText(TEXT(" used by {cam_notice2}scripting{cam_default}"));
			}
			if (DebugEntry.RefCount == 0 && !DebugEntry.bUsedByScripting)
			{
				Renderer.AddText(TEXT(" {cam_passive}not used{cam_default}"));
			}
			Renderer.AddText(TEXT("\n"));
		}
	}
	Renderer.RemoveIndent();
}

void FAutoResetCameraVariableDebugBlock::OnSerialize(FArchive& Ar)
{
	Ar << AutoResetVariables;
}

FArchive& operator<< (FArchive& Ar, FAutoResetCameraVariableDebugBlock::FDebugEntry& Entry)
{
	Ar << Entry.VariableName;
	Ar << Entry.RefCount;
	Ar << Entry.bUsedByScripting;

	return Ar;
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

