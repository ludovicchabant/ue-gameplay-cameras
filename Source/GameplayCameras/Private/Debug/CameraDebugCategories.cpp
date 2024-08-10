// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDebugCategories.h"

#include "Containers/UnrealString.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

const FString FCameraDebugCategories::NodeTree("nodetree");
const FString FCameraDebugCategories::DirectorTree("directortree");
const FString FCameraDebugCategories::BlendStacks("blendstacks");
const FString FCameraDebugCategories::Services("services");
const FString FCameraDebugCategories::PoseStats("posestats");
const FString FCameraDebugCategories::Viewfinder("viewfinder");

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

