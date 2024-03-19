// Copyright Epic Games, Inc. All Rights Reserved.

#include "Styles/GameplayCamerasEditorStyle.h"

#include "Brushes/SlateImageBrush.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateTypes.h"
#include "Styling/StyleColors.h"

namespace UE::Cameras
{

TSharedPtr<FGameplayCamerasEditorStyle> FGameplayCamerasEditorStyle::Singleton;

FGameplayCamerasEditorStyle::FGameplayCamerasEditorStyle()
	: FSlateStyleSet("GameplayCamerasEditorStyle")
{
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon24x24(24.0f, 24.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon48x48(48.0f, 48.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);

	const FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("GameplayCameras"))->GetContentDir();
	SetContentRoot(ContentDir);
	SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	// Camera rig editor icons.
	Set("CameraRigAssetEditor.Tabs.Toolbox", new IMAGE_BRUSH_SVG("Icons/CameraRig-Toolbox", Icon16x16));
	Set("CameraRigAssetEditor.Tabs.Search", new CORE_IMAGE_BRUSH_SVG("Starship/Common/search", Icon16x16));
	Set("CameraRigAssetEditor.Tabs.Messages", new CORE_IMAGE_BRUSH_SVG("Starship/Common/OutputLog", Icon16x16));
	Set("CameraRigAssetEditor.Tabs.NodeHierarchy", new IMAGE_BRUSH_SVG("Icons/CameraRig-NodeHierarchy", Icon16x16));
	Set("CameraRigAssetEditor.Tabs.Transitions", new IMAGE_BRUSH_SVG("Icons/CameraRig-Transitions", Icon16x16));

	Set("CameraRigAssetEditor.Build", new IMAGE_BRUSH_SVG("Icons/CameraRig-BuildStatus_Background", Icon20x20));
	Set("CameraRigAssetEditor.BuildStatus.Background", new IMAGE_BRUSH_SVG("Icons/CameraRig-BuildStatus_Background", Icon20x20));
	Set("CameraRigAssetEditor.BuildStatus.Overlay.Fail", new IMAGE_BRUSH_SVG("Icons/CameraRig-BuildStatus_Fail_Badge", Icon20x20, FStyleColors::Error));
	Set("CameraRigAssetEditor.BuildStatus.Overlay.Good", new IMAGE_BRUSH_SVG("Icons/CameraRig-BuildStatus_Good_Badge", Icon20x20, FStyleColors::AccentGreen));
	Set("CameraRigAssetEditor.BuildStatus.Overlay.Unknown", new IMAGE_BRUSH_SVG("Icons/CameraRig-BuildStatus_Unknown_Badge", Icon20x20, FStyleColors::AccentYellow));
	Set("CameraRigAssetEditor.BuildStatus.Overlay.Warning", new IMAGE_BRUSH_SVG("Icons/CameraRig-BuildStatus_Warning_Badge", Icon20x20, FStyleColors::Warning));

	Set("CameraRigAssetEditor.ShowNodeHierarchy", new IMAGE_BRUSH_SVG("Icons/CameraRig-NodeHierarchy", Icon20x20));
	Set("CameraRigAssetEditor.ShowTransitions", new IMAGE_BRUSH_SVG("Icons/CameraRig-Transitions", Icon20x20));
	Set("CameraRigAssetEditor.FocusHome", new IMAGE_BRUSH_SVG("Icons/GraphEditor-Home", Icon20x20));

	Set("CameraRigAssetEditor.LiveUpdate", new IMAGE_BRUSH_SVG("Icons/CameraRig-LiveUpdate", Icon20x20));
	Set("CameraRigAssetEditor.Apply", new IMAGE_BRUSH_SVG("CameraEditor-Apply", Icon20x20));
	Set("CameraRigAssetEditor.FindInCameraRig", new CORE_IMAGE_BRUSH_SVG("Starship/Common/Search", Icon20x20));

	// Debugger tool icons.
	Set("Debugger.EnableDebugInfo.Icon", new IMAGE_BRUSH("Icons/EnableDebugInfo", Icon16x16));
	Set("Debugger.DisableDebugInfo.Icon", new IMAGE_BRUSH("Icons/DisableDebugInfo", Icon16x16));

	Set("DebugCategory.NodeTree.Icon", new IMAGE_BRUSH("Icons/DebugCategory-NodeTree", Icon16x16));
	Set("DebugCategory.DirectorTree.Icon", new IMAGE_BRUSH("Icons/DebugCategory-DirectorTree", Icon16x16));
	Set("DebugCategory.BlendStacks.Icon", new IMAGE_BRUSH("Icons/DebugCategory-BlendStacks", Icon16x16));
	Set("DebugCategory.PoseStats.Icon", new IMAGE_BRUSH("Icons/DebugCategory-PoseStats", Icon16x16));
	Set("DebugCategory.Viewfinder.Icon", new IMAGE_BRUSH("Icons/DebugCategory-Viewfinder", Icon16x16));

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

}  // namespace UE::Cameras

