// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDebugRenderer.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "HAL/IConsoleManager.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

int32 GGameplayCamerasDebugMargin = 10;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugMargin(
	TEXT("GameplayCameras.Debug.Margin"),
	GGameplayCamerasDebugMargin,
	TEXT("(Default: 10px. The margin for rendering Gameplay Cameras debug text."));

int32 GGameplayCamerasDebugIndent = 20;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugIndent(
	TEXT("GameplayCameras.Debug.Indent"),
	GGameplayCamerasDebugIndent,
	TEXT("(Default: 20px. The indent for rendering Gameplay Cameras debug text."));

namespace UE::Cameras
{

FCameraDebugRenderer::FCameraDebugRenderer(UCanvas* InCanvas)
	: Canvas(InCanvas)
{
	OriginalDrawColor = Canvas->DrawColor;
	Canvas->SetDrawColor(FColor::White);

	RenderFont = GEngine->GetSmallFont();
	MaxCharHeight = RenderFont->GetMaxCharHeight();

	NextBlockY = 0;
}

FCameraDebugRenderer::~FCameraDebugRenderer()
{
	FlushText();
	Canvas->SetDrawColor(OriginalDrawColor);
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

void FCameraDebugRenderer::FlushText()
{
	if (LineBuilder.Len() > 0)
	{
		if (NextBlockY < Canvas->ClipY)
		{
			int32 BlockHeight, BlockWidth;
			const TCHAR* TextBlock = LineBuilder.ToString();
			RenderFont->GetStringHeightAndWidth(TextBlock, BlockHeight, BlockWidth);

			float X = GGameplayCamerasDebugMargin + IndentLevel * GGameplayCamerasDebugIndent;
			float Y = GGameplayCamerasDebugMargin + NextBlockY;
			Canvas->DrawText(RenderFont, TextBlock, X, Y);

			NextBlockY += BlockHeight;
		}
		// else: text is going off-screen.

		LineBuilder.Reset();
	}
}

void FCameraDebugRenderer::AddIndent()
{
	// Flush any remaining text we have on the current indent level.
	FlushText();

	++IndentLevel;
}

void FCameraDebugRenderer::RemoveIndent()
{
	// Flush any remaining text we have on the current indent level.
	FlushText();

	if (ensureMsgf(IndentLevel > 0, TEXT("Can't go into negative indenting!")))
	{
		--IndentLevel;
	}
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

