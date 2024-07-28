// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Collision/OcclusionMaterialCameraNode.h"

#include "CollisionQueryParams.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/Map.h"
#include "Containers/Set.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraParameterReader.h"
#include "Core/CameraSystemEvaluator.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameplayCameras.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/AssertionMacros.h"
#include "WorldCollision.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OcclusionMaterialCameraNode)

namespace UE::Cameras
{

struct FOcclusionMaterialOverrideInfo
{
	TArray<UMaterialInterface*> OriginalMaterials;
	TArray<UMaterialInterface*> OverrideMaterials;
};

class FOcclusionMaterialCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FOcclusionMaterialCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	void RunOcclusionTrace(UWorld* World, APlayerController* PlayerController, FCameraNodeEvaluationResult& OutResult);
	void HandleOcclusionTraceResult(UWorld* World);

	void ApplyOcclusionMaterial(TSet<UStaticMeshComponent*> MeshComponents);
	void RemoveOcclusionMaterial(TSet<UStaticMeshComponent*> MeshComponents);

private:

	TCameraParameterReader<float> OcclusionSphereRadiusReader;
	TCameraParameterReader<FVector3d> OcclusionTargetOffsetReader;

	FTraceHandle OcclusionTraceHandle;
	TSet<UStaticMeshComponent*> CurrentlyOccludedMeshComponents;
	TMap<UStaticMeshComponent*, FOcclusionMaterialOverrideInfo> AppliedMaterialOverrides;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FOcclusionMaterialCameraNodeEvaluator)

void FOcclusionMaterialCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UOcclusionMaterialCameraNode* OcclusionMaterialNode = GetCameraNodeAs<UOcclusionMaterialCameraNode>();
	OcclusionSphereRadiusReader.Initialize(OcclusionMaterialNode->OcclusionSphereRadius);
	OcclusionTargetOffsetReader.Initialize(OcclusionMaterialNode->OcclusionTargetOffset);
}

void FOcclusionMaterialCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	if (!ensure(Params.EvaluationContext))
	{
		return;
	}

	if (Params.EvaluationType != ECameraNodeEvaluationType::Standard)
	{
		// Don't run occlusion traces during IK/stateless updates.
		return;
	}

	UWorld* World = Params.EvaluationContext->GetWorld();
	APlayerController* PlayerController = Params.EvaluationContext->GetPlayerController();
	if (!World || !PlayerController)
	{
		return;
	}

	HandleOcclusionTraceResult(World);
	RunOcclusionTrace(World, PlayerController, OutResult);
}

void FOcclusionMaterialCameraNodeEvaluator::RunOcclusionTrace(UWorld* World, APlayerController* PlayerController, FCameraNodeEvaluationResult& OutResult)
{
	static FName OcclusionTraceTag(TEXT("CameraOcclusion"));
	static FName OcclusionTraceOwnerTag(TEXT("OcclusionMaterialCameraNode"));

	APawn* Pawn = PlayerController->GetPawn();

	// TODO fix this hardcoded nonsense
	const FVector3d OcclusionTarget = Pawn->GetActorLocation();

	const UOcclusionMaterialCameraNode* OcclusionMaterialNode = GetCameraNodeAs<UOcclusionMaterialCameraNode>();
	ECollisionChannel OcclusionChannel = OcclusionMaterialNode->OcclusionChannel;

	const float OcclusionSphereRadius = OcclusionSphereRadiusReader.Get(OutResult.VariableTable);
	const FVector3d TraceEndOffset = OcclusionTargetOffsetReader.Get(OutResult.VariableTable);

	const FVector3d TraceStart(OutResult.CameraPose.GetLocation());
	const FVector3d TraceEnd(OcclusionTarget + TraceEndOffset);

	FCollisionShape SweepShape = FCollisionShape::MakeSphere(OcclusionSphereRadius);
	// Ignore the player pawn by default.
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(StartOcclusionSweep), false, Pawn);
	QueryParams.TraceTag = OcclusionTraceTag;
	QueryParams.OwnerTag = OcclusionTraceOwnerTag;

	OcclusionTraceHandle = World->AsyncSweepByChannel(
			EAsyncTraceType::Multi, 
			TraceStart, TraceEnd, FQuat::Identity, 
			OcclusionChannel, 
			SweepShape,
			QueryParams, 
			FCollisionResponseParams::DefaultResponseParam);
}

void FOcclusionMaterialCameraNodeEvaluator::HandleOcclusionTraceResult(UWorld* World)
{
	// Do some basic validation... right now we just bail out if we can't get the trace result
	// without figuring out if it's too old, still running, or whatever else. This is because
	// we're supposed to be running only once a frame, so our trace should have run last frame
	// and be available now. We'll have to better handle error cases when we start doing multi
	// evaluations.
	if (!OcclusionTraceHandle.IsValid())
	{
		return;
	}

	FTraceDatum TraceDatum;
	if (!World->QueryTraceData(OcclusionTraceHandle, TraceDatum))
	{
		return;
	}

	// Get the list of meshes collected by the occlusion trace, and figure out which ones are
	// new, and which ones got out of the way.
	TSet<UStaticMeshComponent*> MeshComponents;
	for (const FHitResult& Hit : TraceDatum.OutHits)
	{
		UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(Hit.GetComponent());
		if (MeshComponent)
		{
			MeshComponents.Add(MeshComponent);
		}
	}

	TSet<UStaticMeshComponent*> NewMeshComponents = MeshComponents.Difference(CurrentlyOccludedMeshComponents);
	TSet<UStaticMeshComponent*> OldMeshComponents = CurrentlyOccludedMeshComponents.Difference(MeshComponents);

	CurrentlyOccludedMeshComponents = MeshComponents;

	// Apply occlusion material changes to new/old components.
	ApplyOcclusionMaterial(NewMeshComponents);
	RemoveOcclusionMaterial(OldMeshComponents);

	OcclusionTraceHandle.Invalidate();
}

void FOcclusionMaterialCameraNodeEvaluator::ApplyOcclusionMaterial(TSet<UStaticMeshComponent*> MeshComponents)
{
	const UOcclusionMaterialCameraNode* OcclusionMaterialNode = GetCameraNodeAs<UOcclusionMaterialCameraNode>();
	UMaterialInterface* OcclusionTransparencyMaterial = OcclusionMaterialNode->OcclusionTransparencyMaterial;
	if (!ensureMsgf(OcclusionTransparencyMaterial, TEXT("No occlusion transparency material set!")))
	{
		return;
	}

	for (UStaticMeshComponent* MeshComponent : MeshComponents)
	{
		if (!MeshComponent || AppliedMaterialOverrides.Contains(MeshComponent))
		{
			continue;
		}

		FOcclusionMaterialOverrideInfo& MaterialOverride = AppliedMaterialOverrides.Add(MeshComponent);
		for (int32 MaterialIndex = 0; MaterialIndex < MeshComponent->GetNumMaterials(); ++MaterialIndex)
		{
			UMaterialInterface* OriginalMaterial = MeshComponent->GetMaterial(MaterialIndex);
			UMaterialInterface* OverrideMaterial = MeshComponent->CreateDynamicMaterialInstance(MaterialIndex, OcclusionTransparencyMaterial);
			MaterialOverride.OriginalMaterials.Add(OriginalMaterial);
			MaterialOverride.OverrideMaterials.Add(OverrideMaterial);
			MeshComponent->SetMaterial(MaterialIndex, OverrideMaterial);
		}
	}
}

void FOcclusionMaterialCameraNodeEvaluator::RemoveOcclusionMaterial(TSet<UStaticMeshComponent*> MeshComponents)
{
	for (UStaticMeshComponent* MeshComponent : MeshComponents)
	{
		if (!MeshComponent)
		{
			continue;
		}

		FOcclusionMaterialOverrideInfo MaterialOverrides;
		const bool bFoundEntry = AppliedMaterialOverrides.RemoveAndCopyValue(MeshComponent, MaterialOverrides);
		if (bFoundEntry)
		{
			const int32 NumMaterials = MaterialOverrides.OriginalMaterials.Num();
			for (int32 MaterialIndex = 0; MaterialIndex < NumMaterials; ++MaterialIndex)
			{
				MeshComponent->SetMaterial(MaterialIndex, MaterialOverrides.OriginalMaterials[MaterialIndex]);
			}
		}
	}
}

}  // namespace UE::Cameras

UOcclusionMaterialCameraNode::UOcclusionMaterialCameraNode(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	OcclusionSphereRadius.Value = 10.f;
}

FCameraNodeEvaluatorPtr UOcclusionMaterialCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FOcclusionMaterialCameraNodeEvaluator>();
}

