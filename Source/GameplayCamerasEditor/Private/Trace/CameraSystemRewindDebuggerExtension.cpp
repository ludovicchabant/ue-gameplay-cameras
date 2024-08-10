// Copyright Epic Games, Inc. All Rights Reserved.

#include "Trace/CameraSystemRewindDebuggerExtension.h"

#include "Core/CameraPose.h"
#include "Debug/CameraDebugRenderer.h"
#include "Debug/CameraSystemTrace.h"
#include "Debug/CategoryTitleDebugBlock.h"
#include "Debug/DebugDrawService.h"
#include "Debug/RootCameraDebugBlock.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraSystemActor.h"
#include "Trace/CameraSystemTraceProvider.h"

#if UE_GAMEPLAY_CAMERAS_TRACE

namespace UE::Cameras
{

FCameraSystemRewindDebuggerExtension::FCameraSystemRewindDebuggerExtension()
{
}

FCameraSystemRewindDebuggerExtension::~FCameraSystemRewindDebuggerExtension()
{
	EnsureDebugDrawDelegate(false);
}

void FCameraSystemRewindDebuggerExtension::RecordingStarted(IRewindDebugger* RewindDebugger)
{
	UE::Trace::ToggleChannel(*FCameraSystemTrace::ChannelName, true);
}

void FCameraSystemRewindDebuggerExtension::Update(float DeltaTime, IRewindDebugger* RewindDebugger)
{
	if (RewindDebugger->IsPIESimulating() || RewindDebugger->GetRecordingDuration() == 0.0)
	{
		EnsureDebugDrawDelegate(false);
		return;
	}

	EnsureDebugDrawDelegate(true);

	const TraceServices::IAnalysisSession* AnalysisSession = RewindDebugger->GetAnalysisSession();
	TraceServices::FAnalysisSessionReadScope SessionReadScope(*AnalysisSession);
	
	const FCameraSystemTraceProvider* CameraSystemProvider = AnalysisSession->ReadProvider<FCameraSystemTraceProvider>(FCameraSystemTraceProvider::ProviderName);
	const TraceServices::IFrameProvider& FrameProvider = TraceServices::ReadFrameProvider(*RewindDebugger->GetAnalysisSession());

	double CurrentTraceTime = RewindDebugger->CurrentTraceTime();

	TraceServices::FFrame Frame;
	if (FrameProvider.GetFrameFromTime(ETraceFrameType::TraceFrameType_Game, CurrentTraceTime, Frame))
	{
		const FCameraSystemTraceFrameData* FoundFrameData = nullptr;

		const FCameraSystemTraceTimeline* CameraSystemTimeline = CameraSystemProvider->GetTimeline();
		CameraSystemTimeline->EnumerateEvents(
				Frame.StartTime, Frame.EndTime, 
				[&FoundFrameData](double InStartTime, double InEndTime, uint32 InDepth, const FCameraSystemTraceFrameData& FrameData)  
				{
					FoundFrameData = &FrameData;
					return TraceServices::EEventEnumerate::Continue;
				});

		if (FoundFrameData && CurrentTraceTime != LastTraceTime)
		{
			LastTraceTime = CurrentTraceTime;
			VisualizedWorld = RewindDebugger->GetWorldToVisualize();

			DebugBlockStorage.DestroyDebugBlocks();

			FCameraDebugBlock* ReadBlock = FCameraSystemTrace::ReadEvaluationTrace(
					FoundFrameData->SerializedBlocks, DebugBlockStorage);
			ensure(ReadBlock && ReadBlock->GetTypeID() == FRootCameraDebugBlock::StaticTypeID());
			RootDebugBlock = ReadBlock->CastThisChecked<FRootCameraDebugBlock>();
		}
	}
}

void FCameraSystemRewindDebuggerExtension::RecordingStopped(IRewindDebugger* RewindDebugger)
{
	UE::Trace::ToggleChannel(*FCameraSystemTrace::ChannelName, false);
}

void FCameraSystemRewindDebuggerExtension::EnsureDebugDrawDelegate(bool bIsRegistered)
{
	if (bIsRegistered && !DebugDrawDelegateHandle.IsValid())
	{
		DebugDrawDelegateHandle = UDebugDrawService::Register(
				TEXT("GameplayDebug"), FDebugDrawDelegate::CreateRaw(this, &FCameraSystemRewindDebuggerExtension::DebugDraw));
	}
	else if (!bIsRegistered && DebugDrawDelegateHandle.IsValid())
	{
		UDebugDrawService::Unregister(DebugDrawDelegateHandle);
		DebugDrawDelegateHandle .Reset();
	}
}

void FCameraSystemRewindDebuggerExtension::DebugDraw(UCanvas* Canvas, APlayerController* PlayController)
{
	if (RootDebugBlock && VisualizedWorld)
	{
		FCameraDebugRenderer CameraDebugRenderer(VisualizedWorld, Canvas);
		RootDebugBlock->RootDebugDraw(CameraDebugRenderer);
	}
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_TRACE

