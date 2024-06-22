// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/BlueprintCameraDirectorAssetEditorMode.h"

#include "Core/CameraRigProxyTable.h"
#include "Directors/BlueprintCameraDirector.h"

namespace UE::Cameras
{

TSharedPtr<FCameraDirectorAssetEditorMode> FBlueprintCameraDirectorAssetEditorMode::CreateInstance(UCameraAsset* InCameraAsset)
{
	UCameraDirector* CameraDirector = InCameraAsset->GetCameraDirector();
	if (Cast<UBlueprintCameraDirector>(CameraDirector))
	{
		return MakeShared<FBlueprintCameraDirectorAssetEditorMode>(InCameraAsset);
	}
	return nullptr;
}

FBlueprintCameraDirectorAssetEditorMode::FBlueprintCameraDirectorAssetEditorMode(UCameraAsset* InCameraAsset)
	: FCameraDirectorAssetEditorMode(InCameraAsset)
{
	if (InCameraAsset)
	{
		InCameraAsset->EventHandlers.Register(EventHandler, this);
	}
}

void FBlueprintCameraDirectorAssetEditorMode::OnCameraRigsChanged(UCameraAsset* InCameraAsset, const TCameraArrayChangedEvent<UCameraRigAsset*>& Event)
{
	UBlueprintCameraDirector* CameraDirector = Cast<UBlueprintCameraDirector>(InCameraAsset->GetCameraDirector());
	if (!ensure(CameraDirector))
	{
		return;
	}

	TSet<UCameraRigAsset*> CameraRigs(InCameraAsset->GetCameraRigs());

	for (FCameraRigProxyTableEntry& Entry : CameraDirector->CameraRigProxyTable->Entries)
	{
		if (!CameraRigs.Contains(Entry.CameraRig))
		{
			CameraDirector->CameraRigProxyTable->Modify();
			Entry.CameraRig = nullptr;
		}
	}
}

}  // namespace UE::Cameras

