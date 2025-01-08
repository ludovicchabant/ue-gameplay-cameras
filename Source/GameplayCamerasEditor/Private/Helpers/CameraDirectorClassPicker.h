// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Templates/SubclassOf.h"

class UCameraDirector;

namespace UE::Cameras
{

class FCameraDirectorClassPicker
{
public:

	FCameraDirectorClassPicker();

	void AddCommonCameraDirector(TSubclassOf<UCameraDirector> InClass);
	void ResetCommonCameraDirectors();

	bool PickCameraDirectorClass(TSubclassOf<UCameraDirector>& OutChosenClass);

private:

	TArray<TSubclassOf<UCameraDirector>> CommonCameraDirectorClasses;
};

}  // namespace UE::Cameras

