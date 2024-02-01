// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Templates/SharedPointer.h"
#include "UObject/UnrealType.h"

class UObject;

/**
 * Interface for an object that can handle live-editing features of the camera assets.
 */
class IGameplayCamerasLiveEditManager : public TSharedFromThis<IGameplayCamerasLiveEditManager>
{
public:

	virtual ~IGameplayCamerasLiveEditManager() {}
};

