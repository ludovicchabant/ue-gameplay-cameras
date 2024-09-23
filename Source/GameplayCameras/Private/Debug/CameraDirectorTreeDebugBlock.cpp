// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDirectorTreeDebugBlock.h"

#include "Core/CameraAsset.h"
#include "Core/CameraDirector.h"
#include "Core/CameraDirectorEvaluator.h"
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
		InitializeEntry(Context, EntryDebugInfo, Builder);
		CameraDirectors.Add(EntryDebugInfo);
	}
}

void FCameraDirectorTreeDebugBlock::Initialize(TArrayView<const TSharedPtr<FCameraEvaluationContext>> Contexts, FCameraDebugBlockBuilder& Builder)
{
	for (TSharedPtr<FCameraEvaluationContext> Context : Contexts)
	{
		FDirectorDebugInfo EntryDebugInfo;
		InitializeEntry(Context, EntryDebugInfo, Builder);
		CameraDirectors.Add(EntryDebugInfo);
	}
}

void FCameraDirectorTreeDebugBlock::InitializeEntry(TSharedPtr<FCameraEvaluationContext> Context, FDirectorDebugInfo& EntryDebugInfo, FCameraDebugBlockBuilder& Builder)
{
	if (Context)
	{
		const FCameraObjectTypeRegistry& TypeRegistry = FCameraObjectTypeRegistry::Get();
		const FName ContextTypeName = TypeRegistry.GetTypeNameSafe(Context->GetTypeID());

		const UObject* ContextOwner = Context->GetOwner();
		const FCameraDirectorEvaluator* DirectorEvaluator = Context->GetDirectorEvaluator();

		EntryDebugInfo.ContextClassName = ContextTypeName;
		EntryDebugInfo.OwnerName = *GetPathNameSafe(ContextOwner);
		EntryDebugInfo.OwnerClassName = ContextOwner ? ContextOwner->GetClass()->GetFName() : NAME_None;
		EntryDebugInfo.CameraAssetName = GetNameSafe(Context->GetCameraAsset());
		EntryDebugInfo.CameraDirectorClassName = DirectorEvaluator->GetCameraDirector()->GetFName();
		EntryDebugInfo.InitialContextTransform = Context->GetInitialResult().CameraPose.GetTransform();
		EntryDebugInfo.bIsValid = true;

		const FCameraNodeEvaluationResult& InitialResult = Context->GetInitialResult();
		AddChild(&Builder.BuildDebugBlock<FCameraPoseDebugBlock>(InitialResult.CameraPose)
				.WithShowUnchangedCVar(TEXT("GameplayCameras.Debug.ContextInitialResult.ShowUnchanged")));

		TArrayView<const TSharedPtr<FCameraEvaluationContext>> ChildrenContexts = Context->GetChildrenContexts();
		if (ChildrenContexts.Num() > 0)
		{
			FCameraDirectorTreeDebugBlock& ChildBlock = Builder.StartChildDebugBlock<FCameraDirectorTreeDebugBlock>();
			{
				ChildBlock.Initialize(ChildrenContexts, Builder);
			}
			Builder.EndChildDebugBlock();
		}
	}
	else
	{
		EntryDebugInfo.bIsValid = false;

		// Dummy debug block.
		AddChild(&Builder.BuildDebugBlock<FCameraDebugBlock>());
	}
}

void FCameraDirectorTreeDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	const FCameraDebugColors& Colors = FCameraDebugColors::Get();

	TArrayView<FCameraDebugBlock*> ChildrenView(GetChildren());

	const int32 MinNum = FMath::Min(ChildrenView.Num(), CameraDirectors.Num());

	Renderer.SetTextColor(Colors.Notice);
	Renderer.AddText("Inactive Directors\n");
	Renderer.SetTextColor(Colors.Default);
	Renderer.AddIndent();

	for (int32 Index = 0; Index < MinNum; ++Index)
	{
		if (Index == CameraDirectors.Num() - 1)
		{
			Renderer.RemoveIndent();

			Renderer.SetTextColor(Colors.Notice);
			Renderer.AddText("Active Director\n");
			Renderer.SetTextColor(Colors.Default);
			Renderer.AddIndent();
		}

		Renderer.AddText(TEXT("{cam_passive}[%d]{cam_default} "), Index + 1);

		const FDirectorDebugInfo& EntryDebugInfo(CameraDirectors[Index]);
		if (EntryDebugInfo.bIsValid)
		{
			Renderer.AddText(TEXT("{cam_passive}[%s]{cam_default}"), 
					*EntryDebugInfo.CameraDirectorClassName.ToString());
			Renderer.AddIndent();
			{
				Renderer.AddText(TEXT("Context {cam_passive}[%s]{cam_default}\n"), *EntryDebugInfo.ContextClassName.ToString());

				Renderer.AddText(TEXT("Owned by {cam_passive}[%s]{cam_default}\n"), *EntryDebugInfo.OwnerClassName.ToString());
				Renderer.AddIndent();
				{
					Renderer.AddText(*EntryDebugInfo.OwnerName);
				}
				Renderer.RemoveIndent();

				Renderer.AddText(TEXT("{cam_passive}From camera asset {cam_notice}%s{cam_default}\n"), 
						*EntryDebugInfo.CameraAssetName);
			}
			Renderer.RemoveIndent();

			Renderer.DrawCoordinateSystem(EntryDebugInfo.InitialContextTransform);
		}
		else
		{
			Renderer.AddText(TEXT("{cam_error}Invalid context!{cam_default}\n"));
		}

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
	Ar << DirectorDebugInfo.ContextClassName;
	Ar << DirectorDebugInfo.OwnerClassName;
	Ar << DirectorDebugInfo.OwnerName;
	Ar << DirectorDebugInfo.CameraAssetName;
	Ar << DirectorDebugInfo.CameraDirectorClassName;
	Ar << DirectorDebugInfo.InitialContextTransform;
	Ar << DirectorDebugInfo.bIsValid;
	return Ar;
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

