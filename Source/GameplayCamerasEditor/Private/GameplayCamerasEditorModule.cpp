// Copyright Epic Games, Inc. All Rights Reserved.

#include "IGameplayCamerasEditorModule.h"

#include "AssetTools/CameraAssetEditor.h"
#include "AssetTools/CameraRigAssetEditor.h"
#include "AssetTools/CameraRigProxyAssetEditor.h"
#include "AssetTools/CameraVariableCollectionEditor.h"
#include "Builders/BlueprintCameraDirectorEditorBuilder.h"
#include "Commands/CameraAssetEditorCommands.h"
#include "Commands/CameraRigAssetEditorCommands.h"
#include "Commands/CameraRigTransitionEditorCommands.h"
#include "Commands/CameraVariableCollectionEditorCommands.h"
#include "Commands/GameplayCamerasDebuggerCommands.h"
#include "Customizations/CameraParameterDetailsCustomizations.h"
#include "Customizations/CameraRigAssetReferenceDetailsCustomization.h"
#include "Customizations/CameraRigPtrDetailsCustomization.h"
#include "Customizations/CameraProxyTableDetailsCustomization.h"
#include "Customizations/CameraVariableReferenceDetailsCustomizations.h"
#include "Customizations/FilmbackCameraNodeDetailsCustomization.h"
#include "Debug/CameraDebugCategories.h"
#include "Debugger/SBlendStacksDebugPanel.h"
#include "Debugger/SCameraNodeTreeDebugPanel.h"
#include "Debugger/SEvaluationServicesDebugPanel.h"
#include "Debugger/SCameraPoseStatsDebugPanel.h"
#include "Debugger/SGameplayCamerasDebugger.h"
#include "Directors/BlueprintCameraDirector.h"
#include "EdGraph/EdGraph.h"
#include "Editors/GameplayCamerasGraphPanelPinFactory.h"
#include "Editors/SCameraRigPicker.h"
#include "Editors/SCameraVariablePicker.h"
#include "Features/IModularFeatures.h"
#include "GameplayCameras.h"
#include "GameplayCamerasEditorSettings.h"
#include "GameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasEditorModule.h"
#include "IGameplayCamerasModule.h"
#include "IRewindDebuggerExtension.h"
#include "ISettingsModule.h"
#include "K2Node_Event.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "ToolMenus.h"
#include "Toolkits/BlueprintCameraDirectorAssetEditorMode.h"
#include "Toolkits/CameraAssetEditorToolkit.h"
#include "Toolkits/CameraRigAssetEditorToolkit.h"
#include "Toolkits/SingleCameraDirectorAssetEditorMode.h"
#include "Trace/CameraSystemRewindDebuggerExtension.h"
#include "Trace/CameraSystemRewindDebuggerTrack.h"
#include "Trace/CameraSystemTraceModule.h"

#define LOCTEXT_NAMESPACE "GameplayCamerasEditor"

DEFINE_LOG_CATEGORY(LogCameraSystemEditor);

const FName IGameplayCamerasEditorModule::GameplayCamerasEditorAppIdentifier("GameplayCamerasEditorApp");
const FName IGameplayCamerasEditorModule::CameraRigAssetEditorToolBarName("CameraRigAssetEditor.ToolBar");

IGameplayCamerasEditorModule& IGameplayCamerasEditorModule::Get()
{
	return FModuleManager::LoadModuleChecked<IGameplayCamerasEditorModule>("GameplayCamerasEditor");
}

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
		RegisterCameraDirectorEditors();
		RegisterBuilders();
		RegisterCoreDebugCategories();
		RegisterRewindDebuggerFeatures();
		RegisterDetailsCustomizations();
		RegisterEdGraphUtilities();

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
		FCameraRigTransitionEditorCommands::Unregister();
		FCameraVariableCollectionEditorCommands::Unregister();
		FGameplayCamerasDebuggerCommands::Unregister();

		UnregisterSettings();
		UnregisterCameraDirectorEditors();
		UnregisterBuilders();
		UnregisterCoreDebugCategories();
		UnregisterRewindDebuggerFeatures();
		UnregisterDetailsCustomizations();
		UnregisterEdGraphUtilities();

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

	virtual UCameraRigProxyAssetEditor* CreateCameraRigProxyEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCameraRigProxyAsset* CameraRigProxy) override
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		UCameraRigProxyAssetEditor* AssetEditor = NewObject<UCameraRigProxyAssetEditor>(AssetEditorSubsystem, NAME_None, RF_Transient);
		AssetEditor->Initialize(CameraRigProxy);
		return AssetEditor;
	}

	virtual UCameraVariableCollectionEditor* CreateCameraVariableCollectionEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCameraVariableCollection* VariableCollection) override
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		UCameraVariableCollectionEditor* AssetEditor = NewObject<UCameraVariableCollectionEditor>(AssetEditorSubsystem, NAME_None, RF_Transient);
		AssetEditor->Initialize(VariableCollection);
		return AssetEditor;
	}

	virtual TSharedRef<SWidget> CreateCameraRigPicker(const FCameraRigPickerConfig& InPickerConfig) override
	{
		using namespace UE::Cameras;

		return SNew(SCameraRigPicker)
			.CameraRigPickerConfig(InPickerConfig);
	}

	virtual TSharedRef<SWidget> CreateCameraVariablePicker(const FCameraVariablePickerConfig& InPickerConfig) override
	{
		using namespace UE::Cameras;

		return SNew(SCameraVariablePicker)
			.CameraVariablePickerConfig(InPickerConfig);
	}

	virtual FDelegateHandle RegisterCameraDirectorEditor(FOnCreateCameraDirectorAssetEditorMode InOnCreateEditor) override
	{
		CameraDirectorEditorCreators.Add(InOnCreateEditor);
		return CameraDirectorEditorCreators.Last().GetHandle();
	}

	virtual TArrayView<const FOnCreateCameraDirectorAssetEditorMode> GetCameraDirectorEditorCreators() const override
	{
		return CameraDirectorEditorCreators;
	}

	virtual void UnregisterCameraDirectorEditor(FDelegateHandle InHandle) override
	{
		CameraDirectorEditorCreators.RemoveAll(
				[=](const FOnCreateCameraDirectorAssetEditorMode& Delegate) 
				{
					return Delegate.GetHandle() == InHandle; 
				});
	}

	virtual FDelegateHandle RegisterCameraAssetBuilder(FOnBuildCameraAsset InOnBuildCameraAsset) override
	{
		CameraAssetBuilders.Add(InOnBuildCameraAsset);
		return CameraAssetBuilders.Last().GetHandle();
	}

	virtual TArrayView<const FOnBuildCameraAsset> GetCameraAssetBuilders() const override
	{
		return CameraAssetBuilders;
	}

	virtual void UnregisterCameraAssetBuilder(FDelegateHandle InHandle) override
	{
		CameraAssetBuilders.RemoveAll(
				[=](const FOnBuildCameraAsset& Delegate)
				{
					return Delegate.GetHandle() == InHandle;
				});
	}

	virtual FDelegateHandle RegisterCameraRigAssetBuilder(FOnBuildCameraRigAsset InOnBuildCameraRigAsset) override
	{
		CameraRigAssetBuilders.Add(InOnBuildCameraRigAsset);
		return CameraRigAssetBuilders.Last().GetHandle();
	}
	
	virtual TArrayView<const FOnBuildCameraRigAsset> GetCameraRigAssetBuilders() const override
	{
		return CameraRigAssetBuilders;
	}

	virtual void UnregisterCameraRigAssetBuilder(FDelegateHandle InHandle) override
	{
		CameraRigAssetBuilders.RemoveAll(
				[=](const FOnBuildCameraRigAsset& Delegate)
				{
					return Delegate.GetHandle() == InHandle;
				});
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

	void RegisterCameraDirectorEditors()
	{
		using namespace UE::Cameras;

		BuiltInDirectorCreatorHandles.Add(
				RegisterCameraDirectorEditor(FOnCreateCameraDirectorAssetEditorMode::CreateStatic(
						&FSingleCameraDirectorAssetEditorMode::CreateInstance)));
		BuiltInDirectorCreatorHandles.Add(
				RegisterCameraDirectorEditor(FOnCreateCameraDirectorAssetEditorMode::CreateStatic(
						&FBlueprintCameraDirectorAssetEditorMode::CreateInstance)));
	}

	void UnregisterCameraDirectorEditors()
	{
		for (FDelegateHandle Handle : BuiltInDirectorCreatorHandles)
		{
			UnregisterCameraDirectorEditor(Handle);
		}
		BuiltInDirectorCreatorHandles.Reset();
	}

	void RegisterBuilders()
	{
		using namespace UE::Cameras;

		BuiltInCameraAssetBuilders.Add(
				RegisterCameraAssetBuilder(FOnBuildCameraAsset::CreateStatic(
						&FBlueprintCameraDirectorEditorBuilder::OnBuildCameraAsset)));
	}

	void UnregisterBuilders()
	{
		for (FDelegateHandle Handle : BuiltInCameraAssetBuilders)
		{
			UnregisterCameraAssetBuilder(Handle);
		}
		BuiltInCameraAssetBuilders.Reset();

		for (FDelegateHandle Handle : BuiltInCameraRigAssetBuilders)
		{
			UnregisterCameraRigAssetBuilder(Handle);
		}
		BuiltInCameraRigAssetBuilders.Reset();
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
				FCameraDebugCategories::Services,
				LOCTEXT("ServicesDebugCategory", "Services"),
				LOCTEXT("ServicesDebugCategoryToolTip", "Shows the debug information from evaluation services"),
				FSlateIcon(GameplayCamerasEditorStyleName, "DebugCategory.Services.Icon")
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
		RegisterDebugCategoryPanel(FCameraDebugCategories::Services, FOnCreateDebugCategoryPanel::CreateLambda([](const FString&)
					{
						return SNew(SEvaluationServicesDebugPanel);
					}));
		RegisterDebugCategoryPanel(FCameraDebugCategories::PoseStats, FOnCreateDebugCategoryPanel::CreateLambda([](const FString&)
					{
						return SNew(SCameraPoseStatsDebugPanel);
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
		FCameraRigTransitionEditorCommands::Register();
		FCameraVariableCollectionEditorCommands::Register();
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

	void RegisterDetailsCustomizations()
	{
		using namespace UE::Cameras;

		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		FCameraParameterDetailsCustomization::Register(PropertyEditorModule);
		FCameraVariableReferenceDetailsCustomization::Register(PropertyEditorModule);

		PropertyEditorModule.RegisterCustomPropertyTypeLayout("CameraRigProxyTableEntry", FOnGetPropertyTypeCustomizationInstance::CreateStatic(
					&FCameraProxyTableEntryDetailsCustomization::MakeInstance));
		PropertyEditorModule.RegisterCustomPropertyTypeLayout("CameraRigAsset", FOnGetPropertyTypeCustomizationInstance::CreateStatic(
					&FCameraRigPtrDetailsCustomization::MakeInstance));
		PropertyEditorModule.RegisterCustomPropertyTypeLayout("CameraRigAssetReference", FOnGetPropertyTypeCustomizationInstance::CreateStatic(
					&FCameraRigAssetReferenceDetailsCustomization::MakeInstance));

		PropertyEditorModule.RegisterCustomClassLayout("FilmbackCameraNode", FOnGetDetailCustomizationInstance::CreateStatic(
					&FFilmbackCameraNodeDetailsCustomization::MakeInstance));
	}

	void UnregisterDetailsCustomizations()
	{
		using namespace UE::Cameras;

		FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");

		if (PropertyEditorModule)
		{
			FCameraParameterDetailsCustomization::Unregister(*PropertyEditorModule);
			FCameraVariableReferenceDetailsCustomization::Unregister(*PropertyEditorModule);

			PropertyEditorModule->UnregisterCustomPropertyTypeLayout("CameraRigProxyTableEntry");
			PropertyEditorModule->UnregisterCustomPropertyTypeLayout("CameraRigAsset");
			PropertyEditorModule->UnregisterCustomPropertyTypeLayout("CameraRigAssetReference");

			PropertyEditorModule->UnregisterCustomClassLayout("FilmbackCameraNode");
		}
	}

	void RegisterEdGraphUtilities()
	{
		using namespace UE::Cameras;

		GraphPanelPinFactory = MakeShared<FGameplayCamerasGraphPanelPinFactory>();
		FEdGraphUtilities::RegisterVisualPinFactory(GraphPanelPinFactory);

		FKismetEditorUtilities::RegisterAutoGeneratedDefaultEvent(
				this,
				UBlueprintCameraDirectorEvaluator::StaticClass(),
				GET_FUNCTION_NAME_CHECKED(UBlueprintCameraDirectorEvaluator, RunCameraDirector));
		FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback(
				this, 
				UBlueprintCameraDirectorEvaluator::StaticClass(),
				FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(
					this, &FGameplayCamerasEditorModule::OnNewBlueprintCameraDirectorEvaluatorCreated));
	}

	void OnNewBlueprintCameraDirectorEvaluatorCreated(UBlueprint* InBlueprint)
	{
		if (InBlueprint->BlueprintType != BPTYPE_Normal)
		{
			return;
		}
		TObjectPtr<UEdGraph>* FoundItem = InBlueprint->UbergraphPages.FindByPredicate(
					[](UEdGraph* Item) { return Item->GetFName() == TEXT("EventGraph"); });
		if (!FoundItem)
		{
			return;
		}

		TObjectPtr<UEdGraph> EventGraph(*FoundItem);

		TArray<UK2Node_Event*> EventNodes;
		EventGraph->GetNodesOfClass(EventNodes);
		if (EventNodes.IsEmpty())
		{
			return;
		}

		const FName EventName = GET_FUNCTION_NAME_CHECKED(UBlueprintCameraDirectorEvaluator, RunCameraDirector);
		UK2Node_Event** FoundEventNode = EventNodes.FindByPredicate(
				[&EventName](UK2Node_Event* Item) 
				{
					return Item->EventReference.GetMemberName() == EventName;
				});
		if (!FoundEventNode)
		{
			return;
		}

		UK2Node_Event* RunEventNode(*FoundEventNode);
	
		const FText RunEventNodeCommentText = LOCTEXT(
				"BlueprintCameraDirector_RunEventComment",
				"Implement your camera director logic starting from here.\n"
				"This node is currently disabled, but start dragging off pins to enable it.\n"
				"Call ActivateCameraRig at least once to declare which camera rig(s) should be active this frame.");
		RunEventNode->NodeComment = RunEventNodeCommentText.ToString();
		RunEventNode->bCommentBubbleVisible = true;
	}

	void UnregisterEdGraphUtilities()
	{
		if (GraphPanelPinFactory)
		{
			FEdGraphUtilities::UnregisterVisualPinFactory(GraphPanelPinFactory);
		}

		FKismetEditorUtilities::UnregisterAutoBlueprintNodeCreation(this);
	}

	void InitializeLiveEditManager()
	{
		using namespace UE::Cameras;

		LiveEditManager = MakeShared<FGameplayCamerasLiveEditManager>();

		IGameplayCamerasModule& CamerasModule = FModuleManager::LoadModuleChecked<IGameplayCamerasModule>("GameplayCameras");
		CamerasModule.SetLiveEditManager(LiveEditManager);
	}

	void TeardownLiveEditManager()
	{
		FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);

		IGameplayCamerasModule& CamerasModule = FModuleManager::LoadModuleChecked<IGameplayCamerasModule>("GameplayCameras");
		CamerasModule.SetLiveEditManager(nullptr);

		LiveEditManager.Reset();
	}
	
private:

	TSharedPtr<UE::Cameras::FGameplayCamerasLiveEditManager> LiveEditManager;

	TArray<FOnCreateCameraDirectorAssetEditorMode> CameraDirectorEditorCreators;
	TArray<FDelegateHandle> BuiltInDirectorCreatorHandles;

	TArray<FOnBuildCameraAsset> CameraAssetBuilders;
	TArray<FDelegateHandle> BuiltInCameraAssetBuilders;

	TArray<FOnBuildCameraRigAsset> CameraRigAssetBuilders;
	TArray<FDelegateHandle> BuiltInCameraRigAssetBuilders;

	TSharedPtr<UE::Cameras::FGameplayCamerasGraphPanelPinFactory> GraphPanelPinFactory;

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

