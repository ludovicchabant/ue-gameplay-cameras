// Copyright Epic Games, Inc. All Rights Reserved.

#include "IGameplayCamerasModule.h"

#include "Camera/CameraModularFeature.h"
#include "CameraAnimationCameraModifier.h"
#include "CameraAnimationSequencePlayer.h"
#include "Debug/CameraDebugColors.h"
#include "Features/IModularFeatures.h"
#include "GameplayCameras.h"
#include "GameplayCamerasSettings.h"
#include "ISettingsModule.h"
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
		RegisterSettings();

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
		UnregisterSettings();

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

	void RegisterSettings()
	{
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->RegisterSettings("Project", "Plugins", "Gameplay Cameras",
				LOCTEXT("GameplayCamerasProjectSettingsName", "Gameplay Cameras"),
				LOCTEXT("GameplayCamerasProjectSettingsDescription", "Configure gameplay cameras."),
				GetMutableDefault<UGameplayCamerasSettings>()
			);
		}
	}

	void UnregisterSettings()
	{
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->UnregisterSettings("Project", "Plugins", "Gameplay Cameras");
		}
	}

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

