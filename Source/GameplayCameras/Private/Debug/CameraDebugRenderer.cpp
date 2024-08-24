// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDebugRenderer.h"

#include "Algo/Find.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Components/LineBatchComponent.h"
#include "Debug/CameraDebugClock.h"
#include "Debug/CameraDebugColors.h"
#include "Debug/DebugTextRenderer.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Math/Box2D.h"
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

int32 GGameplayCamerasDebugRightMargin = 10;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugRightMargin(
	TEXT("GameplayCameras.Debug.RightMargin"),
	GGameplayCamerasDebugRightMargin,
	TEXT("(Default: 10px. The right margin for rendering Gameplay Cameras debug text."));

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
	TEXT("Default: 1. The sort key for drawing the background behind debug text and debug cards."));

int32 GGameplayCamerasDebugCardWidth = 200;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugCardWidth(
	TEXT("GameplayCameras.Debug.CardWidth"),
	GGameplayCamerasDebugCardWidth,
	TEXT("Default: 200px. The width of the debug cards (e.g. graphs, clocks, etc.)"));

int32 GGameplayCamerasDebugCardHeight = 250;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugCardHeight(
	TEXT("GameplayCameras.Debug.CardHeight"),
	GGameplayCamerasDebugCardHeight,
	TEXT("Default: 250px. The height of the debug cards (e.g. graphs, clocks, etc.)"));

int32 GGameplayCamerasDebugCardGap = 10;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugCardGap(
	TEXT("GameplayCameras.Debug.CardGap"),
	GGameplayCamerasDebugCardGap,
	TEXT("Default: 10px. The gap between the debug cards (e.g. graphs, clocks, etc.)"));

int32 GGameplayCamerasDebugMaxCardColumns = 2;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugMaxCardColumns(
	TEXT("GameplayCameras.Debug.MaxCardColumns"),
	GGameplayCamerasDebugMaxCardColumns,
	TEXT("Default: 2. The number of columns to layout the debug cards (e.g. graphs, clocks, etc.)"));

float GGameplayCamerasDebugDefaultCoordinateSystemAxesLength = 100.f;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugDefaultCoordinateSystemAxesLength(
	TEXT("GameplayCameras.Debug.DefaultCoordinateSystemAxesLength"),
	GGameplayCamerasDebugDefaultCoordinateSystemAxesLength,
	TEXT("Default: 100. The default length of coordinate system axes."));

FCameraDebugRenderer::FCameraDebugRenderer(UWorld* InWorld, UCanvas* InCanvasObject)
	: World(InWorld)
	, CanvasObject(InCanvasObject)
	, DrawColor(FColor::White)
{
	RenderFont = GEngine->GetSmallFont();
	MaxCharHeight = RenderFont->GetMaxCharHeight();

	NextDrawPosition = FVector2f{ (float)GGameplayCamerasDebugLeftMargin, (float)GGameplayCamerasDebugTopMargin };

	NextCardPosition = FVector2f::ZeroVector;
	NextCardColumn = 0;
	if (CanvasObject)
	{
		NextCardPosition = FVector2f{ 
			CanvasObject->SizeX - (float)GGameplayCamerasDebugCardWidth - (float)GGameplayCamerasDebugRightMargin,
			(float)GGameplayCamerasDebugTopMargin };
	}
}

FCameraDebugRenderer::~FCameraDebugRenderer()
{
	FlushText();
}

FCanvas* FCameraDebugRenderer::GetCanvas() const
{
	return CanvasObject ? CanvasObject->Canvas : nullptr;
}

FVector2D FCameraDebugRenderer::GetCanvasSize() const
{
	if (CanvasObject)
	{
		FIntPoint ParentSize = CanvasObject->Canvas->GetParentCanvasSize();
		return FVector2D(ParentSize.X, ParentSize.Y);
	}
	return FVector2D::ZeroVector;
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
			FDebugTextRenderer TextRenderer(GetCanvas(), DrawColor, RenderFont);
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
	if (FCanvas* Canvas = GetCanvas())
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

void FCameraDebugRenderer::DrawClock(FCameraDebugClock& InClock, const FText& InClockName)
{
	FCameraDebugClockDrawParams DrawParams;
	DrawParams.ClockName = InClockName;
	DrawParams.ClockPosition = GetNextCardPosition();
	DrawParams.ClockSize = FVector2f(GGameplayCamerasDebugCardWidth, GGameplayCamerasDebugCardHeight);
	InClock.Draw(GetCanvas(), DrawParams);
}

FVector2f FCameraDebugRenderer::GetNextCardPosition()
{
	const FVector2f Result(NextCardPosition);

	++NextCardColumn;
	if (NextCardColumn >= GGameplayCamerasDebugMaxCardColumns)
	{
		// We went over the number of columns we're supposed to stick to.
		// Place the next card below the previous cards, at the right-side edge of the canvas.
		NextCardColumn = 0;
		NextCardPosition.X = CanvasObject->SizeX - (float)GGameplayCamerasDebugCardWidth - (float)GGameplayCamerasDebugRightMargin;
		NextCardPosition.Y += GGameplayCamerasDebugCardHeight + GGameplayCamerasDebugCardGap;
	}
	else
	{
		// We can go to the next column. Place the next card to the left of the previous card.
		NextCardPosition.X -= (float)GGameplayCamerasDebugCardWidth + (float)GGameplayCamerasDebugCardGap;
	}

	return Result;
}

void FCameraDebugRenderer::GetNextDrawGraphParams(FCameraDebugGraphDrawParams& OutDrawParams, const FText& InGraphName)
{
	OutDrawParams.GraphName = InGraphName;
	OutDrawParams.GraphPosition = GetNextCardPosition();
	OutDrawParams.GraphSize = FVector2f(GGameplayCamerasDebugCardWidth, GGameplayCamerasDebugCardHeight);
}

void FCameraDebugRenderer::Draw2DLine(const FVector2D& Start, const FVector2D& End, const FLinearColor& LineColor, float LineThickness)
{
	if (FCanvas* Canvas = GetCanvas())
	{
		FCanvasLineItem LineItem(Start, End);
		LineItem.SetColor(LineColor);
		LineItem.LineThickness = LineThickness;
		Canvas->DrawItem(LineItem);
	}
}

void FCameraDebugRenderer::Draw2DBox(const FBox2D& Box, const FLinearColor& LineColor, float LineThickness)
{
	if (FCanvas* Canvas = GetCanvas())
	{
		FCanvasBoxItem BoxItem(Box.Min, Box.GetSize());
		BoxItem.SetColor(LineColor);
		BoxItem.LineThickness = LineThickness;
		Canvas->DrawItem(BoxItem);
	}
}

void FCameraDebugRenderer::Draw2DBox(const FVector2D& BoxPosition, const FVector2D& BoxSize, const FLinearColor& LineColor, float LineThickness)
{
	if (FCanvas* Canvas = GetCanvas())
	{
		FCanvasBoxItem BoxItem(BoxPosition, BoxSize);
		BoxItem.SetColor(LineColor);
		BoxItem.LineThickness = LineThickness;
		Canvas->DrawItem(BoxItem);
	}
}

void FCameraDebugRenderer::Draw2DCircle(const FVector2D& Center, float Radius, const FLinearColor& LineColor, float LineThickness, int32 NumSides)
{
	if (NumSides <= 0)
	{
		NumSides = FMath::Max(6, (int)Radius / 25);
	}

	const float	AngleDelta = 2.0f * UE_PI / NumSides;
	const FVector2D AxisX(1.f, 0.f);
	const FVector2D AxisY(0.f, -1.f);
	FVector2D LastVertex = Center + AxisX * Radius;

	for (int32 SideIndex = 0; SideIndex < NumSides; SideIndex++)
	{
		const float CurAngle = AngleDelta * (SideIndex + 1);
		const FVector2D Vertex = Center + (AxisX * FMath::Cos(CurAngle) + AxisY * FMath::Sin(CurAngle)) * Radius;
		Draw2DLine(LastVertex, Vertex, LineColor, LineThickness);
		LastVertex = Vertex;
	}
}

void FCameraDebugRenderer::DrawLine(const FVector3d& Start, const FVector3d& End, const FLinearColor& LineColor, float LineThickness)
{
	if (ULineBatchComponent* LineBatcher = GetDebugLineBatcher())
	{
		LineBatcher->DrawLine(Start, End, LineColor, SDPG_Foreground, LineThickness);
	}
}

void FCameraDebugRenderer::DrawSphere(const FVector3d& Center, float Radius, int32 Segments, const FLinearColor& LineColor, float LineThickness)
{
	if (ULineBatchComponent* LineBatcher = GetDebugLineBatcher())
	{
		LineBatcher->DrawSphere(Center, Radius, Segments, LineColor, 0.f, SDPG_Foreground, LineThickness);
	}
}

void FCameraDebugRenderer::DrawDirectionalArrow(const FVector3d& Start, const FVector3d& End, float ArrowSize, const FLinearColor& LineColor, float LineThickness)
{
	if (ULineBatchComponent* LineBatcher = GetDebugLineBatcher())
	{
		LineBatcher->DrawDirectionalArrow(Start, End, ArrowSize, LineColor, 0.f, SDPG_Foreground, LineThickness);
	}
}

void FCameraDebugRenderer::DrawCoordinateSystem(const FVector3d& Location, const FRotator3d& Rotation, float AxesLength)
{
	if (ULineBatchComponent* LineBatcher = GetDebugLineBatcher())
	{
		if (AxesLength <= 0.f)
		{
			AxesLength = GGameplayCamerasDebugDefaultCoordinateSystemAxesLength;
		}

		LineBatcher->DrawLine(
				Location, 
				Location + Rotation.RotateVector(FVector3d::ForwardVector * AxesLength),
				FLinearColor::Red,
				SDPG_Foreground,
				0.f);
		LineBatcher->DrawLine(
				Location, 
				Location + Rotation.RotateVector(FVector3d::RightVector * AxesLength),
				FLinearColor::Green,
				SDPG_Foreground,
				0.f);
		LineBatcher->DrawLine(
				Location, 
				Location + Rotation.RotateVector(FVector3d::UpVector * AxesLength),
				FLinearColor::Blue,
				SDPG_Foreground,
				0.f);
	}
}

void FCameraDebugRenderer::DrawCoordinateSystem(const FTransform3d& Transform, float AxesLength)
{
	DrawCoordinateSystem(Transform.GetLocation(), Transform.GetRotation().Rotator(), AxesLength);
}

void FCameraDebugRenderer::DrawText(const FVector3d& WorldPosition, const FString& Text, const FLinearColor& TextColor, UFont* TextFont)
{
	DrawText(WorldPosition, FVector2d::ZeroVector, Text, TextColor, TextFont);
}

void FCameraDebugRenderer::DrawText(const FVector3d& WorldPosition, const FVector2d& ScreenOffset, const FString& Text, const FLinearColor& TextColor, UFont* TextFont)
{
	if (CanvasObject)
	{
		const FColor PreviousColor = CanvasObject->DrawColor;
		const FVector3d ScreenPosition = CanvasObject->Project(WorldPosition);
		UFont* ActualTextFont = TextFont ? TextFont : GEngine->GetSmallFont();
		CanvasObject->DrawColor = TextColor.ToFColor(true);
		CanvasObject->DrawText(
				ActualTextFont, Text, 
				ScreenPosition.X + ScreenOffset.X, ScreenPosition.Y + ScreenOffset.Y);
		CanvasObject->DrawColor = PreviousColor;
	}
}

ULineBatchComponent* FCameraDebugRenderer::GetDebugLineBatcher() const
{
	return World ? World->ForegroundLineBatcher : nullptr;
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

