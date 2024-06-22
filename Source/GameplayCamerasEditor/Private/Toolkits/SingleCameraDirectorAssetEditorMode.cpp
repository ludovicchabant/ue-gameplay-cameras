// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/SingleCameraDirectorAssetEditorMode.h"

#include "Directors/SingleCameraDirector.h"

namespace UE::Cameras
{

TSharedPtr<FCameraDirectorAssetEditorMode> FSingleCameraDirectorAssetEditorMode::CreateInstance(UCameraAsset* InCameraAsset)
{
	UCameraDirector* CameraDirector = InCameraAsset->GetCameraDirector();
	if (Cast<USingleCameraDirector>(CameraDirector))
	{
		return MakeShared<FSingleCameraDirectorAssetEditorMode>(InCameraAsset);
	}
	return nullptr;
}

FSingleCameraDirectorAssetEditorMode::FSingleCameraDirectorAssetEditorMode(UCameraAsset* InCameraAsset)
	: FCameraDirectorAssetEditorMode(InCameraAsset)
{
	if (InCameraAsset)
	{
		InCameraAsset->EventHandlers.Register(EventHandler, this);
	}
}

void FSingleCameraDirectorAssetEditorMode::OnCameraRigsChanged(UCameraAsset* InCameraAsset, const TCameraArrayChangedEvent<UCameraRigAsset*>& Event)
{
	USingleCameraDirector* CameraDirector = Cast<USingleCameraDirector>(InCameraAsset->GetCameraDirector());
	if (!ensure(CameraDirector))
	{
		return;
	}

	if (!InCameraAsset->GetCameraRigs().Contains(CameraDirector->CameraRig))
	{
		CameraDirector->Modify();
		CameraDirector->CameraRig = nullptr;
	}
}

}  // namespace UE::Cameras

