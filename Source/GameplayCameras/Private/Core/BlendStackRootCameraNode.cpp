// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BlendStackRootCameraNode.h"

#include "Algo/Transform.h"
#include "Core/BlendCameraNode.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetReference.h"
#include "Core/CameraRigParameterOverrideEvaluator.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "Nodes/Common/CameraRigCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlendStackRootCameraNode)

FCameraNodeChildrenView UBlendStackRootCameraNode::OnGetChildren()
{
	FCameraNodeChildrenView Children;
	if (Blend)
	{
		Children.Add(Blend);
	}
	if (RootNode)
	{
		Children.Add(RootNode);
	}
	return Children;
}

FCameraNodeEvaluatorPtr UBlendStackRootCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FBlendStackRootCameraNodeEvaluator>();
}

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBlendStackRootCameraNodeEvaluator)

UE_DECLARE_CAMERA_DEBUG_BLOCK_START(GAMEPLAYCAMERAS_API, FBlendStackRootCameraDebugBlock)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FString, CameraRigAssetName)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(TArray<FString>, BlendedParameterOverridesEntries)
UE_DECLARE_CAMERA_DEBUG_BLOCK_END()

UE_DEFINE_CAMERA_DEBUG_BLOCK_WITH_FIELDS(FBlendStackRootCameraDebugBlock)

FBlendStackRootCameraNodeEvaluator::FBlendStackRootCameraNodeEvaluator()
{
	AddNodeEvaluatorFlags(ECameraNodeEvaluatorFlags::NeedsParameterUpdate);
}

FCameraNodeEvaluatorChildrenView FBlendStackRootCameraNodeEvaluator::OnGetChildren()
{
	FCameraNodeEvaluatorChildrenView Children;
	if (BlendEvaluator)
	{
		Children.Add(BlendEvaluator);
	}
	for (FBlendedParameterOverrides& BlendedParameterOverrides : BlendedParameterOverridesStack)
	{
		Children.Add(BlendedParameterOverrides.BlendEvaluator);
	}
	if (RootEvaluator)
	{
		Children.Add(RootEvaluator);
	}
	return Children;
}

void FBlendStackRootCameraNodeEvaluator::OnBuild(const FCameraNodeEvaluatorBuildParams& Params)
{
	const UBlendStackRootCameraNode* RootNode = GetCameraNodeAs<UBlendStackRootCameraNode>();

	BlendEvaluator = Params.BuildEvaluatorAs<FBlendCameraNodeEvaluator>(RootNode->Blend);
	RootEvaluator = Params.BuildEvaluator(RootNode->RootNode);
}

void FBlendStackRootCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UBlendStackRootCameraNode* RootNode = GetCameraNodeAs<UBlendStackRootCameraNode>();
	if (RootNode->RootNode)
	{
		// See if the camera rig we are running is only made up of a prefab with some overrides.
		InitialPrefabNode = Cast<const UCameraRigCameraNode>(RootNode->RootNode);
		if (InitialPrefabNode)
		{
			BlendablePrefabCameraRig = InitialPrefabNode->CameraRigReference.GetCameraRig();
		}

#if UE_GAMEPLAY_CAMERAS_DEBUG
		const UCameraRigAsset* CameraRig = RootNode->RootNode->GetTypedOuter<UCameraRigAsset>();
		CameraRigAssetName = CameraRig ? CameraRig->GetDisplayName() : TEXT("<None>");
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
	}
}

EBlendStackEntryComparison FBlendStackRootCameraNodeEvaluator::Compare(const UCameraRigAsset* CameraRig) const
{
	if (!BlendablePrefabCameraRig)
	{
		return EBlendStackEntryComparison::Different;
	}
	
	const UCameraRigCameraNode* PrefabNode = Cast<const UCameraRigCameraNode>(CameraRig->RootNode);
	if (!PrefabNode)
	{
		return EBlendStackEntryComparison::Different;
	}

	const UCameraRigAsset* Prefab = PrefabNode->CameraRigReference.GetCameraRig();
	if (!Prefab)
	{
		return EBlendStackEntryComparison::Different;
	}

	if (Prefab != BlendablePrefabCameraRig)
	{
		return EBlendStackEntryComparison::Different;
	}

	if (BlendedParameterOverridesStack.IsEmpty())
	{
		return EBlendStackEntryComparison::EligibleForMerge;
	}
	
	const FBlendedParameterOverrides& TopEntry = BlendedParameterOverridesStack.Top();
	if (TopEntry.PrefabNodeAsset == CameraRig)
	{
		return EBlendStackEntryComparison::Active;
	}

	return EBlendStackEntryComparison::EligibleForMerge;
}

void FBlendStackRootCameraNodeEvaluator::MergeCameraRig(const FCameraNodeEvaluatorBuildParams& Params, const UCameraRigCameraNode* PrefabNode, const UBlendCameraNode* Blend)
{
	if (!ensureMsgf(PrefabNode, TEXT("No prefab node given.")))
	{
		return;
	}

	if (!ensureMsgf(BlendablePrefabCameraRig,
				TEXT("Adding blended parameter overrides for a camera rig that doesn't support it.")))
	{
		return;
	}

	if (!ensureMsgf(PrefabNode->CameraRigReference.GetCameraRig() == BlendablePrefabCameraRig,
				TEXT("Adding blended parameter overrides for a different camera rig.")))
	{
		return;
	}

	InitializeBlendedParameterOverridesStack();

	FBlendedParameterOverrides BlendedParameterOverrides;
	BlendedParameterOverrides.PrefabNodeAsset = PrefabNode->GetTypedOuter<UCameraRigAsset>();
	BlendedParameterOverrides.PrefabNode = PrefabNode;
	BlendedParameterOverrides.Blend = Blend;
	BlendedParameterOverrides.Result.VariableTable.Initialize(BlendedParameterOverridesTableAllocationInfo);
	if (Blend)
	{
		BlendedParameterOverrides.BlendEvaluator = Params.BuildEvaluatorAs<FBlendCameraNodeEvaluator>(Blend);
	}

	BlendedParameterOverridesStack.Add(MoveTemp(BlendedParameterOverrides));
}

void FBlendStackRootCameraNodeEvaluator::InitializeBlendedParameterOverridesStack()
{
	if (!ensureMsgf(BlendablePrefabCameraRig, TEXT("The blended parameter overiddes stack has already been initialized.")))
	{
		return;
	}

	if (!BlendedParameterOverridesStack.IsEmpty())
	{
		return;
	}

	// Build the allocation info for the variable tables we keep with each set of parameter overrides.
	for (const UCameraRigInterfaceParameter* InterfaceParameter : BlendablePrefabCameraRig->Interface.InterfaceParameters)
	{
		if (!ensure(InterfaceParameter))
		{
			continue;
		}
		if (!InterfaceParameter->PrivateVariable)
		{
			continue;
		}

		BlendedParameterOverridesTableAllocationInfo.VariableDefinitions.Add(
				InterfaceParameter->PrivateVariable->GetVariableDefinition());;
	}

	FCameraRigCameraNodeEvaluator* RootPrefabNodeEvaluator = RootEvaluator->CastThisChecked<FCameraRigCameraNodeEvaluator>();
	RootPrefabNodeEvaluator->SetApplyParameterOverrides(false);

	FBlendedParameterOverrides InitialParameterOverrides;
	InitialParameterOverrides.PrefabNodeAsset = InitialPrefabNode->GetTypedOuter<UCameraRigAsset>();
	InitialParameterOverrides.PrefabNode = InitialPrefabNode;
	InitialParameterOverrides.Result.VariableTable.Initialize(BlendedParameterOverridesTableAllocationInfo);
	BlendedParameterOverridesStack.Add(MoveTemp(InitialParameterOverrides));
}

void FBlendStackRootCameraNodeEvaluator::OnUpdateParameters(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult)
{
	RunBlendedParameterOverridesStack(Params, OutResult);
}

void FBlendStackRootCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	if (BlendEvaluator)
	{
		BlendEvaluator->Run(Params, OutResult);
	}
	if (RootEvaluator)
	{
		RootEvaluator->Run(Params, OutResult);
	}
}

void FBlendStackRootCameraNodeEvaluator::SetDefaultInterfaceParameterValues(FCameraVariableTable& OutVariableTable)
{
	for (const UCameraRigInterfaceParameter* InterfaceParameter : BlendablePrefabCameraRig->Interface.InterfaceParameters)
	{
		if (!ensure(InterfaceParameter))
		{
			continue;
		}

		const UCameraVariableAsset* PrivateVariable = InterfaceParameter->PrivateVariable;
		if (!PrivateVariable)
		{
			continue;
		}

		OutVariableTable.SetValue(
				PrivateVariable->GetVariableID(), 
				PrivateVariable->GetVariableType(), 
				PrivateVariable->GetDefaultValuePtr());
	}
}

void FBlendStackRootCameraNodeEvaluator::RunBlendedParameterOverridesStack(const FCameraBlendedParameterUpdateParams& Params, FCameraBlendedParameterUpdateResult& OutResult)
{
	if (BlendedParameterOverridesStack.IsEmpty())
	{
		return;
	}

	if (!ensure(BlendablePrefabCameraRig))
	{
		return;
	}

	int32 PopEntriesBelow = INDEX_NONE;
	for (int32 EntryIndex = 0; EntryIndex < BlendedParameterOverridesStack.Num(); ++EntryIndex)
	{
		FBlendedParameterOverrides& BlendedParameterOverrides(BlendedParameterOverridesStack[EntryIndex]);
		FCameraNodeEvaluationResult& CurResult(BlendedParameterOverrides.Result);

		// Start by setting the default values of all parameters. If we don't do this, parameter overrides
		// wouldn't have a base value to blend from.
		SetDefaultInterfaceParameterValues(CurResult.VariableTable);

		// Next, override the defaults with the specific values of this entry.
		FCameraRigParameterOverrideEvaluator OverrideEvaluator(BlendedParameterOverrides.PrefabNode->CameraRigReference);
		OverrideEvaluator.ApplyParameterOverrides(CurResult.VariableTable, false);

		// Finally, update the parameter overrides' blend, and apply it.
		if (BlendedParameterOverrides.BlendEvaluator)
		{
			BlendedParameterOverrides.BlendEvaluator->Run(Params.EvaluationParams, CurResult);

			FCameraNodePreBlendParams BlendParams(Params.EvaluationParams, Params.LastCameraPose, CurResult.VariableTable);
			BlendParams.ExtraVariableTableFilter = ECameraVariableTableFilter::Private;
			FCameraNodePreBlendResult BlendResult(OutResult.VariableTable);
			BlendedParameterOverrides.BlendEvaluator->BlendParameters(BlendParams, BlendResult);

			if (BlendResult.bIsBlendFinished && BlendResult.bIsBlendFull)
			{
				PopEntriesBelow = EntryIndex;
			}
		}
		else
		{
			OutResult.VariableTable.Override(
					CurResult.VariableTable, ECameraVariableTableFilter::Input | ECameraVariableTableFilter::Private);

			PopEntriesBelow = EntryIndex;
		}
	}
	if (PopEntriesBelow >= 0)
	{
		BlendedParameterOverridesStack.RemoveAt(0, PopEntriesBelow);
	}
}

void FBlendStackRootCameraNodeEvaluator::OnAddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(BlendablePrefabCameraRig);
	Collector.AddReferencedObject(InitialPrefabNode);

	for (FBlendedParameterOverrides& BlendedParameterOverrides: BlendedParameterOverridesStack)
	{
		Collector.AddReferencedObject(BlendedParameterOverrides.PrefabNodeAsset);
		Collector.AddReferencedObject(BlendedParameterOverrides.PrefabNode);
		Collector.AddReferencedObject(BlendedParameterOverrides.Blend);
	}
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FBlendStackRootCameraNodeEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	FBlendStackRootCameraDebugBlock& DebugBlock = Builder.StartChildDebugBlock<FBlendStackRootCameraDebugBlock>();
	DebugBlock.CameraRigAssetName = CameraRigAssetName;
	Algo::Transform(BlendedParameterOverridesStack, DebugBlock.BlendedParameterOverridesEntries,
			[](const FBlendedParameterOverrides& Item)
			{
				if (Item.PrefabNode)
				{
					const UCameraRigAsset* OuterCameraRig = Item.PrefabNode->GetTypedOuter<const UCameraRigAsset>();
					if (OuterCameraRig)
					{
						return OuterCameraRig->GetDisplayName();
					}
				}
				return FString(TEXT("<invalid camera rig>"));
			});

	if (BlendEvaluator)
	{
		BlendEvaluator->BuildDebugBlocks(Params, Builder);
	}
	else
	{
		// Dummy block.
		Builder.StartChildDebugBlock<FCameraDebugBlock>();
		Builder.EndChildDebugBlock();
	}

	Builder.StartChildDebugBlock<FCameraDebugBlock>();
	for (const FBlendedParameterOverrides& BlendedParameterOverrides : BlendedParameterOverridesStack)
	{
		if (BlendedParameterOverrides.BlendEvaluator)
		{
			BlendedParameterOverrides.BlendEvaluator->BuildDebugBlocks(Params, Builder);
		}
		else
		{
			// Dummy block.
			Builder.StartChildDebugBlock<FCameraDebugBlock>();
			Builder.EndChildDebugBlock();
		}
	}
	Builder.EndChildDebugBlock();

	if (RootEvaluator)
	{
		RootEvaluator->BuildDebugBlocks(Params, Builder);
	}
	else
	{
		// Dummy block.
		Builder.StartChildDebugBlock<FCameraDebugBlock>();
		Builder.EndChildDebugBlock();
	}

	Builder.EndChildDebugBlock();
	Builder.SkipChildren();
}

void FBlendStackRootCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	TArrayView<FCameraDebugBlock*> ChildrenView(GetChildren());

	Renderer.AddText(TEXT("{cam_passive}<Blend>{cam_default}\n"));
	Renderer.AddIndent();
	ChildrenView[0]->DebugDraw(Params, Renderer);
	Renderer.RemoveIndent();

	if (!BlendedParameterOverridesEntries.IsEmpty())
	{
		Renderer.AddText(TEXT("{cam_passive}<%d Merged Camera Rigs>{cam_default}\n"), BlendedParameterOverridesEntries.Num());
		Renderer.AddIndent();
		{
			int32 ChildIndex = 0;
			for (FCameraDebugBlock* ParameterOverridesDebugBlock : ChildrenView[1]->GetChildren())
			{
				const FString& ParameterOverridesCameraRigName = BlendedParameterOverridesEntries[ChildIndex++];
				Renderer.AddText(ParameterOverridesCameraRigName);
				ParameterOverridesDebugBlock->DebugDraw(Params, Renderer);
			}
		}
		Renderer.RemoveIndent();
	}

	Renderer.AddText(TEXT("{cam_passive}<CameraRig> {cam_default}Running {cam_notice}%s{cam_default}\n"), *CameraRigAssetName);
	Renderer.AddIndent();
	ChildrenView[2]->DebugDraw(Params, Renderer);
	Renderer.RemoveIndent();

	Renderer.SkipAllBlocks();
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

