// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCamerasEditorSettings.h"

#include "AssetTools/CameraAssetEditor.h"
#include "AssetTools/CameraRigAssetEditor.h"
#include "Commands/CameraAssetEditorCommands.h"
#include "Commands/CameraRigAssetEditorCommands.h"
#include "Commands/GameplayCamerasDebuggerCommands.h"
#include "Debug/CameraDebugCategories.h"
#include "Debugger/SBlendStacksDebugPanel.h"
#include "Debugger/SCameraNodeTreeDebugPanel.h"
#include "Debugger/SGameplayCamerasDebugger.h"
#include "Features/IModularFeatures.h"
#include "GameplayCameras.h"
#include "GameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasEditorModule.h"
#include "IGameplayCamerasModule.h"
#include "IRewindDebuggerExtension.h"
#include "ISettingsModule.h"
#include "Misc/CoreDelegates.h"
#include "Modules/ModuleManager.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "ToolMenus.h"
#include "Toolkits/CameraAssetEditorToolkit.h"
#include "Toolkits/CameraRigAssetEditorToolkit.h"
#include "Trace/CameraSystemRewindDebuggerExtension.h"
#include "Trace/CameraSystemRewindDebuggerTrack.h"
#include "Trace/CameraSystemTraceModule.h"

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
		if (GEditor)
		{
			OnPostEngineInit();
		}
		else
		{
			FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGameplayCamerasEditorModule::OnPostEngineInit);
		}

		FCoreDelegates::OnEnginePreExit.AddRaw(this, &FGameplayCamerasEditorModule::OnPreExit);

		RegisterSettings();
		RegisterCoreDebugCategories();
		RegisterRewindDebuggerFeatures();
		InitializeLiveEditManager();

		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(
					this, &FGameplayCamerasEditorModule::RegisterMenus));
	}

	virtual void ShutdownModule() override
	{
		using namespace UE::Cameras;

		UToolMenus::UnRegisterStartupCallback(this);

		FCameraAssetEditorCommands::Unregister();
		FCameraRigAssetEditorCommands::Unregister();
		FGameplayCamerasDebuggerCommands::Unregister();

		UnregisterSettings();
		UnregisterCoreDebugCategories();
		UnregisterRewindDebuggerFeatures();
		TeardownLiveEditManager();

		FCoreDelegates::OnPostEngineInit.RemoveAll(this);
		FCoreDelegates::OnEnginePreExit.RemoveAll(this);
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

	virtual void RegisterDebugCategory(const UE::Cameras::FCameraDebugCategoryInfo& InCategoryInfo) override
	{
		if (!ensureMsgf(!InCategoryInfo.Name.IsEmpty(), TEXT("A debug category must at least specify a name!")))
		{
			return;
		}

		DebugCategoryInfos.Add(InCategoryInfo.Name, InCategoryInfo);
	}

	virtual void GetRegisteredDebugCategories(TArray<UE::Cameras::FCameraDebugCategoryInfo>& OutCategoryInfos) override
	{
		DebugCategoryInfos.GenerateValueArray(OutCategoryInfos);
	}

	virtual void UnregisterDebugCategory(const FString& InCategoryName)
	{
		DebugCategoryInfos.Remove(InCategoryName);

	}

	virtual void RegisterDebugCategoryPanel(const FString& InDebugCategory, FOnCreateDebugCategoryPanel OnCreatePanel) override
	{
		if (!DebugCategoryPanelCreators.Contains(InDebugCategory))
		{
			DebugCategoryPanelCreators.Add(InDebugCategory, OnCreatePanel);
		}
		else
		{
			// Override existing creator... for games and projects that want to extend a panel with extra controls.
			DebugCategoryPanelCreators[InDebugCategory] = OnCreatePanel;
		}
	}

	virtual TSharedPtr<SWidget> CreateDebugCategoryPanel(const FString& InDebugCategory) override
	{
		if (FOnCreateDebugCategoryPanel* PanelCreator = DebugCategoryPanelCreators.Find(InDebugCategory))
		{
			return PanelCreator->Execute(InDebugCategory).ToSharedPtr();
		}
		return nullptr;
	}

	virtual void UnregisterDebugCategoryPanel(const FString& InDebugCategory) override
	{
		DebugCategoryPanelCreators.Remove(InDebugCategory);
	}

private:

	void OnPostEngineInit()
	{
		using namespace UE::Cameras;

		SGameplayCamerasDebugger::RegisterTabSpawners();
	}

	void OnPreExit()
	{
		using namespace UE::Cameras;

		SGameplayCamerasDebugger::UnregisterTabSpawners();
	}

	void RegisterSettings()
	{
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->RegisterSettings("Editor", "Plugins", "Gameplay Cameras",
				LOCTEXT("GameplayCamerasEditorProjectSettingsName", "Gameplay Cameras"),
				LOCTEXT("GameplayCamerasEditorProjectSettingsDescription", "Configure the gameplay cameras editors."),
				GetMutableDefault<UGameplayCamerasEditorSettings>()
			);
		}
	}

	void UnregisterSettings()
	{
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->UnregisterSettings("Editor", "Plugins", "Gameplay Cameras");
		}
	}

	void RegisterCoreDebugCategories()
	{
		using namespace UE::Cameras;

		TSharedRef<FGameplayCamerasEditorStyle> GameplayCamerasEditorStyle = FGameplayCamerasEditorStyle::Get();
		const FName& GameplayCamerasEditorStyleName = GameplayCamerasEditorStyle->GetStyleSetName();

		RegisterDebugCategory(FCameraDebugCategoryInfo{
				FCameraDebugCategories::NodeTree,
				LOCTEXT("NodeTreeDebugCategory", "Node Tree"),
				LOCTEXT("NodeTreeDebugCategoryToolTip", "Shows the entire camrera node evaluator tree"),
				FSlateIcon(GameplayCamerasEditorStyleName, "DebugCategory.NodeTree.Icon")
			});
		RegisterDebugCategory(FCameraDebugCategoryInfo{
				FCameraDebugCategories::DirectorTree,
				LOCTEXT("DirectorTreeDebugCategory", "Director Tree"),
				LOCTEXT("DirectorTreeDebugCategoryToolTip", "Shows the active/inactive directors, and their evaluation context"),
				FSlateIcon(GameplayCamerasEditorStyleName, "DebugCategory.DirectorTree.Icon")
			});
		RegisterDebugCategory(FCameraDebugCategoryInfo{
				FCameraDebugCategories::BlendStacks,
				LOCTEXT("BlendStacksDebugCategory", "Blend Stacks"),
				LOCTEXT("BlendStacksDebugCategoryToolTip", "Shows a summary of the blend stacks"),
				FSlateIcon(GameplayCamerasEditorStyleName, "DebugCategory.BlendStacks.Icon")
			});
		RegisterDebugCategory(FCameraDebugCategoryInfo{
				FCameraDebugCategories::PoseStats,
				LOCTEXT("PoseStatsDebugCategory", "Pose Stats"),
				LOCTEXT("PoseStatsDebugCategoryToolTip", "Shows the evaluated camera pose"),
				FSlateIcon(GameplayCamerasEditorStyleName, "DebugCategory.PoseStats.Icon")
			});
		RegisterDebugCategory(FCameraDebugCategoryInfo{
				FCameraDebugCategories::Viewfinder,
				LOCTEXT("ViewfinderDebugCategory", "Viewfinder"),
				LOCTEXT("ViewfinderDebugCategoryToolTip", "Shows an old-school viewfinder on screen"),
				FSlateIcon(GameplayCamerasEditorStyleName, "DebugCategory.Viewfinder.Icon")
			});

		RegisterDebugCategoryPanel(FCameraDebugCategories::NodeTree, FOnCreateDebugCategoryPanel::CreateLambda([](const FString&)
					{
						return SNew(SCameraNodeTreeDebugPanel);
					}));
		RegisterDebugCategoryPanel(FCameraDebugCategories::BlendStacks, FOnCreateDebugCategoryPanel::CreateLambda([](const FString&)
					{
						return SNew(SBlendStacksDebugPanel);
					}));
	}

	void UnregisterCoreDebugCategories()
	{
		using namespace UE::Cameras;

		UnregisterDebugCategoryPanel(FCameraDebugCategories::BlendStacks);
		UnregisterDebugCategoryPanel(FCameraDebugCategories::NodeTree);
	}

	void RegisterMenus()
	{
		using namespace UE::Cameras;

		FCameraAssetEditorCommands::Register();
		FCameraRigAssetEditorCommands::Register();	
		FGameplayCamerasDebuggerCommands::Register();
	}

	void RegisterRewindDebuggerFeatures()
	{
#if UE_GAMEPLAY_CAMERAS_TRACE
		using namespace UE::Cameras;

		TraceModule = MakeShared<UE::Cameras::FCameraSystemTraceModule>();
		RewindDebuggerExtension = MakeShared<FCameraSystemRewindDebuggerExtension>();
		RewindDebuggerTrackCreator = MakeShared<FCameraSystemRewindDebuggerTrackCreator>();

		IModularFeatures& ModularFeatures = IModularFeatures::Get();
		ModularFeatures.RegisterModularFeature(IRewindDebuggerExtension::ModularFeatureName, RewindDebuggerExtension.Get());
		ModularFeatures.RegisterModularFeature(RewindDebugger::IRewindDebuggerTrackCreator::ModularFeatureName, RewindDebuggerTrackCreator.Get());
		ModularFeatures.RegisterModularFeature(TraceServices::ModuleFeatureName, TraceModule.Get());
#endif  // UE_GAMEPLAY_CAMERAS_TRACE
	}

	void UnregisterRewindDebuggerFeatures()
	{
#if UE_GAMEPLAY_CAMERAS_TRACE
		IModularFeatures& ModularFeatures = IModularFeatures::Get();
		ModularFeatures.UnregisterModularFeature(IRewindDebuggerExtension::ModularFeatureName, RewindDebuggerExtension.Get());
		ModularFeatures.UnregisterModularFeature(RewindDebugger::IRewindDebuggerTrackCreator::ModularFeatureName, RewindDebuggerTrackCreator.Get());
		ModularFeatures.UnregisterModularFeature(TraceServices::ModuleFeatureName, TraceModule.Get());
#endif  // UE_GAMEPLAY_CAMERAS_TRACE
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

	TMap<FString, UE::Cameras::FCameraDebugCategoryInfo> DebugCategoryInfos;
	TMap<FString, FOnCreateDebugCategoryPanel> DebugCategoryPanelCreators;

#if UE_GAMEPLAY_CAMERAS_TRACE
	TSharedPtr<UE::Cameras::FCameraSystemTraceModule> TraceModule;
	TSharedPtr<UE::Cameras::FCameraSystemRewindDebuggerExtension> RewindDebuggerExtension;
	TSharedPtr<UE::Cameras::FCameraSystemRewindDebuggerTrackCreator> RewindDebuggerTrackCreator;
#endif  // UE_GAMEPLAY_CAMERAS_TRACE
};

IMPLEMENT_MODULE(FGameplayCamerasEditorModule, GameplayCamerasEditor);

#undef LOCTEXT_NAMESPACE

