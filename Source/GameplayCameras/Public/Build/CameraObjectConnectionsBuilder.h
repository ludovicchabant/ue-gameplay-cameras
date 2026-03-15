// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Build/CameraBuildContext.h"
#include "Containers/ArrayView.h"
#include "Core/CameraContextDataTableFwd.h"
#include "Core/CameraVariableTableFwd.h"

#define UE_API GAMEPLAYCAMERAS_API

class UBaseCameraObject;
class UCameraObjectInterfaceBlendableParameter;
class UCameraObjectInterfaceDataParameter;
class UCameraNode;
struct FCameraObjectInterface;

namespace UE::Cameras
{

class FCameraNodeHierarchy;

namespace Internal { struct FInterfaceParameterBindingBuilder; }

class FCameraObjectConnectionsBuilder
{
public:

	UE_API FCameraObjectConnectionsBuilder(FCameraBuildContext& InBuildContext);

	UE_API void BuildConnections(UBaseCameraObject* InCameraObject, const FCameraNodeHierarchy& InHierarchy, bool bCollectStrayNodes);
	UE_API void BuildConnections(UBaseCameraObject* InCameraObject, TArrayView<UCameraNode*> InCameraObjectNodes);

private:

	UE_API void GatherOldDrivenParameters();
	UE_API void BuildConnectionsImpl();
	UE_API void DiscardUnusedParameters();

private:

	FCameraBuildContext BuildContext;

	UBaseCameraObject* CameraObject = nullptr;
	TArray<UCameraNode*> CameraObjectNodes;

	using FDrivenParameterKey = TTuple<UObject*, FName>;
	TMap<FDrivenParameterKey, FCameraVariableID> OldDrivenBlendableParameters;
	TMap<FDrivenParameterKey, FCameraContextDataID> OldDrivenDataParameters;

	friend struct Internal::FInterfaceParameterBindingBuilder;
};

}  // namespace UE::Cameras

#undef UE_API
