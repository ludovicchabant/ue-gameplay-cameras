// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "GameplayCameras.h"
#include "Math/Color.h"
#include "Math/Vector.h"
#include "Misc/StringBuilder.h"

class UCanvas;
class UFont;

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

class FDebugTextRenderer;

enum class ECameraDebugDrawVisitFlags
{
	None = 0,
	SkipAttachedBlocks = 1 << 0,
	SkipChildrenBlocks = 1 << 1
};
ENUM_CLASS_FLAGS(ECameraDebugDrawVisitFlags)

/**
 * Utility class for camera-related debug drawing.
 */
class FCameraDebugRenderer
{
public:

	/** Creates a new debug renderer. */
	FCameraDebugRenderer(UCanvas* InCanvas);
	/** Destroys the debug renderer. */
	~FCameraDebugRenderer();

	/** Adds text to the text wall. */
	void AddText(const FString& InString);
	void AddText(const TCHAR* Fmt, ...);

	/** 
	 * Move to a new line on the text wall.
	 *
	 * @return Whether a new line was added.
	 */
	bool NewLine(bool bSkipIfEmptyLine = false);

	/** Gets the current text color. */
	FColor GetTextColor() const;
	/** Sets the text color for further calls. Returns the previous color. */
	FColor SetTextColor(const FColor& Color);

	/** Increases the indent of the next text wall entry. This will make a new line. */
	void AddIndent();
	/** Decreases the indent of the next text wall entry. This will make a new line. */
	void RemoveIndent();

	/** Draws a translucent background behind the text. */
	void DrawTextBackgroundTile(float Opacity);

public:

	/** Request skipping drawing any blocks attached to the current block. */
	void SkipAttachedBlocks();
	/** Request skipping drawing any children blocks of the current block. */
	void SkipChildrenBlocks();
	/** Skip all related blocks (attached, children, etc.) */
	void SkipAllBlocks();
	/**Gets block visiting flags. */
	ECameraDebugDrawVisitFlags GetVisitFlags() const;
	/** Resets block visiting flags. */
	void ResetVisitFlags();

public:

	/** Gets the drawing canvas. */
	UCanvas* GetCanvas() const { return Canvas; }

private:

	void AddTextFmtImpl(const TCHAR* Fmt, va_list Args);
	void AddTextImpl(const TCHAR* Buffer);

	float GetIndentMargin() const;
	void FlushText();

private:

	/** The canvas used to draw the text wall. */
	UCanvas* Canvas;
	/** The original draw color of the canvas. */
	FColor OriginalDrawColor;

	/** The font used to render the text wall. */
	const UFont* RenderFont;
	/** The height of one line of the text wall. */
	int32 MaxCharHeight;

	/** Temporary string formatter for variadic methods. */
	TStringBuilder<512> Formatter;
	/** String formatter for building a line up until the point it needs to be rendered. */
	TStringBuilder<512> LineBuilder;

	/** Current indent level. */
	int8 IndentLevel = 0;
	/** The screenspace coordinates for the next block of text on the wall. */
	FVector2f NextDrawPosition;
	/** The maximum horizontal extent of the text rendered so far. */
	float RightMargin = 0;

	/** How to visit the next debug blocks. */
	ECameraDebugDrawVisitFlags VisitFlags;
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

