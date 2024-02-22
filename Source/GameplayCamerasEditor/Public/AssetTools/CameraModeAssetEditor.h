// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Tools/UAssetEditor.h"

#include "CameraModeAssetEditor.generated.h"

class FBaseAssetToolkit;
class UCameraMode;

/**
 * Editor for a camera mode asset.
 */
UCLASS(Transient)
class UCameraModeAssetEditor : public UAssetEditor
{
	GENERATED_BODY()

public:

	void Initialize(TObjectPtr<UCameraMode> InCameraModeAsset);

	UCameraMode* GetCameraModeAsset() const { return CameraModeAsset; }

public:

	// UAssetEditor interface
	virtual void GetObjectsToEdit(TArray<UObject*>& InObjectsToEdit) override;
	virtual TSharedPtr<FBaseAssetToolkit> CreateToolkit() override;

private:

	TObjectPtr<UCameraMode> CameraModeAsset;
};

