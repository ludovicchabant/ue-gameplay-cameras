// Copyright Epic Games, Inc. All Rights Reserved.

#include "Trace/CameraSystemTraceProvider.h"

#if UE_GAMEPLAY_CAMERAS_TRACE

namespace UE::Cameras
{

FName FCameraSystemTraceProvider::ProviderName("CameraSystemTraceProvider");

FCameraSystemTraceProvider::FCameraSystemTraceProvider(TraceServices::IAnalysisSession& InSession)
	: Session(InSession)
{
	Timeline = MakeShared<FFrameDataPointTimeline>(Session.GetLinearAllocator());
}

void FCameraSystemTraceProvider::AppendFrameData(double InRecordingTime, FCameraSystemTraceFrameData&& InFrameData)
{
	Session.WriteAccessCheck();

	Timeline->AppendEvent(InRecordingTime, MoveTemp(InFrameData));
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_TRACE

