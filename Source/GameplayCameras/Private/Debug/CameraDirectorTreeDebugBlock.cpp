// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDirectorTreeDebugBlock.h"

#include "Core/CameraAsset.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraEvaluationContextStack.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugColors.h"
#include "Debug/CameraDebugRenderer.h"
#include "Debug/CameraPoseDebugBlock.h"
#include "HAL/IConsoleManager.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

bool GGameplayCamerasDebugContextInitialResultShowUnchanged = false;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugContextInitialResultShowUnchanged(
	TEXT("GameplayCameras.Debug.ContextInitialResult.ShowUnchanged"),
	GGameplayCamerasDebugContextInitialResultShowUnchanged,
	TEXT(""));

UE_DEFINE_CAMERA_DEBUG_BLOCK(FCameraDirectorTreeDebugBlock)

FCameraDirectorTreeDebugBlock::FCameraDirectorTreeDebugBlock()
{
}

void FCameraDirectorTreeDebugBlock::Initialize(const FCameraEvaluationContextStack& ContextStack, FCameraDebugBlockBuilder& Builder)
{
	const int32 NumContexts = ContextStack.NumContexts();
	for (int32 Index = 0; Index < NumContexts; ++Index)
	{
		const FCameraEvaluationContextStack::FContextEntry& Entry(ContextStack.Entries[Index]);
		TSharedPtr<FCameraEvaluationContext> Context = Entry.WeakContext.Pin();

		FDirectorDebugInfo EntryDebugInfo;
		EntryDebugInfo.CameraAssetName = Context ? Context->GetCameraAsset()->GetName() : TEXT("<no camera asset>");
		CameraDirectors.Add(EntryDebugInfo);

		const FCameraNodeEvaluationResult& InitialResult = Context->GetInitialResult();
		AddChild(&Builder.BuildDebugBlock<FCameraPoseDebugBlock>(InitialResult.CameraPose)
				.WithShowUnchangedCVar(TEXT("GameplayCameras.Debug.ContextInitialResult.ShowUnchanged")));
	}
}

void FCameraDirectorTreeDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	const FCameraDebugColors& Colors = FCameraDebugColors::Get();

	TArrayView<FCameraDebugBlock*> ChildrenView(GetChildren());

	const int32 MinNum = FMath::Min(ChildrenView.Num(), CameraDirectors.Num());

	Renderer.SetTextColor(Colors.Hightlighted);
	Renderer.AddText("Inactive Directors\n\n");
	Renderer.SetTextColor(Colors.Default);
	Renderer.AddIndent();

	for (int32 Index = 0; Index < MinNum; ++Index)
	{
		if (Index == CameraDirectors.Num() - 1)
		{
			Renderer.RemoveIndent();
			Renderer.NewLine();

			Renderer.SetTextColor(Colors.Notice);
			Renderer.AddText("Active Director\n\n");
			Renderer.SetTextColor(Colors.Default);
			Renderer.AddIndent();
		}

		const FDirectorDebugInfo& EntryDebugInfo(CameraDirectors[Index]);
		Renderer.AddText(TEXT("Camera asset: {cam_notice}%s{cam_default}\n"), *EntryDebugInfo.CameraAssetName);

		Renderer.AddIndent();
		ChildrenView[Index]->DebugDraw(Params, Renderer);
		Renderer.RemoveIndent();

		Renderer.NewLine();
	}

	Renderer.RemoveIndent();
	Renderer.SetTextColor(Colors.Default);

	Renderer.SkipAllBlocks();
}

void FCameraDirectorTreeDebugBlock::OnSerialize(FArchive& Ar)
{
	 Ar << CameraDirectors;
}

FArchive& operator<< (FArchive& Ar, FCameraDirectorTreeDebugBlock::FDirectorDebugInfo& DirectorDebugInfo)
{
	Ar << DirectorDebugInfo.CameraAssetName;
	return Ar;
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

