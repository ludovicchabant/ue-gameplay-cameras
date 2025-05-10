// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraComponent.h"

#include "Core/CameraAsset.h"
#include "GameplayCamerasDelegates.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraComponent)

#define LOCTEXT_NAMESPACE "GameplayCameraComponent"

UGameplayCameraComponent::UGameplayCameraComponent(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UGameplayCameraComponent::PostLoad()
{
	Super::PostLoad();

	if (Camera_DEPRECATED)
	{
		CameraReference.SetCameraAsset(Camera_DEPRECATED);
		Camera_DEPRECATED = nullptr;
	}
}

void UGameplayCameraComponent::OnRegister()
{
	using namespace UE::Cameras;

	Super::OnRegister();

#if WITH_EDITOR

	FGameplayCamerasDelegates::OnCameraAssetBuilt().AddUObject(this, &UGameplayCameraComponent::OnCameraAssetBuilt);

#endif
}

void UGameplayCameraComponent::OnUnregister()
{
	using namespace UE::Cameras;

#if WITH_EDITOR

	FGameplayCamerasDelegates::OnCameraAssetBuilt().RemoveAll(this);

#endif  // WITH_EDITOR

	Super::OnUnregister();
}

UCameraAsset* UGameplayCameraComponent::GetCameraAsset()
{
	return CameraReference.GetCameraAsset();
}

bool UGameplayCameraComponent::OnValidateCameraEvaluationContextActivation()
{
	const bool bIsValid = CameraReference.IsValid();
	if (!bIsValid && !IsEditorWorld())
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't activate gameplay camera component: no camera asset was set!"),
				ELogVerbosity::Error);
	}
	return bIsValid;
}

void UGameplayCameraComponent::OnUpdateCameraEvaluationContext(bool bForceApplyParameterOverrides)
{
	using namespace UE::Cameras;

	FCameraNodeEvaluationResult& InitialResult = GetEvaluationContext()->GetInitialResult();

	const bool bApplyDrivenParametersOnly = !bForceApplyParameterOverrides;
	CameraReference.ApplyParameterOverrides(InitialResult, bApplyDrivenParametersOnly);
}

#if WITH_EDITOR

void UGameplayCameraComponent::OnCameraAssetBuilt(const UCameraAsset* InCameraAsset)
{
	using namespace UE::Cameras;

	if (InCameraAsset != CameraReference.GetCameraAsset())
	{
		return;
	}

	// If our camera asset was just built, it may have some new parameters. We need to rebuild
	// our variable table and context data table, and re-apply overrides.
	CameraReference.RebuildParametersIfNeeded();
	if (HasCameraEvaluationContext())
	{
		const FCameraAssetAllocationInfo& AllocationInfo = InCameraAsset->GetAllocationInfo();
		ReinitializeCameraEvaluationContext(AllocationInfo.VariableTableInfo, AllocationInfo.ContextDataTableInfo);
		UpdateCameraEvaluationContext(true);
	}
}

void UGameplayCameraComponent::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent)
{
	using namespace UE::Cameras;

	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UGameplayCameraComponent, CameraReference))
	{
		if (HasCameraEvaluationContext())
		{
			if (PropertyChangedEvent.GetPropertyName() == TEXT("CameraAsset"))
			{
				// The camera asset has changed! Recreate the context.
				RecreateEditorWorldCameraEvaluationContext();
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

#undef LOCTEXT_NAMESPACE

