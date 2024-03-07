// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/ViewfinderDebugBlock.h"

#include "Debug/CameraDebugCategories.h"
#include "Debug/CameraDebugColors.h"
#include "Debug/CameraDebugRenderer.h"
#include "DrawDebugHelpers.h"
#include "Engine/Canvas.h"
#include "HAL/IConsoleManager.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

float GGameplayCamerasDebugViewfinderReticleSizeFactor = 0.27f;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugViewfinderReticleSizeFactor(
	TEXT("GameplayCameras.Debug.Viewfinder.ReticleSizeFactor"),
	GGameplayCamerasDebugViewfinderReticleSizeFactor,
	TEXT("Default: 0.1. The size of the viewfinder reticle, as a factor of the screen's vertical size."));

float GGameplayCamerasDebugViewfinderReticleInnerSizeFactor = 0.7f;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugViewfinderReticleInnerSizeFactor(
	TEXT("GameplayCameras.Debug.Viewfinder.ReticleInnerSizeFactor"),
	GGameplayCamerasDebugViewfinderReticleInnerSizeFactor,
	TEXT(""));

int32 GGameplayCamerasDebugViewfinderReticleNumSides = 60;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugViewfinderReticleNumSides(
	TEXT("GameplayCameras.Debug.Viewfinder.ReticleNumSides"),
	GGameplayCamerasDebugViewfinderReticleNumSides,
	TEXT(""));

float GGameplayCamerasDebugViewfinderGuidesGapFactor = 0.02f;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugViewfinderGuidesGapFactor(
	TEXT("GameplayCameras.Debug.Viewfinder.GuidesGapFactor"),
	GGameplayCamerasDebugViewfinderGuidesGapFactor,
	TEXT(""));

UE_DEFINE_CAMERA_DEBUG_BLOCK(FViewfinderDebugBlock)

FViewfinderDebugBlock::FViewfinderDebugBlock()
{
}

void FViewfinderDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	if (!Params.IsCategoryActive(FCameraDebugCategories::Viewfinder))
	{
		return;
	}

	UCanvas* Canvas = Renderer.GetCanvas();

	const FVector2d CanvasCenter(Canvas->SizeX / 2.f, Canvas->SizeY / 2.f);

	// Draw the reticle.
	const float ReticleRadius = Canvas->SizeY * GGameplayCamerasDebugViewfinderReticleSizeFactor / 2.f;
	const float ReticleInnerRadiusFactor = GGameplayCamerasDebugViewfinderReticleInnerSizeFactor;
	const int32 ReticleNumSides = GGameplayCamerasDebugViewfinderReticleNumSides;
	const FColor ReticleColor = FCameraDebugColors::Get().Passive;

	// ...outer reticle circle.
	DrawDebugCanvas2DCircle(Canvas, CanvasCenter, ReticleRadius, ReticleNumSides, ReticleColor);
	// ...inner reticle circle.
	const float ReticleInnerRadius = ReticleRadius * ReticleInnerRadiusFactor;
	const int32 ReticleInnerNumSides = (int32)(ReticleNumSides * ReticleInnerRadiusFactor);
	DrawDebugCanvas2DCircle(Canvas, CanvasCenter, ReticleInnerRadius, ReticleInnerNumSides, ReticleColor);
	// ...horizontal line inside reticle.
	DrawDebugCanvas2DLine(
			Canvas, 
			CanvasCenter - FVector2D(ReticleInnerRadius, 0), CanvasCenter + FVector2D(ReticleInnerRadius, 0), 
			ReticleColor);

	// Draw the rule-of-thirds guides.
	const FColor GuideColor = FCameraDebugColors::Get().VeryPassive;
	const float GuidesGap = Canvas->SizeY * GGameplayCamerasDebugViewfinderGuidesGapFactor;
	const FVector2D OneThird(Canvas->SizeX / 3.f, Canvas->SizeY / 3.f);
	const FVector2D TwoThirds(Canvas->SizeX / 1.5f, Canvas->SizeY / 1.5f);
	// ...top vertical guides
	DrawDebugCanvas2DLine(
			Canvas,
			FVector2D(OneThird.X, 0), FVector2D(OneThird.X, OneThird.Y - GuidesGap),
			ReticleColor, 2.f);
	DrawDebugCanvas2DLine(
			Canvas,
			FVector2D(TwoThirds.X, 0), FVector2D(TwoThirds.X, OneThird.Y - GuidesGap),
			ReticleColor, 2.f);
	// ...bottom vertical guides
	DrawDebugCanvas2DLine(
			Canvas,
			FVector2D(OneThird.X, TwoThirds.Y + GuidesGap), FVector2D(OneThird.X, Canvas->SizeY),
			ReticleColor, 2.f);
	DrawDebugCanvas2DLine(
			Canvas,
			FVector2D(TwoThirds.X, TwoThirds.Y + GuidesGap), FVector2D(TwoThirds.X, Canvas->SizeY),
			ReticleColor, 2.f);
	// ...left horizontal guides
	DrawDebugCanvas2DLine(
			Canvas,
			FVector2D(0, OneThird.Y), FVector2D(OneThird.X - GuidesGap, OneThird.Y),
			ReticleColor, 2.f);
	DrawDebugCanvas2DLine(
			Canvas,
			FVector2D(0, TwoThirds.Y), FVector2D(OneThird.X - GuidesGap, TwoThirds.Y),
			ReticleColor, 2.f);
	// ...right horizontal guides
	DrawDebugCanvas2DLine(
			Canvas,
			FVector2D(TwoThirds.X + GuidesGap, OneThird.Y), FVector2D(Canvas->SizeX, OneThird.Y),
			ReticleColor, 2.f);
	DrawDebugCanvas2DLine(
			Canvas,
			FVector2D(TwoThirds.X + GuidesGap, TwoThirds.Y), FVector2D(Canvas->SizeX, TwoThirds.Y),
			ReticleColor, 2.f);
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

