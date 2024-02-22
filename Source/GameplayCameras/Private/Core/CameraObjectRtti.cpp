// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraObjectRtti.h"

namespace UE::Cameras
{

uint32 FCameraObjectTypeID::RegisterNewID()
{
	static uint32 ID = 0;
	return ID++;
}

}  // namespace UE::Cameras

