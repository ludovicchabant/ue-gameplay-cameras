// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Widgets/SCompoundWidget.h"

#include "SGameplayCamerasDebugger.generated.h"

class FSpawnTabArgs;
class FTabManager;
class FUICommandList;
class SBox;
class SDockTab;
class SWidget;
class UToolMenu;
struct FSlateIcon;

namespace UE::Cameras
{

class SGameplayCamerasDebugger : public SCompoundWidget
{
public:

	static const FName WindowName;
	static const FName MenubarName;
	static const FName ToolbarName;

	static void RegisterTabSpawners();
	static TSharedRef<SDockTab> SpawnGameplayCamerasDebugger(const FSpawnTabArgs& Args);
	static void UnregisterTabSpawners();

public:

	SLATE_BEGIN_ARGS(SGameplayCamerasDebugger) {}
	SLATE_END_ARGS();

	SGameplayCamerasDebugger();
	virtual ~SGameplayCamerasDebugger();

	void Construct(const FArguments& InArgs);

protected:

	static SGameplayCamerasDebugger* FromContext(UToolMenu* InMenu);
	TSharedRef<SWidget> ConstructMenubar();
	TSharedRef<SWidget> ConstructToolbar(TSharedRef<FUICommandList> InCommandList);
	TSharedRef<SWidget> ConstructGeneralOptions(TSharedRef<FUICommandList> InCommandList);
	void ConstructDebugPanels();

	void InitializeColorSchemeNames();

	static bool IsDebugCategoryActive(FString InCategoryName);
	void SetActiveDebugCategoryPanel(FString InCategoryName);

	FText GetToggleDebugDrawText() const;
	FSlateIcon GetToggleDebugDrawIcon() const;

private:

	FName GameplayCamerasEditorStyleName;

	TSharedPtr<SBox> PanelHost;

	TSharedPtr<SWidget> EmptyPanel;
	TMap<FString, TSharedPtr<SWidget>> DebugPanels;

	TArray<TSharedPtr<FString>> ColorSchemeNames;
};

}  // namespace UE::Cameras

UCLASS()
class UGameplayCamerasDebuggerMenuContext : public UObject
{
	GENERATED_BODY()

public:

	TWeakPtr<UE::Cameras::SGameplayCamerasDebugger> CamerasDebugger;
};

