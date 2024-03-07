// Copyright Epic Games, Inc. All Rights Reserved.

#include "Styles/GameplayCamerasEditorStyle.h"

#include "Brushes/SlateImageBrush.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Styling/SlateStyleRegistry.h"

namespace UE::Cameras
{

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

TSharedPtr<FGameplayCamerasEditorStyle> FGameplayCamerasEditorStyle::Singleton;

FGameplayCamerasEditorStyle::FGameplayCamerasEditorStyle()
	: FSlateStyleSet("GameplayCamerasEditorStyle")
{
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon24x24(24.0f, 24.0f);
	const FVector2D Icon48x48(48.0f, 48.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);

	const FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("GameplayCameras"))->GetContentDir();
	SetContentRoot(ContentDir);
	SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	Set("Debugger.EnableDebugInfo.Icon", new IMAGE_BRUSH("/Icons/EnableDebugInfo", Icon16x16));

	Set("DebugCategory.NodeTree.Icon", new IMAGE_BRUSH("/Icons/DebugCategory-NodeTree", Icon16x16));
	Set("DebugCategory.DirectorTree.Icon", new IMAGE_BRUSH("/Icons/DebugCategory-DirectorTree", Icon16x16));
	Set("DebugCategory.BlendStacks.Icon", new IMAGE_BRUSH("/Icons/DebugCategory-BlendStacks", Icon16x16));
	Set("DebugCategory.PoseStats.Icon", new IMAGE_BRUSH("/Icons/DebugCategory-PoseStats", Icon16x16));
	Set("DebugCategory.Viewfinder.Icon", new IMAGE_BRUSH("/Icons/DebugCategory-Viewfinder", Icon16x16));

	FSlateStyleRegistry::RegisterSlateStyle(*this);
}

FGameplayCamerasEditorStyle::~FGameplayCamerasEditorStyle()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*this);
}

TSharedRef<FGameplayCamerasEditorStyle> FGameplayCamerasEditorStyle::Get()
{
	if (!Singleton.IsValid())
	{
		Singleton = MakeShareable(new FGameplayCamerasEditorStyle);
	}
	return Singleton.ToSharedRef();
}

#undef IMAGE_BRUSH

}  // namespace UE::Cameras

