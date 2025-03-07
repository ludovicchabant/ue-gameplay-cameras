// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigCombinationRegistry.h"

#include "Core/CameraRigAsset.h"
#include "Helpers/CameraObjectReferenceParameterOverrideEvaluator.h"

namespace UE::Cameras
{

FCameraRigCombinationRegistry::FCameraRigCombinationKey::FCameraRigCombinationKey(TArrayView<const UCameraRigAsset*> InCombination)
{
	CachedHash = 0;
	for (const UCameraRigAsset* CameraRig : InCombination)
	{
		ensure(CameraRig);
		TWeakObjectPtr<const UCameraRigAsset> WeakCameraRig(CameraRig);
		CachedHash = HashCombine(CachedHash, GetTypeHash(WeakCameraRig));
		Combination.Add(WeakCameraRig);
	}
}

const UCameraRigAsset* FCameraRigCombinationRegistry::FindOrCreateCombination(TArrayView<const UCameraRigAsset*> InCombination)
{
	FCameraRigCombinationKey Key(InCombination);
	int32& Index = Combinations.FindOrAdd(Key, INDEX_NONE);
	if (Index == INDEX_NONE)
	{
		Index = CombinedCameraRigs.Find(nullptr);
		if (Index == INDEX_NONE)
		{
			Index = CombinedCameraRigs.Add(nullptr);
		}
		check(CombinedCameraRigs.IsValidIndex(Index));

		UCameraRigAsset* NewCombinedCameraRig = NewObject<UCameraRigAsset>();
		UCombinedCameraRigsCameraNode* PrefabNode = NewObject<UCombinedCameraRigsCameraNode>(NewCombinedCameraRig);
		{
			for (const UCameraRigAsset* IndividualCameraRig : InCombination)
			{
				PrefabNode->CameraRigReferences.Add(const_cast<UCameraRigAsset*>(IndividualCameraRig));

				NewCombinedCameraRig->AllocationInfo.Append(IndividualCameraRig->AllocationInfo);
			}
		}
		NewCombinedCameraRig->RootNode = PrefabNode;

		CombinedCameraRigs[Index] = NewCombinedCameraRig;
	}
	return CombinedCameraRigs[Index];
}

void FCameraRigCombinationRegistry::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(CombinedCameraRigs);
}

class FCombinedCameraRigsCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FCombinedCameraRigsCameraNodeEvaluator)

protected:

	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnBuild(const FCameraNodeEvaluatorBuildParams& Params) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	void ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, bool bDrivenOnly);

private:

	TArray<FCameraNodeEvaluator*> CameraRigRootEvaluators;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FCombinedCameraRigsCameraNodeEvaluator)

FCameraNodeEvaluatorChildrenView FCombinedCameraRigsCameraNodeEvaluator::OnGetChildren()
{
	return FCameraNodeEvaluatorChildrenView(CameraRigRootEvaluators);
}

void FCombinedCameraRigsCameraNodeEvaluator::OnBuild(const FCameraNodeEvaluatorBuildParams& Params)
{
	const UCombinedCameraRigsCameraNode* CombinedRigsNode = GetCameraNodeAs<UCombinedCameraRigsCameraNode>();
	for (const FCameraRigAssetReference& IndividualCameraRigReference : CombinedRigsNode->CameraRigReferences)
	{
		if (const UCameraRigAsset* CameraRig = IndividualCameraRigReference.GetCameraRig())
		{
			if (CameraRig->RootNode)
			{
				FCameraNodeEvaluator* CameraRigRootEvaluator = Params.BuildEvaluator(CameraRig->RootNode);
				if (CameraRigRootEvaluator)
				{
					CameraRigRootEvaluators.Add(CameraRigRootEvaluator);
				}
			}
		}
	}
}

void FCombinedCameraRigsCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	// Apply overrides right away.
	ApplyParameterOverrides(OutResult.VariableTable, false);
}

void FCombinedCameraRigsCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	// Keep applying overrides in case they are driven by a variable.
	ApplyParameterOverrides(OutResult.VariableTable, true);

	for (FCameraNodeEvaluator* CameraRigRootEvaluator : CameraRigRootEvaluators)
	{
		CameraRigRootEvaluator->Run(Params, OutResult);
	}
}

void FCombinedCameraRigsCameraNodeEvaluator::ApplyParameterOverrides(FCameraVariableTable& OutVariableTable, bool bDrivenOnly)
{
	const UCombinedCameraRigsCameraNode* CombinedRigsNode = GetCameraNodeAs<UCombinedCameraRigsCameraNode>();
	for (const FCameraRigAssetReference& IndividualCameraRigReference : CombinedRigsNode->CameraRigReferences)
	{
		FCameraObjectReferenceParameterOverrideEvaluator OverrideEvaluator(IndividualCameraRigReference);
		OverrideEvaluator.ApplyParameterOverrides(OutVariableTable, bDrivenOnly);
	}
}

}  // namespace UE::Cameras

void UCombinedCameraRigsCameraNode::GetAllCombinationCameraRigs(const UCameraRigAsset* InCameraRig, TArray<const UCameraRigAsset*>& OutCameraRigs)
{
	if (!InCameraRig)
	{
		return;
	}

	if (const UCombinedCameraRigsCameraNode* CameraRigCombinationNode = Cast<UCombinedCameraRigsCameraNode>(InCameraRig->RootNode))
	{
		for (const FCameraRigAssetReference& CameraRigReference : CameraRigCombinationNode->CameraRigReferences)
		{
			if (const UCameraRigAsset* CameraRig = CameraRigReference.GetCameraRig())
			{
				OutCameraRigs.Add(CameraRig);
			}
		}
	}
	else
	{
		OutCameraRigs.Add(InCameraRig);
	}
}

FCameraNodeEvaluatorPtr UCombinedCameraRigsCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FCombinedCameraRigsCameraNodeEvaluator>();
}

