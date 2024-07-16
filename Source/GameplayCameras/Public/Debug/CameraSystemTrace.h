// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "GameplayCameras.h"
#include "Trace/Config.h"
#include "TraceFilter.h"

#if UE_GAMEPLAY_CAMERAS_TRACE

class UWorld;

namespace UE::Cameras
{

class FCameraDebugBlock;
class FCameraDebugBlockStorage;
struct FCameraSystemEvaluationResult;

/**
 * Trace utility class for the camera system.
 */
class FCameraSystemTrace
{
public:

	GAMEPLAYCAMERAS_API static FString ChannelName;
	GAMEPLAYCAMERAS_API static FString LoggerName;
	GAMEPLAYCAMERAS_API static FString EvaluationEventName;

public:

	/** Gets whether we are currently replaying traced information (such as with rewind debugger). */
	GAMEPLAYCAMERAS_API static bool IsTraceReplay();
	/** Sets whether we are currently replaying traced information (such as with rewind debugger). */
	GAMEPLAYCAMERAS_API static void SetTraceReplay(bool bInIsReplaying);

	/** Returns whether tracing of camera system evaluation is enabled. */
	GAMEPLAYCAMERAS_API static bool IsTraceEnabled();
	/** Records one frame of camera system evaluation. */
	GAMEPLAYCAMERAS_API static void TraceEvaluation(UWorld* InWorld, const FCameraSystemEvaluationResult& InResult, FCameraDebugBlock& InRootDebugBlock);
	/** Reads back one frame of camera system evaluation. */
	GAMEPLAYCAMERAS_API static FCameraDebugBlock* ReadEvaluationTrace(TArray<uint8> InSerializedBlocks, FCameraDebugBlockStorage& InStorage);

private:

	static bool bIsReplaying;
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_TRACE

