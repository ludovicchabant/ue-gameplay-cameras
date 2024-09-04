// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraSystemHost.h"

#include "Core/CameraSystemEvaluator.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraSystemActor.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "Templates/UnrealTemplate.h"

const TCHAR* UGameplayCameraSystemHost::DefaultHostName = TEXT("GameplayCameraSystemHost");

UGameplayCameraSystemHost* UGameplayCameraSystemHost::FindOrCreateHost(APlayerController* PlayerController, const TCHAR* HostName)
{
	static bool bIsCreatingHost = false;

	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (UGameplayCameraSystemHost* ExistingHost = FindHost(PlayerController, HostName, true))
	{
		return ExistingHost;
	}
	
	if (!ensureMsgf(!bIsCreatingHost, TEXT("Detected reentrant call to UGameplayCameraSystemHost::FindOrCreateHost!")))
	{
		return nullptr;
	}

	TGuardValue<bool> IsCreatingHostGuard(bIsCreatingHost, true);

	if (!HostName)
	{
		HostName = DefaultHostName;
	}

	UGameplayCameraSystemHost* NewHost = NewObject<UGameplayCameraSystemHost>(PlayerController, HostName);

	return NewHost;
}

UGameplayCameraSystemHost* UGameplayCameraSystemHost::FindHost(APlayerController* PlayerController, const TCHAR* HostName, bool bAllowNull)
{
	if (!PlayerController)
	{
		ensureMsgf(bAllowNull, TEXT("Can't find gameplay camera system host: null player controller provided!"));
		return nullptr;
	}

	if (!HostName)
	{
		HostName = DefaultHostName;
	}
	
	UGameplayCameraSystemHost* Host = FindObject<UGameplayCameraSystemHost>(PlayerController, HostName);
	ensureMsgf(Host || bAllowNull, 
			TEXT("Can't find gameplay camera system host named '%s' under player controller '%s'."),
			HostName, *GetNameSafe(PlayerController));
	return Host;
}

UGameplayCameraSystemHost::UGameplayCameraSystemHost(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		Evaluator = MakeShared<FCameraSystemEvaluator>();
		Evaluator->Initialize(this);
	}
}

void UGameplayCameraSystemHost::BeginDestroy()
{
	Super::BeginDestroy();

	Evaluator.Reset();
}

void UGameplayCameraSystemHost::AddReferencedObjects(UObject* Object, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(Object, Collector);

	UGameplayCameraSystemHost* This = CastChecked<UGameplayCameraSystemHost>(Object);
	if (This->Evaluator.IsValid())
	{
		This->Evaluator->AddReferencedObjects(Collector);
	}
}

APlayerController* UGameplayCameraSystemHost::GetPlayerController()
{
	return GetTypedOuter<APlayerController>();
}

TSharedPtr<UE::Cameras::FCameraSystemEvaluator> UGameplayCameraSystemHost::GetCameraSystemEvaluator()
{
	return Evaluator;
}

