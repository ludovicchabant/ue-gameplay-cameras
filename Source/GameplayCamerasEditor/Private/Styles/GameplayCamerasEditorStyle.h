// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Styling/SlateStyle.h"

namespace UE::Cameras
{

/**
 * Implements the visual style of the gameplay cameras editors.
 */
class FGameplayCamerasEditorStyle final : public FSlateStyleSet
{
public:
	
	FGameplayCamerasEditorStyle();
	virtual ~FGameplayCamerasEditorStyle();

	static TSharedRef<FGameplayCamerasEditorStyle> Get();

private:

	static TSharedPtr<FGameplayCamerasEditorStyle> Singleton;
};

}  // namespace UE::Cameras

