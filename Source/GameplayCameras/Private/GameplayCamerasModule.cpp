// Copyright Epic Games, Inc. All Rights Reserved.

#include "IGameplayCamerasModule.h"

#include "Debug/CameraDebugColors.h"
#include "GameplayCameras.h"
#include "Logging/MessageLog.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "GameplayCamerasModule"

DEFINE_LOG_CATEGORY(LogCameraSystem);

IGameplayCamerasModule& IGameplayCamerasModule::Get()
{
	return FModuleManager::LoadModuleChecked<IGameplayCamerasModule>("GameplayCameras");
}

class FGameplayCamerasModule : public IGameplayCamerasModule
{
public:

	// IModuleInterface interface
	virtual void StartupModule() override
	{
#if UE_GAMEPLAY_CAMERAS_DEBUG
		UE::Cameras::FCameraDebugColors::RegisterBuiltinColorSchemes();
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
	}

	virtual void ShutdownModule() override
	{
	}

public:

	// IGameplayCamerasModule interface
#if WITH_EDITOR
	virtual TSharedPtr<IGameplayCamerasLiveEditManager> GetLiveEditManager() const override
	{
		return LiveEditManager;
	}

	virtual void SetLiveEditManager(TSharedPtr<IGameplayCamerasLiveEditManager> InLiveEditManager) override
	{
		LiveEditManager = InLiveEditManager;
	}
#endif

private:

#if WITH_EDITOR
	TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager;
#endif
};

IMPLEMENT_MODULE(FGameplayCamerasModule, GameplayCameras);

#undef LOCTEXT_NAMESPACE

