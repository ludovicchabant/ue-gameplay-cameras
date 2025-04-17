// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraRigComponent.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Directors/SingleCameraDirector.h"
#include "GameplayCamerasDelegates.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraRigComponent)

UGameplayCameraRigComponent::UGameplayCameraRigComponent(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UGameplayCameraRigComponent::OnRegister()
{
	using namespace UE::Cameras;

	Super::OnRegister();

#if WITH_EDITOR

	FGameplayCamerasDelegates::OnCameraRigAssetBuilt().AddUObject(this, &UGameplayCameraRigComponent::OnCameraRigAssetBuilt);

#endif
}

void UGameplayCameraRigComponent::OnUnregister()
{
	using namespace UE::Cameras;

#if WITH_EDITOR

	FGameplayCamerasDelegates::OnCameraRigAssetBuilt().RemoveAll(this);

#endif  // WITH_EDITOR

	Super::OnUnregister();
}

UCameraAsset* UGameplayCameraRigComponent::GetCameraAsset()
{
	if (!GeneratedCameraAsset)
	{
		USingleCameraDirector* SingleDirector = NewObject<USingleCameraDirector>(this, TEXT("GeneratedCameraDirector"), RF_Transient);
		SingleDirector->CameraRig = CameraRigReference.GetCameraRig();

		GeneratedCameraAsset = NewObject<UCameraAsset>(this, TEXT("GeneratedCameraAsset"), RF_Transient);
		GeneratedCameraAsset->SetCameraDirector(SingleDirector);

		GeneratedCameraAsset->BuildCamera();
	}

	return GeneratedCameraAsset;
}

bool UGameplayCameraRigComponent::OnValidateCameraEvaluationContextActivation()
{
	if (!CameraRigReference.IsValid())
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't activate gameplay camera rig component: no camera rig asset was set!"),
				ELogVerbosity::Error);
		return false;
	}
	
	return true;
}

void UGameplayCameraRigComponent::OnUpdateCameraEvaluationContext(bool bForceApplyParameterOverrides)
{
	using namespace UE::Cameras;

	FCameraNodeEvaluationResult& InitialResult = GetEvaluationContext()->GetInitialResult();

	const bool bApplyDrivenParametersOnly = !bForceApplyParameterOverrides;
	CameraRigReference.ApplyParameterOverrides(InitialResult, bApplyDrivenParametersOnly);
}

#if WITH_EDITOR

void UGameplayCameraRigComponent::OnCameraRigAssetBuilt(const UCameraRigAsset* InCameraRigAsset)
{
	using namespace UE::Cameras;

	if (InCameraRigAsset != CameraRigReference.GetCameraRig())
	{
		return;
	}

	// If our camera rig asset was just built, it may have some new parameters. We need to rebuild
	// our variable table and context data table, and re-apply overrides.
	if (HasCameraEvaluationContext())
	{
		const FCameraObjectAllocationInfo& AllocationInfo = InCameraRigAsset->AllocationInfo;
		FCameraNodeEvaluationResult& InitialResult = GetEvaluationContext()->GetInitialResult();
		InitialResult.VariableTable.Initialize(AllocationInfo.VariableTableInfo);
		InitialResult.ContextDataTable.Initialize(AllocationInfo.ContextDataTableInfo);

		CameraRigReference.RebuildParametersIfNeeded();

		UpdateCameraEvaluationContext(true);
	}
}

void UGameplayCameraRigComponent::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent)
{
	using namespace UE::Cameras;

	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UGameplayCameraRigComponent, CameraRigReference))
	{
		if (HasCameraEvaluationContext())
		{
			if (PropertyChangedEvent.GetPropertyName() == TEXT("CameraRig"))
			{
				// The camera rig asset has changed! Recreate the context.
				GeneratedCameraAsset = nullptr;
				RecreateCameraEvaluationContext();
			}
			else
			{
				// Otherwise, maybe one of the parameter overrides has changed. Re-apply them.
				UpdateCameraEvaluationContext(true);
			}
		}
	}
}

#endif  // WITH_EDITOR

