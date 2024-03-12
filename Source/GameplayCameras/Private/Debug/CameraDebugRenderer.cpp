// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDebugRenderer.h"

#include "Algo/Find.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Debug/CameraDebugColors.h"
#include "Debug/DebugTextRenderer.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "HAL/IConsoleManager.h"
#include "Misc/TVariant.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

int32 GGameplayCamerasDebugLeftMargin = 10;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugLeftMargin(
	TEXT("GameplayCameras.Debug.LeftMargin"),
	GGameplayCamerasDebugLeftMargin,
	TEXT("(Default: 10px. The left margin for rendering Gameplay Cameras debug text."));

int32 GGameplayCamerasDebugTopMargin = 10;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugTopMargin(
	TEXT("GameplayCameras.Debug.TopMargin"),
	GGameplayCamerasDebugTopMargin,
	TEXT("(Default: 10px. The top margin for rendering Gameplay Cameras debug text."));

int32 GGameplayCamerasDebugInnerMargin = 5;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugInnerMargin(
	TEXT("GameplayCameras.Debug.InnerMargin"),
	GGameplayCamerasDebugInnerMargin,
	TEXT("(Default: 10px. The inner margin for rendering Gameplay Cameras debug text."));

int32 GGameplayCamerasDebugIndent = 20;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugIndent(
	TEXT("GameplayCameras.Debug.Indent"),
	GGameplayCamerasDebugIndent,
	TEXT("(Default: 20px. The indent for rendering Gameplay Cameras debug text."));

int32 GGameplayCamerasDebugBackgroundDepthSortKey = 1;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugBackgroundDepthSortKey(
	TEXT("GameplayCameras.Debug.BackgroundDepthSortKey"),
	GGameplayCamerasDebugBackgroundDepthSortKey,
	TEXT(""));

FCameraDebugRenderer::FCameraDebugRenderer(FCanvas* InCanvas)
	: Canvas(InCanvas)
	, DrawColor(FColor::White)
{
	RenderFont = GEngine->GetSmallFont();
	MaxCharHeight = RenderFont->GetMaxCharHeight();

	NextDrawPosition = FVector2f{ (float)GGameplayCamerasDebugLeftMargin, (float)GGameplayCamerasDebugTopMargin };
}

FCameraDebugRenderer::~FCameraDebugRenderer()
{
	FlushText();
}

FVector2D FCameraDebugRenderer::GetCanvasSize() const
{
	FIntPoint ParentSize = Canvas->GetParentCanvasSize();
	return FVector2D(ParentSize.X, ParentSize.Y);
}

void FCameraDebugRenderer::AddText(const FString& InString)
{
	AddTextImpl(*InString);
}

void FCameraDebugRenderer::AddText(const TCHAR* Fmt, ...)
{
	va_list Args;
	va_start(Args, Fmt);
	AddTextFmtImpl(Fmt, Args);
	va_end(Args);
}

void FCameraDebugRenderer::AddTextFmtImpl(const TCHAR* Fmt, va_list Args)
{
	Formatter.Reset();
	Formatter.AppendV(Fmt, Args);
	const TCHAR* Message = Formatter.ToString();
	AddTextImpl(Message);
}

void FCameraDebugRenderer::AddTextImpl(const TCHAR* Buffer)
{
	LineBuilder.Append(Buffer);
}

bool FCameraDebugRenderer::NewLine(bool bSkipIfEmptyLine)
{
	FlushText();

	const float IndentMargin = GetIndentMargin();
	const bool bIsLineEmpty = FMath::IsNearlyEqual(NextDrawPosition.X, IndentMargin);
	if (!bIsLineEmpty || !bSkipIfEmptyLine)
	{
		NextDrawPosition.X = IndentMargin;
		NextDrawPosition.Y += MaxCharHeight;
		return true;
	}
	return false;
}

FColor FCameraDebugRenderer::GetTextColor() const
{
	return DrawColor;
}

FColor FCameraDebugRenderer::SetTextColor(const FColor& Color)
{
	FlushText();
	FColor ReturnColor = DrawColor;
	DrawColor = Color;
	return ReturnColor;
}

float FCameraDebugRenderer::GetIndentMargin() const
{
	return (float)(GGameplayCamerasDebugLeftMargin + IndentLevel * GGameplayCamerasDebugIndent);
}

void FCameraDebugRenderer::FlushText()
{
	if (LineBuilder.Len() > 0)
	{
		int32 ViewHeight = GetCanvasSize().Y;
		if (NextDrawPosition.Y < ViewHeight)
		{
			FDebugTextRenderer TextRenderer(Canvas, DrawColor, RenderFont);
			TextRenderer.LeftMargin = GetIndentMargin();
			TextRenderer.RenderText(NextDrawPosition, LineBuilder.ToView());

			NextDrawPosition = TextRenderer.GetEndDrawPosition();
			RightMargin = FMath::Max(RightMargin, TextRenderer.GetRightMargin());
		}
		// else: text is going off-screen.

		LineBuilder.Reset();
	}
}

void FCameraDebugRenderer::AddIndent()
{
	// Flush any remaining text we have on the current indent level and move
	// to a new line, unless the current line was empty.
	NewLine(true);

	++IndentLevel;

	// The next draw position is at the beginning of a new line (or the beginning
	// of an old line if it was empty). Either way, it's left at the previous
	// indent level, so we need to bump it to the right.
	NextDrawPosition.X = GetIndentMargin();
}

void FCameraDebugRenderer::RemoveIndent()
{
	// Flush any remaining text we have on the current indent level and move
	// to a new line, unless the current line was empty.
	NewLine(true);

	if (ensureMsgf(IndentLevel > 0, TEXT("Can't go into negative indenting!")))
	{
		--IndentLevel;

		// See comment in AddIndent().
		NextDrawPosition.X = GetIndentMargin();
	}
}

void FCameraDebugRenderer::DrawTextBackgroundTile(float Opacity)
{
	const float IndentMargin = GetIndentMargin();
	const bool bIsLineEmpty = FMath::IsNearlyEqual(NextDrawPosition.X, IndentMargin);
	const float TextBottom = bIsLineEmpty ? NextDrawPosition.Y : NextDrawPosition.Y + MaxCharHeight;

	const float InnerMargin = GGameplayCamerasDebugInnerMargin;
	const FVector2D TopLeft(GGameplayCamerasDebugLeftMargin - InnerMargin, GGameplayCamerasDebugTopMargin - InnerMargin);
	const FVector2D BottomRight(RightMargin + InnerMargin, TextBottom + InnerMargin);
	const FVector2D TileSize(BottomRight.X - TopLeft.X, BottomRight.Y - TopLeft.Y);

	const FColor BackgroundColor = FCameraDebugColors::Get().Background.WithAlpha((uint8)(Opacity * 255));

	// Draw the background behind the text.
	if (Canvas)
	{
		Canvas->PushDepthSortKey(GGameplayCamerasDebugBackgroundDepthSortKey);
		{
			FCanvasTileItem BackgroundTile(TopLeft, TileSize, BackgroundColor);
			BackgroundTile.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(BackgroundTile);
		}
		Canvas->PopDepthSortKey();
	}
}

void FCameraDebugRenderer::SkipAttachedBlocks()
{
	VisitFlags |= ECameraDebugDrawVisitFlags::SkipAttachedBlocks;
}

void FCameraDebugRenderer::SkipChildrenBlocks()
{
	VisitFlags |= ECameraDebugDrawVisitFlags::SkipChildrenBlocks;
}

void FCameraDebugRenderer::SkipAllBlocks()
{
	VisitFlags |= (ECameraDebugDrawVisitFlags::SkipAttachedBlocks | ECameraDebugDrawVisitFlags::SkipChildrenBlocks);
}

ECameraDebugDrawVisitFlags FCameraDebugRenderer::GetVisitFlags() const
{
	return VisitFlags;
}

void FCameraDebugRenderer::ResetVisitFlags()
{
	VisitFlags = ECameraDebugDrawVisitFlags::None;
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

