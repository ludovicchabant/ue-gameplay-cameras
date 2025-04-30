// Copyright Epic Games, Inc. All Rights Reserved.

#include "Services/CameraShakeService.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraNodeEvaluatorHierarchy.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraShakeAsset.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNode.h"
#include "Core/ShakeCameraNode.h"
#include "UObject/ObjectMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraShakeService)

namespace UE::Cameras
{

class FCameraShakeServiceCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FCameraShakeServiceCameraNodeEvaluator)

public:

	void StartCameraShake(const FStartCameraShakeParams& Params);
	void RequestCameraShakeThisFrame(const FStartCameraShakeParams& Params);

protected:

	// FCameraNodeEvaluator interface.
	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	struct FShakeEntry;

	FShakeEntry* AddCameraShake(const FStartCameraShakeParams& Params);

	void InitializeEntry(
		FShakeEntry& NewEntry, 
		const UCameraShakeAsset* CameraShake,
		TSharedPtr<const FCameraEvaluationContext> EvaluationContext);

	void PopEntry(int32 EntryIndex);

private:

	struct FShakeEntry
	{
		TWeakPtr<const FCameraEvaluationContext> EvaluationContext;
		TObjectPtr<const UCameraShakeAsset> CameraShake;
		FCameraNodeEvaluatorStorage EvaluatorStorage;
		FBlendCameraNodeEvaluator* BlendInEvaluator = nullptr;
		FBlendCameraNodeEvaluator* BlendOutEvaluator = nullptr;
		FShakeCameraNodeEvaluator* RootEvaluator = nullptr;
		FCameraNodeEvaluatorHierarchy EvaluatorHierarchy;
		FCameraNodeEvaluationResult Result;
		float CurrentTime = 0.f;
		float ShakeScale = 1.f;
		ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal;
		FMatrix UserPlaySpaceMatrix;
		uint8 NumRequests = 0;
		bool bPersistentRequest = false;
		bool bIsFirstFrame = false;
	};

	FCameraSystemEvaluator* OwningEvaluator = nullptr;
	TSharedPtr<const FCameraEvaluationContext> ShakeContext;
	const FCameraVariableTable* BlendedParameters = nullptr;;

	TArray<FShakeEntry> Entries;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FCameraShakeServiceCameraNodeEvaluator)

FCameraNodeEvaluatorChildrenView FCameraShakeServiceCameraNodeEvaluator::OnGetChildren()
{
	FCameraNodeEvaluatorChildrenView ChildrenView;
	for (FShakeEntry& Entry : Entries)
	{
		ChildrenView.Add(Entry.RootEvaluator);
	}
	return ChildrenView;
}

void FCameraShakeServiceCameraNodeEvaluator::StartCameraShake(const FStartCameraShakeParams& Params)
{
	if (!Params.CameraShake)
	{
		return;
	}

	// If this shake wants to only have a single instance active at a time, look for a running
	// one and restart it.
	if (Params.CameraShake->bIsSingleInstance)
	{
		FShakeEntry* ExistingEntry = nullptr;
		for (FShakeEntry& Entry : Entries)
		{
			if (Entry.CameraShake == Params.CameraShake)
			{
				ExistingEntry = &Entry;
				break;
			}
		}
		if (ExistingEntry && ensure(ExistingEntry->RootEvaluator))
		{
			FCameraNodeShakeRestartParams RestartParams;
			ExistingEntry->RootEvaluator->RestartShake(RestartParams);
			return;
		}
	}

	FShakeEntry* NewEntry = AddCameraShake(Params);
	if (NewEntry)
	{
		NewEntry->bPersistentRequest = true;
	}
}

void FCameraShakeServiceCameraNodeEvaluator::RequestCameraShakeThisFrame(const FStartCameraShakeParams& Params)
{
	if (!Params.CameraShake)
	{
		return;
	}

	// Record this request on a running camera shake, if any.
	bool bFoundShake = false;
	for (FShakeEntry& Entry : Entries)
	{
		if (Entry.CameraShake == Params.CameraShake && !Entry.bPersistentRequest)
		{
			++Entry.NumRequests;
			bFoundShake = true;
			break;
		}
	}

	if (bFoundShake)
	{
		return;
	}

	// Create a new camera shake if there wasn't any, and record this first request.
	FShakeEntry* NewEntry = AddCameraShake(Params);
	if (NewEntry)
	{
		NewEntry->bPersistentRequest = false;
		NewEntry->NumRequests = 1;
	}
}

FCameraShakeServiceCameraNodeEvaluator::FShakeEntry* FCameraShakeServiceCameraNodeEvaluator::AddCameraShake(const FStartCameraShakeParams& Params)
{
	ensure(Params.CameraShake);

	FShakeEntry NewEntry;
	InitializeEntry(NewEntry, Params.CameraShake, ShakeContext);

	NewEntry.ShakeScale = Params.ShakeScale;
	NewEntry.PlaySpace = Params.PlaySpace;
	NewEntry.UserPlaySpaceMatrix = Params.UserPlaySpaceMatrix;

	const int32 AddedIndex = Entries.Add(MoveTemp(NewEntry));
	return &Entries[AddedIndex];
}

void FCameraShakeServiceCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	OwningEvaluator = Params.Evaluator;
	ShakeContext = Params.EvaluationContext;
	BlendedParameters = OwningEvaluator->GetRootNodeEvaluator()->GetBlendedParameters();
}

void FCameraShakeServiceCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	TArray<int32> EntriesToRemove;

	for (int32 Index = 0; Index < Entries.Num(); ++Index)
	{
		FShakeEntry& Entry(Entries[Index]);

		// See if anybody still cares about this shake.
		if (!Entry.bPersistentRequest && Entry.NumRequests == 0)
		{
			EntriesToRemove.Add(Index);
			continue;
		}

		// Set us up for updating this shake.
		TSharedPtr<const FCameraEvaluationContext> CurContext = Entry.EvaluationContext.Pin();

		FCameraNodeEvaluationParams CurParams(Params);
		CurParams.EvaluationContext = CurContext;
		CurParams.bIsFirstFrame = Entry.bIsFirstFrame;

		FCameraNodeEvaluationResult& CurResult(Entry.Result);

		// Start with the input given to us.
		{
			CurResult.CameraPose = OutResult.CameraPose;
			CurResult.VariableTable.OverrideAll(OutResult.VariableTable);
			CurResult.ContextDataTable.OverrideAll(OutResult.ContextDataTable);
			CurResult.CameraRigJoints.OverrideAll(OutResult.CameraRigJoints);
			CurResult.PostProcessSettings.OverrideAll(OutResult.PostProcessSettings);

			const FCameraNodeEvaluationResult& ContextResult(CurContext->GetInitialResult());
			CurResult.bIsCameraCut = OutResult.bIsCameraCut || ContextResult.bIsCameraCut;
			CurResult.bIsValid = true;
		}

		// Add any parameters coming from the main blend stack.
		if (BlendedParameters)
		{
			CurResult.VariableTable.Override(*BlendedParameters, ECameraVariableTableFilter::KnownOnly);
		}

		// Update timing.
		Entry.CurrentTime += Params.DeltaTime;
		
		// Run the shake!
		float CurTimeLeft = 0.f;
		if (Entry.RootEvaluator)
		{
			Entry.RootEvaluator->Run(CurParams, CurResult);

			FCameraNodeShakeParams ShakeParams(CurParams);
			ShakeParams.ShakeScale = Entry.ShakeScale;
			ShakeParams.PlaySpace = Entry.PlaySpace;
			ShakeParams.UserPlaySpaceMatrix = Entry.UserPlaySpaceMatrix;

			FCameraNodeShakeResult ShakeResult(CurResult);

			Entry.RootEvaluator->ShakeResult(ShakeParams, ShakeResult);

			CurTimeLeft = ShakeResult.ShakeTimeLeft;
		}

		// We are done with this shake this frame, so clear requests.
		Entry.NumRequests = 0;

		// If it says it's finished, schedule it for removal.
		if (CurTimeLeft == 0)
		{
			EntriesToRemove.Add(Index);
			continue;
		}

		// Update blends.
		if (Entry.CameraShake->BlendIn && 
				Entry.CurrentTime < Entry.CameraShake->BlendIn->BlendTime &&
				ensure(Entry.BlendInEvaluator))
		{
			Entry.BlendInEvaluator->Run(CurParams, CurResult);

			FCameraNodeBlendParams BlendParams(Params, Entry.Result);
			FCameraNodeBlendResult BlendResult(OutResult);
			Entry.BlendInEvaluator->BlendResults(BlendParams, BlendResult);
		}
		else if (Entry.CameraShake->BlendOut &&
				CurTimeLeft >= 0.f && CurTimeLeft < Entry.CameraShake->BlendOut->BlendTime &&
				ensure(Entry.BlendOutEvaluator))
		{
			Entry.BlendOutEvaluator->Run(CurParams, CurResult);

			FCameraNodeBlendParams BlendParams(Params, Entry.Result);
			FCameraNodeBlendResult BlendResult(OutResult);
			Entry.BlendOutEvaluator->BlendResults(BlendParams, BlendResult);
		}
		else
		{
			OutResult.OverrideAll(CurResult);
		}
	}

	// Remove any finished shakes.
	for (int32 Index = EntriesToRemove.Num() - 1; Index >= 0; --Index)
	{
		PopEntry(EntriesToRemove[Index]);
	}
}

void FCameraShakeServiceCameraNodeEvaluator::InitializeEntry(
	FShakeEntry& NewEntry, 
	const UCameraShakeAsset* CameraShake,
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext)
{
	// Generate the hierarchy of node evaluators inside our storage buffer.
	FCameraNodeEvaluatorTreeBuildParams BuildParams;
	BuildParams.RootCameraNode = CameraShake->RootNode;
	BuildParams.AllocationInfo = &CameraShake->AllocationInfo.EvaluatorInfo;
	FCameraNodeEvaluator* RootEvaluator = NewEntry.EvaluatorStorage.BuildEvaluatorTree(BuildParams);

	// Generate the blend-in and blend-out evaluators.
	FBlendCameraNodeEvaluator* BlendInEvaluator = nullptr;
	if (CameraShake->BlendIn)
	{
		FCameraNodeEvaluatorTreeBuildParams BlendBuildParams;
		BlendBuildParams.RootCameraNode = CameraShake->BlendIn;
		BlendInEvaluator = NewEntry.EvaluatorStorage.BuildEvaluatorTree(BlendBuildParams)->CastThis<FBlendCameraNodeEvaluator>();
	}
	FBlendCameraNodeEvaluator* BlendOutEvaluator = nullptr;
	if (CameraShake->BlendOut)
	{
		FCameraNodeEvaluatorTreeBuildParams BlendBuildParams;
		BlendBuildParams.RootCameraNode = CameraShake->BlendOut;
		BlendOutEvaluator = NewEntry.EvaluatorStorage.BuildEvaluatorTree(BlendBuildParams)->CastThis<FBlendCameraNodeEvaluator>();

		const bool bReversed = BlendOutEvaluator->SetReversed(true);
		ensure(bReversed);  // TODO: if the blend can't play in reverse, wrap it in a reverse blend evaluator.
	}

	// Allocate variable table and context data table.
	NewEntry.Result.VariableTable.Initialize(CameraShake->AllocationInfo.VariableTableInfo);
	NewEntry.Result.ContextDataTable.Initialize(CameraShake->AllocationInfo.ContextDataTableInfo);

	// Set all the data from the context.
	const FCameraNodeEvaluationResult& ContextResult = EvaluationContext->GetInitialResult();
	NewEntry.Result.VariableTable.OverrideAll(ContextResult.VariableTable, true);
	NewEntry.Result.ContextDataTable.OverrideAll(ContextResult.ContextDataTable);

	// Initialize the node evaluators.
	if (RootEvaluator)
	{
		FCameraNodeEvaluatorInitializeParams InitParams(&NewEntry.EvaluatorHierarchy);
		InitParams.Evaluator = OwningEvaluator;
		InitParams.EvaluationContext = EvaluationContext;
		RootEvaluator->Initialize(InitParams, NewEntry.Result);
	}

	// Wrap up!
	NewEntry.EvaluationContext = EvaluationContext;
	NewEntry.CameraShake = CameraShake;
	NewEntry.BlendInEvaluator = BlendInEvaluator;
	NewEntry.BlendOutEvaluator = BlendOutEvaluator;
	NewEntry.bIsFirstFrame = true;
	if (RootEvaluator)
	{
		NewEntry.RootEvaluator = RootEvaluator->CastThisChecked<FShakeCameraNodeEvaluator>();
	}
}

void FCameraShakeServiceCameraNodeEvaluator::PopEntry(int32 EntryIndex)
{
	if (!ensure(Entries.IsValidIndex(EntryIndex)))
	{
		return;
	}

	Entries.RemoveAt(EntryIndex);
}

UE_DEFINE_CAMERA_EVALUATION_SERVICE(FCameraShakeService)

void FCameraShakeService::OnInitialize(const FCameraEvaluationServiceInitializeParams& Params)
{
	SetEvaluationServiceFlags(ECameraEvaluationServiceFlags::None);

	ensure(Evaluator == nullptr);
	Evaluator = Params.Evaluator;
}

void FCameraShakeService::OnTeardown(const FCameraEvaluationServiceTeardownParams& Params)
{
	ensure(Evaluator != nullptr);
	Evaluator = nullptr;
}

void FCameraShakeService::StartCameraShake(const FStartCameraShakeParams& Params)
{
	EnsureShakeContextCreated();

	if (ensure(ShakeEvaluator))
	{
		ShakeEvaluator->StartCameraShake(Params);
	}
}

void FCameraShakeService::RequestCameraShakeThisFrame(const FStartCameraShakeParams& Params)
{
	EnsureShakeContextCreated();

	if (ensure(ShakeEvaluator))
	{
		ShakeEvaluator->RequestCameraShakeThisFrame(Params);
	}
}

void FCameraShakeService::EnsureShakeContextCreated()
{
	// Create the evaluation context, which is a "null" context with no particular logic.
	if (!ShakeContext)
	{
		ShakeContext = MakeShared<FCameraEvaluationContext>();
		ShakeContext->GetInitialResult().bIsValid = true;
	}

	// Create the camera rig that will contain and run all the camera shakes.
	if (!ShakeContainerRig)
	{
		ShakeContainerRig = NewObject<UCameraRigAsset>(GetTransientPackage(), TEXT("CameraShakeContainerRig"), RF_Transient);
		ShakeContainerRig->RootNode = NewObject<UCameraShakeServiceCameraNode>(ShakeContainerRig, NAME_None, RF_Transient);
		ShakeContainerRig->BuildCameraRig();
	}

	// Instantiate the "container" camera rig inside the visual layer.
	if (!ShakeEvaluator)
	{
		FRootCameraNodeEvaluator* RootEvaluator = Evaluator->GetRootNodeEvaluator();

		FActivateCameraRigParams ActivateParams;
		ActivateParams.EvaluationContext = ShakeContext;
		ActivateParams.CameraRig = ShakeContainerRig;
		ActivateParams.Layer = ECameraRigLayer::Visual;
		FCameraRigInstanceID InstanceID = RootEvaluator->ActivateCameraRig(ActivateParams);

		FCameraRigEvaluationInfo ShakeContainerRigInfo;
		RootEvaluator->GetCameraRigInfo(InstanceID, ShakeContainerRigInfo);

		if (ShakeContainerRigInfo.RootEvaluator)
		{
			ShakeEvaluator = ShakeContainerRigInfo.RootEvaluator->CastThis<FCameraShakeServiceCameraNodeEvaluator>();
		}
		ensure(ShakeEvaluator);
	}
}

}  // namespace UE::Cameras

FCameraNodeEvaluatorPtr UCameraShakeServiceCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FCameraShakeServiceCameraNodeEvaluator>();
}

