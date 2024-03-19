// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/ObjectTreeConnectionDrawingPolicy.h"

FObjectTreeConnectionDrawingPolicy::FObjectTreeConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements)
	: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
{
	// Don't draw arrowheads.
	ArrowImage = nullptr;
	ArrowRadius = FVector2D::ZeroVector;
}

