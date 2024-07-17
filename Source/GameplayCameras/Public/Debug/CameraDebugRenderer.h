// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "GameplayCameras.h"
#include "Math/Color.h"
#include "Math/MathFwd.h"
#include "Math/Vector.h"
#include "Misc/StringBuilder.h"

class FCanvas;
class UFont;
class ULineBatchComponent;
class UWorld;

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
	GAMEPLAYCAMERAS_API FCameraDebugRenderer(UWorld* InWorld, FCanvas* InCanvas);
	/** Destroys the debug renderer. */
	GAMEPLAYCAMERAS_API ~FCameraDebugRenderer();

	/** Adds text to the text wall. */
	GAMEPLAYCAMERAS_API void AddText(const FString& InString);
	GAMEPLAYCAMERAS_API void AddText(const TCHAR* Fmt, ...);

	/** 
	 * Move to a new line on the text wall.
	 *
	 * @return Whether a new line was added.
	 */
	GAMEPLAYCAMERAS_API bool NewLine(bool bSkipIfEmptyLine = false);

	/** Gets the current text color. */
	GAMEPLAYCAMERAS_API FColor GetTextColor() const;
	/** Sets the text color for further calls. Returns the previous color. */
	GAMEPLAYCAMERAS_API FColor SetTextColor(const FColor& Color);

	/** Increases the indent of the next text wall entry. This will make a new line. */
	GAMEPLAYCAMERAS_API void AddIndent();
	/** Decreases the indent of the next text wall entry. This will make a new line. */
	GAMEPLAYCAMERAS_API void RemoveIndent();

	/** Draws a translucent background behind the text. */
	GAMEPLAYCAMERAS_API void DrawTextBackgroundTile(float Opacity);

public:

	/** Draws a 2D line. */
	GAMEPLAYCAMERAS_API void Draw2DLine(const FVector2D& Start, const FVector2D& End, const FLinearColor& LineColor, float LineThickness = 1.f);
	/** Draws a 2D box. */
	GAMEPLAYCAMERAS_API void Draw2DBox(const FBox2D& Box, const FLinearColor& LineColor, float LineThickness = 1.f);
	/** Draws a 2D box. */
	GAMEPLAYCAMERAS_API void Draw2DBox(const FVector2D& BoxPosition, const FVector2D& BoxSize, const FLinearColor& LineColor, float LineThickness = 1.f);
	/** Draws a 2D circle. */
	GAMEPLAYCAMERAS_API void Draw2DCircle(const FVector2D& Center, float Radius, const FLinearColor& LineColor, float LineThickness = 1.f, int32 NumSides = 0);

	/** Draws a 3D line. */
	GAMEPLAYCAMERAS_API void DrawLine(const FVector3d& Start, const FVector3d& End, const FLinearColor& LineColor, float LineThickness = 1.f);
	/** Draws a 3D sphere. */
	GAMEPLAYCAMERAS_API void DrawSphere(const FVector3d& Center, float Radius, int32 Segments, const FLinearColor& LineColor, float LineThickness);

public:

	/** Request skipping drawing any blocks attached to the current block. */
	GAMEPLAYCAMERAS_API void SkipAttachedBlocks();
	/** Request skipping drawing any children blocks of the current block. */
	GAMEPLAYCAMERAS_API void SkipChildrenBlocks();
	/** Skip all related blocks (attached, children, etc.) */
	GAMEPLAYCAMERAS_API void SkipAllBlocks();
	/**Gets block visiting flags. */
	GAMEPLAYCAMERAS_API ECameraDebugDrawVisitFlags GetVisitFlags() const;
	/** Resets block visiting flags. */
	GAMEPLAYCAMERAS_API void ResetVisitFlags();

public:

	/** Gets the drawing canvas. */
	FCanvas* GetCanvas() const { return Canvas; }

	/** Gets the size of the canvas. */
	FVector2D GetCanvasSize() const;

	bool HasCanvas() const { return Canvas != nullptr; }

private:

	void AddTextFmtImpl(const TCHAR* Fmt, va_list Args);
	void AddTextImpl(const TCHAR* Buffer);

	float GetIndentMargin() const;
	void FlushText();

	ULineBatchComponent* GetDebugLineBatcher() const;

private:

	/** The world in which we might draw debug primitives. */
	UWorld* World;
	/** The canvas used to draw the text wall. */
	FCanvas* Canvas;
	/** The draw color of the canvas. */
	FColor DrawColor;

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

