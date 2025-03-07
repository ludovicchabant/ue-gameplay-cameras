// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Build/CameraBuildLog.h"
#include "Containers/ArrayView.h"
#include "Core/CameraContextDataTableFwd.h"
#include "Core/CameraVariableTableFwd.h"

class UBaseCameraObject;
class UCameraObjectInterfaceBlendableParameter;
class UCameraObjectInterfaceDataParameter;
class UCameraNode;
struct FCameraObjectInterface;

namespace UE::Cameras
{

class FCameraNodeHierarchy;

namespace Internal { struct FInterfaceParameterBindingBuilder; }

class GAMEPLAYCAMERAS_API FCameraObjectInterfaceBuilder
{
public:

	FCameraObjectInterfaceBuilder(FCameraBuildLog& InBuildLog);

	void BuildInterface(UBaseCameraObject* InCameraObject, const FCameraNodeHierarchy& InHierarchy, bool bCollectStrayNodes);
	void BuildInterface(UBaseCameraObject* InCameraObject, TArrayView<UCameraNode*> InCameraObjectNodes);

private:

	void BuildInterfaceImpl();

	void GatherOldDrivenParameters();
	void BuildInterfaceParameters();
	void BuildInterfaceParameterBindings();
	void DiscardUnusedParameters();

private:

	bool SetupCameraParameterOrVariableReferenceOverride(const UCameraObjectInterfaceBlendableParameter* BlendableParameter);
	bool SetupCustomBlendableParameterOverride(const UCameraObjectInterfaceBlendableParameter* BlendableParameter);

	bool SetupDataContextPropertyOverride(const UCameraObjectInterfaceDataParameter* DataParameter);
	bool SetupCustomDataParameterOverride(const UCameraObjectInterfaceDataParameter* DataParameter);

private:

	FCameraBuildLog& BuildLog;

	UBaseCameraObject* CameraObject = nullptr;
	TArray<UCameraNode*> CameraObjectNodes;

	using FDrivenParameterKey = TTuple<FName, UObject*>;
	TMap<FDrivenParameterKey, FCameraVariableID> OldDrivenBlendableParameters;
	TMap<FDrivenParameterKey, FCameraContextDataID> OldDrivenDataParameters;

	friend struct Internal::FInterfaceParameterBindingBuilder;
};

}  // namespace UE::Cameras

