// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Core/CameraParameters.h"

#include "CameraFramingZone.generated.h"

/**
 * A structure that defines a zone for use in framing subjects in screen-space.
 *
 * All margins are defined in percentages of the screen's horizontal size. They are also 
 * all defined relative to their respective edges.
 */
USTRUCT()
struct FCameraFramingZone
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Framing")
	FFloatCameraParameter LeftMargin;

	UPROPERTY(EditAnywhere, Category="Framing")
	FFloatCameraParameter TopMargin;

	UPROPERTY(EditAnywhere, Category="Framing")
	FFloatCameraParameter RightMargin;

	UPROPERTY(EditAnywhere, Category="Framing")
	FFloatCameraParameter BottomMargin;

public:

	FCameraFramingZone()
	{
		LeftMargin.Value = 0;
		TopMargin.Value = 0;
		RightMargin.Value = 0;
		BottomMargin.Value = 0;
	}

	FCameraFramingZone(float UniformMargin)
	{
		LeftMargin.Value = UniformMargin;
		TopMargin.Value = UniformMargin;
		RightMargin.Value = UniformMargin;
		BottomMargin.Value = UniformMargin;
	}

	FCameraFramingZone(float HorizontalMargin, float VerticalMargin)
	{
		LeftMargin.Value = HorizontalMargin;
		TopMargin.Value = VerticalMargin;
		RightMargin.Value = HorizontalMargin;
		BottomMargin.Value = VerticalMargin;
	}

	FCameraFramingZone(float InLeftMargin, float InTopMargin, float InRightMargin, float InBottomMargin)
	{
		LeftMargin.Value = InLeftMargin;
		TopMargin.Value = InTopMargin;
		RightMargin.Value = InRightMargin;
		BottomMargin.Value = InBottomMargin;
	}
};

