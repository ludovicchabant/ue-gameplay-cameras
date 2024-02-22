// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCamerasEditorSettings.h"

#include "AssetTools/CameraAssetEditor.h"
#include "AssetTools/CameraRigAssetEditor.h"
#include "Commands/CameraAssetEditorCommands.h"
#include "Commands/CameraRigAssetEditorCommands.h"
#include "GameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasEditorModule.h"
#include "IGameplayCamerasModule.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "ToolMenus.h"
#include "Toolkits/CameraAssetEditorToolkit.h"
#include "Toolkits/CameraRigAssetEditorToolkit.h"

#define LOCTEXT_NAMESPACE "GameplayCamerasEditor"

const FName IGameplayCamerasEditorModule::GameplayCamerasEditorAppIdentifier("GameplayCamerasEditorApp");
const FName IGameplayCamerasEditorModule::CameraRigAssetEditorToolBarName("CameraRigAssetEditor.ToolBar");

/**
 * Implements the FGameplayCamerasEditor module.
 */
class FGameplayCamerasEditorModule : public IGameplayCamerasEditorModule
{
public:
	FGameplayCamerasEditorModule()
	{
	}

	virtual void StartupModule() override
	{
		RegisterSettings();
		InitializeLiveEditManager();

		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(
					this, &FGameplayCamerasEditorModule::RegisterMenus));
	}

	virtual void ShutdownModule() override
	{
		UToolMenus::UnRegisterStartupCallback(this);

		FCameraAssetEditorCommands::Unregister();
		FCameraRigAssetEditorCommands::Unregister();

		UnregisterSettings();
		TeardownLiveEditManager();
	}

	virtual UCameraAssetEditor* CreateCameraAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCameraAsset* CameraAsset) override
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		UCameraAssetEditor* AssetEditor = NewObject<UCameraAssetEditor>(AssetEditorSubsystem, NAME_None, RF_Transient);
		AssetEditor->Initialize(CameraAsset);
		return AssetEditor;
	}

	virtual UCameraRigAssetEditor* CreateCameraRigEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCameraRigAsset* CameraRig) override
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		UCameraRigAssetEditor* AssetEditor = NewObject<UCameraRigAssetEditor>(AssetEditorSubsystem, NAME_None, RF_Transient);
		AssetEditor->Initialize(CameraRig);
		return AssetEditor;
	}

private:

	void RegisterSettings()
	{
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->RegisterSettings("Project", "Plugins", "GameplayCamerasEditor",
				LOCTEXT("GameplayCamerasEditorProjectSettingsName", "Gameplay Cameras Editor"),
				LOCTEXT("GameplayCamerasEditorProjectSettingsDescription", "Configure the gameplay cameras editor."),
				GetMutableDefault<UGameplayCamerasEditorSettings>()
			);
		}
	}

	void UnregisterSettings()
	{
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->UnregisterSettings("Project", "Plugins", "GameplayCamerasEditor");
			SettingsModule->UnregisterSettings("Editor", "ContentEditors", "GameplayCamerasEditor");
		}
	}

	void RegisterMenus()
	{
		FCameraAssetEditorCommands::Register();
		FCameraRigAssetEditorCommands::Register();	
	}

	void InitializeLiveEditManager()
	{
		using namespace UE::Cameras;

		LiveEditManager = MakeShared<FGameplayCamerasLiveEditManager>();

		IGameplayCamerasModule& CamerasModule = FModuleManager::LoadModuleChecked<IGameplayCamerasModule>("GameplayCameras");
		CamerasModule.SetLiveEditManager(LiveEditManager);

		FCoreUObjectDelegates::GetPostGarbageCollect().AddRaw(this, &FGameplayCamerasEditorModule::OnPostGarbageCollection);
	}

	void TeardownLiveEditManager()
	{
		FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);

		IGameplayCamerasModule& CamerasModule = FModuleManager::LoadModuleChecked<IGameplayCamerasModule>("GameplayCameras");
		CamerasModule.SetLiveEditManager(nullptr);

		LiveEditManager.Reset();
	}
	
	void OnPostGarbageCollection()
	{
	}

private:

	TSharedPtr<UE::Cameras::FGameplayCamerasLiveEditManager> LiveEditManager;
};

IMPLEMENT_MODULE(FGameplayCamerasEditorModule, GameplayCamerasEditor);

#undef LOCTEXT_NAMESPACE

