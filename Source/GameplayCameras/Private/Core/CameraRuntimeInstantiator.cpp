// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRuntimeInstantiator.h"

#include "Core/CameraDirector.h"
#include "Core/CameraNode.h"
#include "CoreGlobals.h"
#include "IGameplayCamerasModule.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "Modules/ModuleManager.h"
#include "UObject/UObjectGlobals.h"

IGameplayCamerasModule* FCameraRuntimeInstantiator::GameplayCamerasModule(nullptr);

IGameplayCamerasModule& FCameraRuntimeInstantiator::GetGameplayCamerasModule()
{
	if (!GameplayCamerasModule)
	{
		GameplayCamerasModule = &FModuleManager::LoadModuleChecked<IGameplayCamerasModule>("GameplayCameras");
	}
	return *GameplayCamerasModule;
}

UCameraNode* FCameraRuntimeInstantiator::InstantiateCameraNodeTree(const UCameraNode* InRootNode, const FCameraRuntimeInstantiationParams& Params)
{
	UCameraNode* NewRootNode = CastChecked<UCameraNode>(InstantiateObject(InRootNode, Params));
	return NewRootNode;
}

UCameraDirector* FCameraRuntimeInstantiator::InstantiateCameraDirector(const UCameraDirector* InCameraDirector, const FCameraRuntimeInstantiationParams& Params)
{
	UCameraDirector* NewCameraDirector = CastChecked<UCameraDirector>(InstantiateObject(InCameraDirector, Params));
	return NewCameraDirector;
}

UObject* FCameraRuntimeInstantiator::InstantiateObject(const UObject* InSource, const FCameraRuntimeInstantiationParams& Params)
{
	FObjectDuplicationParameters DuplicationParams = InitStaticDuplicateObjectParams(
			InSource,
			Params.InstantiationOuter,
			Params.InstantiationName);
#if WITH_EDITOR
	TMap<UObject*, UObject*> CreatedObjects;
	DuplicationParams.CreatedObjects = &CreatedObjects;
#endif

	UObject* OutRootObject = StaticDuplicateObjectEx(DuplicationParams);

#if WITH_EDITOR
	if (!IsRunningCommandlet())
	{
		for (const TPair<UObject*, UObject*>& Pair : CreatedObjects)
		{
			if (UCameraNode* CreatedNode = Cast<UCameraNode>(Pair.Value))
			{
				CreatedNode->SetIsInstantiated();
			}
		}

		if (TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager = GetGameplayCamerasModule().GetLiveEditManager())
		{
			LiveEditManager->RegisterInstantiatedObjects(CreatedObjects);
		}
	}
#endif

	return OutRootObject;
}

void FCameraRuntimeInstantiator::ForwardPropertyChange(const UObject* Object, const FPropertyChangedEvent& PropertyChangedEvent)
{
#if WITH_EDITOR
	if (TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager = GetGameplayCamerasModule().GetLiveEditManager())
	{
		LiveEditManager->ForwardPropertyChange(Object, PropertyChangedEvent);
	}
#endif
}

