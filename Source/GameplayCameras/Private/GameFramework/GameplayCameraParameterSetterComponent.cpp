// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraParameterSetterComponent.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/CameraVariableSetter.h"
#include "Core/RootCameraNode.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameplayCameraSystemHost.h"
#include "Kismet/GameplayStatics.h"

UGameplayCameraParameterSetterComponent::UGameplayCameraParameterSetterComponent(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
}

void UGameplayCameraParameterSetterComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->OnActorBeginOverlap.AddDynamic(this, &UGameplayCameraParameterSetterComponent::OnActorBeginOverlap);
		OwnerActor->OnActorEndOverlap.AddDynamic(this, &UGameplayCameraParameterSetterComponent::OnActorEndOverlap);
	}
}

void UGameplayCameraParameterSetterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->OnActorBeginOverlap.RemoveAll(this);
		OwnerActor->OnActorEndOverlap.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UGameplayCameraParameterSetterComponent::OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	StartParameterSetters();
}

void UGameplayCameraParameterSetterComponent::OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	StopParameterSetters(false);
}

UE::Cameras::FRootCameraNodeEvaluator* UGameplayCameraParameterSetterComponent::GetRootNodeEvaluator()
{
	using namespace UE::Cameras;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
	{
		return nullptr;
	}

	UGameplayCameraSystemHost* CameraSystemHost = UGameplayCameraSystemHost::FindOrCreateHost(PlayerController);
	if (!CameraSystemHost)
	{
		return nullptr;
	}

	TSharedPtr<FCameraSystemEvaluator> SystemEvaluator = CameraSystemHost->GetCameraSystemEvaluator();
	if (!SystemEvaluator)
	{
		return nullptr;
	}

	FRootCameraNodeEvaluator* RootNodeEvaluator = SystemEvaluator->GetRootNodeEvaluator();

	return RootNodeEvaluator;
}

void UGameplayCameraParameterSetterComponent::StartParameterSetters()
{
	using namespace UE::Cameras;

	if (!CameraRigReference.GetCameraRig())
	{
		return;
	}

	FRootCameraNodeEvaluator* RootNodeEvaluator = GetRootNodeEvaluator();
	if (!RootNodeEvaluator)
	{
		return;
	}

	const UCameraRigAsset* CameraRig = CameraRigReference.GetCameraRig();
	const FInstancedPropertyBag& ParameterValues = CameraRigReference.GetParameters();
	const uint8* ParameterValuesPtr = ParameterValues.GetValue().GetMemory();

	for (const FCameraObjectInterfaceParameterDefinition& ParameterDefinition : CameraRig->GetParameterDefinitions())
	{
		// TODO
		if (ParameterDefinition.ParameterType != ECameraObjectInterfaceParameterType::Blendable)
		{
			continue;
		}

		if (!CameraRigReference.IsParameterOverridden(ParameterDefinition.ParameterGuid))
		{
			continue;
		}

		const FPropertyBagPropertyDesc* PropertyDesc = ParameterValues.FindPropertyDescByID(ParameterDefinition.ParameterGuid);
		if (!PropertyDesc || !PropertyDesc->CachedProperty)
		{
			continue;
		}

		FCameraVariableSetterHandle SetterHandle;
		const void* RawValue = PropertyDesc->CachedProperty->ContainerPtrToValuePtr<void>(ParameterValuesPtr);

		switch (ParameterDefinition.VariableType)
		{
#define UE_CAMERA_VARIABLE_FOR_TYPE(VariableType, VariableName)\
			case ECameraVariableType::VariableName:\
				{\
					TCameraVariableSetter<VariableType> Setter(\
							ParameterDefinition.VariableID, *reinterpret_cast<const VariableType*>(RawValue));\
					SetterHandle = RootNodeEvaluator->AddCameraVariableSetter(Setter);\
				}\
				break;
			UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
			default:
				break;
		}

		if (SetterHandle.IsValid())
		{
			SetterHandles.Add(SetterHandle);
		}
	}
}

void UGameplayCameraParameterSetterComponent::StopParameterSetters(bool bImmediately)
{
	using namespace UE::Cameras;

	FRootCameraNodeEvaluator* RootNodeEvaluator = GetRootNodeEvaluator();
	if (!RootNodeEvaluator)
	{
		return;
	}

	for (FCameraVariableSetterHandle Handle : SetterHandles)
	{
		RootNodeEvaluator->StopCameraVariableSetter(Handle, bImmediately);
	}
}

