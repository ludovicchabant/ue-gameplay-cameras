// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

class UBlueprint;
class UCameraAsset;

namespace UE::Cameras
{

class FCameraBuildLog;

/**
 * Camera asset builder that does extra validation related to Blueprint camera directors.
 */
class FBlueprintCameraDirectorEditorBuilder
{
public:

	/** Callback registered on IGameplayCamerasEditorModule's camera asset builders */
	static void OnBuildCameraAsset(UCameraAsset* CameraAsset, UE::Cameras::FCameraBuildLog& BuildLog);

private:

	static bool UsesDirectCameraRigActivation(UBlueprint* Blueprint);
};

}  // namespace UE::Cameras

