// Copyright Epic Games, Inc. All Rights Reserved.

#include "ComponentVisualizers/GameplayCameraComponentVisualizer.h"

#include "GameFramework/GameplayCameraComponentBase.h"

namespace UE::Cameras
{

void FGameplayCameraComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
}

void FGameplayCameraComponentVisualizer::DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (const UGameplayCameraComponentBase* GameplayCameraComponent = Cast<const UGameplayCameraComponentBase>(Component))
	{
		GameplayCameraComponent->OnDrawVisualizationHUD(Viewport, View, Canvas);
	}
}

}  // namespace UE::Cameras

