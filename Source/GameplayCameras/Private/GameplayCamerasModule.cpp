// Copyright Epic Games, Inc. All Rights Reserved.

#include "IGameplayCamerasModule.h"

#include "Camera/CameraModularFeature.h"
#include "CameraAnimationCameraModifier.h"
#include "CameraAnimationSequencePlayer.h"
#include "Debug/CameraDebugColors.h"
#include "Features/IModularFeatures.h"
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
		CameraModularFeature = MakeShared<FCameraModularFeature>();
		if (CameraModularFeature.IsValid())
		{
			IModularFeatures::Get().RegisterModularFeature(ICameraModularFeature::GetModularFeatureName(), CameraModularFeature.Get());
		}

#if UE_GAMEPLAY_CAMERAS_DEBUG
		UE::Cameras::FCameraDebugColors::RegisterBuiltinColorSchemes();
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
	}

	virtual void ShutdownModule() override
	{
		if (CameraModularFeature.IsValid())
		{
			IModularFeatures::Get().UnregisterModularFeature(ICameraModularFeature::GetModularFeatureName(), CameraModularFeature.Get());
			CameraModularFeature = nullptr;
		}
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

	class FCameraModularFeature : public ICameraModularFeature
	{
		// ICameraModularFeature interface
		virtual void GetDefaultModifiers(TArray<TSubclassOf<UCameraModifier>>& ModifierClasses) const override
		{
			ModifierClasses.Add(UCameraAnimationCameraModifier::StaticClass());
		}
	};

	TSharedPtr<FCameraModularFeature> CameraModularFeature;

#if WITH_EDITOR
	TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager;
#endif
};

IMPLEMENT_MODULE(FGameplayCamerasModule, GameplayCameras);

#undef LOCTEXT_NAMESPACE

