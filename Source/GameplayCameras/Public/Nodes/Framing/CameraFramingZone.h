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
	FDoubleCameraParameter LeftMargin;

	UPROPERTY(EditAnywhere, Category="Framing")
	FDoubleCameraParameter TopMargin;

	UPROPERTY(EditAnywhere, Category="Framing")
	FDoubleCameraParameter RightMargin;

	UPROPERTY(EditAnywhere, Category="Framing")
	FDoubleCameraParameter BottomMargin;

public:

	FCameraFramingZone()
	{
		LeftMargin.Value = 0;
		TopMargin.Value = 0;
		RightMargin.Value = 0;
		BottomMargin.Value = 0;
	}

	FCameraFramingZone(double UniformMargin)
	{
		LeftMargin.Value = UniformMargin;
		TopMargin.Value = UniformMargin;
		RightMargin.Value = UniformMargin;
		BottomMargin.Value = UniformMargin;
	}

	FCameraFramingZone(double HorizontalMargin, double VerticalMargin)
	{
		LeftMargin.Value = HorizontalMargin;
		TopMargin.Value = VerticalMargin;
		RightMargin.Value = HorizontalMargin;
		BottomMargin.Value = VerticalMargin;
	}

	FCameraFramingZone(double InLeftMargin, double InTopMargin, double InRightMargin, double InBottomMargin)
	{
		LeftMargin.Value = InLeftMargin;
		TopMargin.Value = InTopMargin;
		RightMargin.Value = InRightMargin;
		BottomMargin.Value = InBottomMargin;
	}
};

