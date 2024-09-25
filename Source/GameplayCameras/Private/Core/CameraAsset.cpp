// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraAsset.h"

#include "Core/CameraAssetBuilder.h"
#include "Core/CameraBuildLog.h"
#include "Core/CameraDirector.h"
#include "Core/CameraRigAsset.h"
#include "UObject/ObjectRedirector.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraAsset)

const FName UCameraAsset::SharedTransitionsGraphName("SharedTransitions");

void UCameraAsset::SetCameraDirector(UCameraDirector* InCameraDirector)
{
	using namespace UE::Cameras;

	if (CameraDirector != InCameraDirector)
	{
		CameraDirector = InCameraDirector;

		TCameraPropertyChangedEvent<UCameraDirector*> ChangedEvent;
		ChangedEvent.NewValue = CameraDirector;
		EventHandlers.Notify(&ICameraAssetEventHandler::OnCameraDirectorChanged, this, ChangedEvent);
	}
}

void UCameraAsset::AddCameraRig(UCameraRigAsset* InCameraRig)
{
	using namespace UE::Cameras;

	ensure(InCameraRig);

	CameraRigs.Add(InCameraRig);

	TCameraArrayChangedEvent<UCameraRigAsset*> ChangedEvent;
	ChangedEvent.EventType = ECameraArrayChangedEventType::Add;
	EventHandlers.Notify(&ICameraAssetEventHandler::OnCameraRigsChanged, this, ChangedEvent);
}

int32 UCameraAsset::RemoveCameraRig(UCameraRigAsset* InCameraRig)
{
	using namespace UE::Cameras;

	const int32 NumRemoved = CameraRigs.Remove(InCameraRig);
	if (NumRemoved > 0)
	{
		TCameraArrayChangedEvent<UCameraRigAsset*> ChangedEvent;
		ChangedEvent.EventType = ECameraArrayChangedEventType::Remove;
		EventHandlers.Notify(&ICameraAssetEventHandler::OnCameraRigsChanged, this, ChangedEvent);
	}
	return NumRemoved;
}

void UCameraAsset::AddEnterTransition(UCameraRigTransition* InTransition)
{
	using namespace UE::Cameras;

	ensure(InTransition);

	EnterTransitions.Add(InTransition);

	TCameraArrayChangedEvent<UCameraRigTransition*> ChangedEvent;
	ChangedEvent.EventType = ECameraArrayChangedEventType::Add;
	EventHandlers.Notify(&ICameraAssetEventHandler::OnEnterTransitionsChanged, this, ChangedEvent);
}

int32 UCameraAsset::RemoveEnterTransition(UCameraRigTransition* InTransition)
{
	using namespace UE::Cameras;
	
	const int32 NumRemoved = EnterTransitions.Remove(InTransition);
	if (NumRemoved > 0)
	{
		TCameraArrayChangedEvent<UCameraRigTransition*> ChangedEvent;
		ChangedEvent.EventType = ECameraArrayChangedEventType::Remove;
		EventHandlers.Notify(&ICameraAssetEventHandler::OnEnterTransitionsChanged, this, ChangedEvent);
	}
	return NumRemoved;
}

void UCameraAsset::AddExitTransition(UCameraRigTransition* InTransition)
{
	using namespace UE::Cameras;

	ensure(InTransition);

	ExitTransitions.Add(InTransition);

	TCameraArrayChangedEvent<UCameraRigTransition*> ChangedEvent;
	ChangedEvent.EventType = ECameraArrayChangedEventType::Add;
	EventHandlers.Notify(&ICameraAssetEventHandler::OnExitTransitionsChanged, this, ChangedEvent);
}

int32 UCameraAsset::RemoveExitTransition(UCameraRigTransition* InTransition)
{
	using namespace UE::Cameras;

	const int32 NumRemoved = ExitTransitions.Remove(InTransition);
	if (NumRemoved > 0)
	{
		TCameraArrayChangedEvent<UCameraRigTransition*> ChangedEvent;
		ChangedEvent.EventType = ECameraArrayChangedEventType::Remove;
		EventHandlers.Notify(&ICameraAssetEventHandler::OnExitTransitionsChanged, this, ChangedEvent);
	}
	return NumRemoved;
}

void UCameraAsset::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	if (CameraDirector)
	{
		EObjectFlags Flags = CameraDirector->GetFlags();
		if (EnumHasAnyFlags(Flags, (RF_Public | RF_Standalone)))
		{
			UE_LOG(LogCameraSystem, Warning, 
					TEXT("Removing incorrect object flags from camera director inside '%s', please re-save the asset."),
					*GetPathNameSafe(this));
			CameraDirector->Modify();
			CameraDirector->ClearFlags(RF_Public | RF_Standalone);
		}
	}

	CleanUpStrayObjects();
#endif  // WITH_EDITOR
}

#if WITH_EDITOR

void UCameraAsset::CleanUpStrayObjects()
{
	UPackage* CameraAssetPackage = GetOutermost();
	if (!CameraAssetPackage || CameraAssetPackage == GetTransientPackage())
	{
		return;
	}

	// Some older versions of the camera editors had a bug that could lead to
	// stray deleted camera rigs being left in the package. Let's clean them up.
	TSet<UObject*> StrayObjects;
	TSet<UCameraRigAsset*> KnownCameraRigs(CameraRigs);

	TArray<UObject*> ObjectsInPackage;
	GetObjectsWithPackage(CameraAssetPackage, ObjectsInPackage);
	for (UObject* Object : ObjectsInPackage)
	{
		UCameraRigAsset* CameraRig = Cast<UCameraRigAsset>(Object);
		if (!CameraRig)
		{
			continue;
		}
		if (KnownCameraRigs.Contains(CameraRig))
		{
			continue;
		}

		Modify();

		CameraRig->ClearFlags(RF_Public | RF_Standalone);
		StrayObjects.Add(CameraRig);
	}

	if (StrayObjects.Num() > 0)
	{
		// Also clean-up any redirectors to these objects.
		for (UObject* Object : ObjectsInPackage)
		{
			if (UObjectRedirector* Redirector = Cast<UObjectRedirector>(Object))
			{
				if (StrayObjects.Contains(Redirector->DestinationObject))
				{
					Redirector->ClearFlags(RF_Public | RF_Standalone);
					Redirector->DestinationObject = nullptr;
				}
			}
		}

		UE_LOG(LogCameraSystem, Warning,
				TEXT("Cleaned up %d stray camera rigs in camera asset '%s'. Please resave the asset."),
				StrayObjects.Num(), *GetPathNameSafe(this));
	}
}


void UCameraAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	using namespace UE::Cameras;

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UCameraAsset, CameraDirector))
	{
		TCameraPropertyChangedEvent<UCameraDirector*> ChangedEvent;
		ChangedEvent.NewValue = CameraDirector;
		EventHandlers.Notify(&ICameraAssetEventHandler::OnCameraDirectorChanged, this, ChangedEvent);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UCameraAsset, CameraRigs))
	{
		TCameraArrayChangedEvent<UCameraRigAsset*> ChangedEvent(PropertyChangedEvent.ChangeType);
		EventHandlers.Notify(&ICameraAssetEventHandler::OnCameraRigsChanged, this, ChangedEvent);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UCameraAsset, EnterTransitions))
	{
		TCameraArrayChangedEvent<UCameraRigTransition*> ChangedEvent(PropertyChangedEvent.ChangeType);
		EventHandlers.Notify(&ICameraAssetEventHandler::OnEnterTransitionsChanged, this, ChangedEvent);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UCameraAsset, ExitTransitions))
	{
		TCameraArrayChangedEvent<UCameraRigTransition*> ChangedEvent(PropertyChangedEvent.ChangeType);
		EventHandlers.Notify(&ICameraAssetEventHandler::OnExitTransitionsChanged, this, ChangedEvent);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif

void UCameraAsset::BuildCamera()
{
	using namespace UE::Cameras;

	FCameraBuildLog BuildLog;
	BuildLog.SetForwardMessagesToLogging(true);
	BuildCamera(BuildLog);
}

void UCameraAsset::BuildCamera(UE::Cameras::FCameraBuildLog& InBuildLog)
{
	using namespace UE::Cameras;

	FCameraAssetBuilder Builder(InBuildLog);
	Builder.BuildCamera(this);
}

void UCameraAsset::DirtyBuildStatus()
{
	BuildStatus = ECameraBuildStatus::Dirty;
}

void UCameraAsset::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
#if WITH_EDITOR

	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		// Build when saving/cooking.
		BuildCamera();
	}

#endif

	Super::PreSave(ObjectSaveContext);
}

#if WITH_EDITOR

void UCameraAsset::GetGraphNodePosition(FName InGraphName, int32& NodePosX, int32& NodePosY) const
{
	NodePosX = TransitionGraphNodePos.X;
	NodePosY = TransitionGraphNodePos.Y;
}

void UCameraAsset::OnGraphNodeMoved(FName InGraphName, int32 NodePosX, int32 NodePosY, bool bMarkDirty)
{
	Modify(bMarkDirty);

	TransitionGraphNodePos.X = NodePosX;
	TransitionGraphNodePos.Y = NodePosY;
}

const FString& UCameraAsset::GetGraphNodeCommentText(FName InGraphName) const
{
	return TransitionGraphNodeComment;
}

void UCameraAsset::OnUpdateGraphNodeCommentText(FName InGraphName, const FString& NewComment)
{
	Modify();

	TransitionGraphNodeComment = NewComment;
}

void UCameraAsset::GetConnectableObjects(FName InGraphName, TSet<UObject*>& OutObjects) const
{
	OutObjects.Append(AllSharedTransitionsObjects);
}

void UCameraAsset::AddConnectableObject(FName InGraphName, UObject* InObject)
{
	Modify();

	const int32 Index = AllSharedTransitionsObjects.AddUnique(InObject);
	ensure(Index == AllSharedTransitionsObjects.Num() - 1);
}

void UCameraAsset::RemoveConnectableObject(FName InGraphName, UObject* InObject)
{
	Modify();

	const int32 NumRemoved = AllSharedTransitionsObjects.Remove(InObject);
	ensure(NumRemoved == 1);
}

#endif  // WITH_EDITOR

