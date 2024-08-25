// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

#include "CameraNodeTypes.generated.h"

/**
 * Defines what space a camera node should operate in.
 */
UENUM()
enum class ECameraNodeSpace
{
	/** Operates in the local camera pose space. */
	CameraPose,
	/** Operates in the space of the owning evaluation context's initial result. */
	OwningContext,
	/** Operates in world space. */
	World
};

