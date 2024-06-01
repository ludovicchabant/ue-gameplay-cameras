// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Tools/UAssetEditor.h"

#include "CameraRigTransitionEditor.generated.h"

class FBaseAssetToolkit;

/**
 * Editor for camera transitions.
 */
UCLASS(Transient)
class UCameraRigTransitionEditor : public UAssetEditor
{
	GENERATED_BODY()

public:

	void Initialize(TObjectPtr<UObject> InTransitionOwner);

	UObject* GetTransitionOwner() const { return TransitionOwner; }

public:

	// UAssetEditor interface
	virtual void GetObjectsToEdit(TArray<UObject*>& InObjectsToEdit) override;
	virtual TSharedPtr<FBaseAssetToolkit> CreateToolkit() override;

private:

	UPROPERTY()
	TObjectPtr<UObject> TransitionOwner;
};

