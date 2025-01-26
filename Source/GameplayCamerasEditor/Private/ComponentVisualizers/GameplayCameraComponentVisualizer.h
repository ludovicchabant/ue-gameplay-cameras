// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ComponentVisualizer.h"

class FPrimitiveDrawInterface;
class FSceneView;
class UActorComponent;

namespace UE::Cameras
{

class FGameplayCameraComponentVisualizer : public FComponentVisualizer
{
public:

	// FComponentVisualizer interface.
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
};

}  // namespace UE::Cameras

