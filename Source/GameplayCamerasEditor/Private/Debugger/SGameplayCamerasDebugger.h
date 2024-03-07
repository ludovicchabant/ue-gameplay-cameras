// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Widgets/SCompoundWidget.h"

class FTabManager;
class FSpawnTabArgs;
class SBox;
class SDockTab;

namespace UE::Cameras
{

class SGameplayCamerasDebugger : public SCompoundWidget
{
public:

	static const FName WindowName;
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

	static bool IsDebugCategoryActive(const FString& InCategoryName);
	void SetActiveDebugCategoryPanel(const FString& InCategoryName);

private:

	TSharedPtr<SBox> PanelHost;
	TSharedPtr<SWidget> EmptyPanel;
	TMap<FString, TSharedPtr<SWidget>> DebugPanels;
};

}  // namespace UE::Cameras

