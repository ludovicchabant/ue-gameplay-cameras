// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Widgets/SCompoundWidget.h"

class FTabManager;
class FSpawnTabArgs;
class SBox;
class SDockTab;
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

	static bool IsDebugCategoryActive(FString InCategoryName);
	void SetActiveDebugCategoryPanel(FString InCategoryName);

	FText GetToggleDebugDrawText() const;
	FSlateIcon GetToggleDebugDrawIcon() const;

private:

	FName GameplayCamerasEditorStyleName;

	TSharedPtr<SBox> PanelHost;
	TSharedPtr<SWidget> EmptyPanel;
	TMap<FString, TSharedPtr<SWidget>> DebugPanels;
};

}  // namespace UE::Cameras

