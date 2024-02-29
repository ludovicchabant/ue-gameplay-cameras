// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "GameplayCameras.h"
#include "Math/Color.h"
#include "Misc/StringBuilder.h"

class UCanvas;
class UFont;

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

class FCameraDebugRenderer
{
public:

	FCameraDebugRenderer(UCanvas* InCanvas);
	~FCameraDebugRenderer();

	void AddText(const FString& InString);
	void AddText(const TCHAR* Fmt, ...);

	void AddIndent();
	void RemoveIndent();

private:

	void AddTextFmtImpl(const TCHAR* Fmt, va_list Args);
	void AddTextImpl(const TCHAR* Buffer);

	void FlushText();

private:

	UCanvas* Canvas;
	FColor OriginalDrawColor;

	const UFont* RenderFont;
	int32 MaxCharHeight;

	TStringBuilder<512> Formatter;
	TStringBuilder<512> LineBuilder;

	int8 IndentLevel = 0;
	float NextBlockY = 0;
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

