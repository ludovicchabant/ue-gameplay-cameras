// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraNodeEvaluator.h"

#include "Core/CameraNode.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraNodeEvaluatorDebugBlock.h"
#include "UObject/UObjectGlobals.h"

namespace UE::Cameras
{

UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(FCameraNodeEvaluator)

void FCameraNodeEvaluationResult::Reset(bool bResetVariableTable)
{
	CameraPose.Reset();
	CameraRigJoints.Reset();

	if (bResetVariableTable)
	{
		VariableTable.UnsetAllValues();
	}

	bIsCameraCut = false;
	bIsValid = false;
}

void FCameraNodeEvaluationResult::Serialize(FArchive& Ar)
{
	CameraPose.SerializeWithFlags(Ar);
	VariableTable.Serialize(Ar);
	Ar << bIsCameraCut;
	Ar << bIsValid;
}

FCameraNodeEvaluator* FCameraNodeEvaluatorBuildParams::BuildEvaluator(const UCameraNode* InNode) const
{
	if (InNode)
	{
		FCameraNodeEvaluator* NewEvaluator = InNode->BuildEvaluator(Builder);
		NewEvaluator->Build(*this);
		return NewEvaluator;
	}
	return nullptr;
}

FCameraNodeEvaluator::FCameraNodeEvaluator()
{
}

void FCameraNodeEvaluator::SetPrivateCameraNode(TObjectPtr<const UCameraNode> InCameraNode)
{
	PrivateCameraNode = InCameraNode;
}

void FCameraNodeEvaluator::SetNodeEvaluatorFlags(ECameraNodeEvaluatorFlags InFlags)
{
	PrivateFlags = InFlags;
}

FCameraNodeEvaluatorChildrenView FCameraNodeEvaluator::GetChildren()
{
	return OnGetChildren();
}

void FCameraNodeEvaluator::ExecuteOperation(const FCameraOperationParams& Params, FCameraOperation& Operation)
{
	if (!PrivateCameraNode || PrivateCameraNode->bIsEnabled)
	{
		if (EnumHasAnyFlags(PrivateFlags, ECameraNodeEvaluatorFlags::SupportsOperations))
		{
			OnExecuteOperation(Params, Operation);
		}
		else
		{
			for (FCameraNodeEvaluator* Child : GetChildren())
			{
				if (Child)
				{
					Child->ExecuteOperation(Params, Operation);
				}
			}
		}
	}
}

void FCameraNodeEvaluator::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (PrivateCameraNode)
	{
		Collector.AddReferencedObject(PrivateCameraNode);
	}

	OnAddReferencedObjects(Collector);

	for (FCameraNodeEvaluator* Child : GetChildren())
	{
		if (Child)
		{
			Child->AddReferencedObjects(Collector);
		}
	}
}

void FCameraNodeEvaluator::Serialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar)
{
	OnSerialize(Params, Ar);

	for (FCameraNodeEvaluator* Child : GetChildren())
	{
		if (Child)
		{
			Child->Serialize(Params, Ar);
		}
	}
}

void FCameraNodeEvaluator::Build(const FCameraNodeEvaluatorBuildParams& Params)
{
	OnBuild(Params);
}

void FCameraNodeEvaluator::Initialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	OnInitialize(Params, OutResult);

	for (FCameraNodeEvaluator* Child : GetChildren())
	{
		if (Child)
		{
			Child->Initialize(Params, OutResult);
		}
	}
}

void FCameraNodeEvaluator::UpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult)
{
	if (!PrivateCameraNode || PrivateCameraNode->bIsEnabled)
	{
		if (EnumHasAnyFlags(PrivateFlags, ECameraNodeEvaluatorFlags::NeedsParameterUpdate))
		{
			OnUpdateParameters(Params, OutResult);
		}
		else
		{
			for (FCameraNodeEvaluator* Child : GetChildren())
			{
				if (Child)
				{
					Child->UpdateParameters(Params, OutResult);
				}
			}
		}
	}
}

void FCameraNodeEvaluator::Run(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	if (!PrivateCameraNode || PrivateCameraNode->bIsEnabled)
	{
		if (EnumHasAnyFlags(PrivateFlags, ECameraNodeEvaluatorFlags::NeedsEvaluationUpdate))
		{
			OnRun(Params, OutResult);
		}
		else
		{
			for (FCameraNodeEvaluator* Child : GetChildren())
			{
				if (Child)
				{
					Child->Run(Params, OutResult);
				}
			}
		}
	}
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FCameraNodeEvaluator::BuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	// Let's start by adding the default debug block for a node evaluator.
	Builder.StartChildDebugBlock<FCameraNodeEvaluatorDebugBlock>(PrivateCameraNode);
	{
		// Then let the node evaluator attach or add other custom debug blocks.
		const int32 PreviousLevel = Builder.GetHierarchyLevel();
		OnBuildDebugBlocks(Params, Builder);
		if (!ensureMsgf(
				PreviousLevel == Builder.GetHierarchyLevel(), 
				TEXT("Node evaluator added new children debug blocks but forgot to end them!")))
		{
			const int32 LevelsToEnd = Builder.GetHierarchyLevel() - PreviousLevel;
			for (int32 Index = 0; Index < LevelsToEnd; ++Index)
			{
				Builder.EndChildDebugBlock();
			}
		}

		// Build debug blocks for children node evaluators.
		ECameraDebugBlockBuildVisitFlags VisitFlags = Builder.GetVisitFlags();
		Builder.ResetVisitFlags();
		if (!EnumHasAnyFlags(VisitFlags, ECameraDebugBlockBuildVisitFlags::SkipChildren))
		{
			FCameraNodeEvaluatorChildrenView ChildrenView(GetChildren());
			for (FCameraNodeEvaluator* Child : ChildrenView)
			{
				if (Child)
				{
					Child->BuildDebugBlocks(Params, Builder);
				}
			}
		}
	}
	Builder.EndChildDebugBlock();
}

void FCameraNodeEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

