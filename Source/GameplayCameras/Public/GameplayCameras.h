// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "HAL/Platform.h"
#include "Stats/Stats.h"

#ifndef UE_GAMEPLAY_CAMERAS_DEBUG
	#if (!UE_BUILD_SHIPPING || WITH_UNREAL_DEVELOPER_TOOLS || WITH_UNREAL_TARGET_DEVELOPER_TOOLS)
		#define UE_GAMEPLAY_CAMERAS_DEBUG 1
	#else
		#define UE_GAMEPLAY_CAMERAS_DEBUG 0
	#endif
#endif

DECLARE_STATS_GROUP(TEXT("Camera System Evaluation"), STATGROUP_CameraSystem, STATCAT_Advanced)
DECLARE_STATS_GROUP(TEXT("Camera Animation Evaluation"), STATGROUP_CameraAnimation, STATCAT_Advanced)

DECLARE_LOG_CATEGORY_EXTERN(LogCameraSystem, Log, All);

