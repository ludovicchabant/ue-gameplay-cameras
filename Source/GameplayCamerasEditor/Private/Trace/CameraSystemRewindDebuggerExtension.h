// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Debug/CameraDebugBlockStorage.h"
#include "GameplayCameras.h"
#include "IRewindDebugger.h"
#include "IRewindDebuggerExtension.h"
#include "Templates/SharedPointerFwd.h"

#if UE_GAMEPLAY_CAMERAS_TRACE

class APlayerController;
class UCanvas;

namespace UE::Cameras
{

class FDebugCanvasDeferredRenderer;
class FRootCameraDebugBlock;

/**
 * Rewind debugger extension for the camera system evaluation trace.
 */
class FCameraSystemRewindDebuggerExtension : public IRewindDebuggerExtension
{
public:

	FCameraSystemRewindDebuggerExtension();
	virtual ~FCameraSystemRewindDebuggerExtension();

	virtual void RecordingStarted(IRewindDebugger* RewindDebugger) override;
	virtual void Update(float DeltaTime, IRewindDebugger* RewindDebugger) override;
	virtual void RecordingStopped(IRewindDebugger* RewindDebugger) override;

private:

	void EnsureDebugDrawDelegate(bool bIsRegistered);
	void DebugDraw(UCanvas* Canvas, APlayerController* PlayController);

private:

	FDelegateHandle DebugDrawDelegateHandle;
	double LastTraceTime = 0.f;

	UWorld* VisualizedWorld = nullptr;

	FCameraDebugBlockStorage DebugBlockStorage;
	FRootCameraDebugBlock* RootDebugBlock = nullptr;

	TSharedPtr<FDebugCanvasDeferredRenderer> DeferredRenderer;
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_TRACE

