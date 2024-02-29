// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDebugBlock.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

TArray<FCameraDebugBlockField*> FCameraDebugBlock::StaticFields;

void FCameraDebugBlock::AddChild(FCameraDebugBlock* InChild)
{
	Children.Add(InChild);
}

void FCameraDebugBlock::DebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	EDebugDrawResult Result = OnDebugDraw(Params, Renderer);
	if (!EnumHasAnyFlags(Result, EDebugDrawResult::SkipChildren))
	{
		for (FCameraDebugBlock* Child : Children)
		{
			Child->DebugDraw(Params, Renderer);
		}
	}
	OnPostDebugDraw(Params, Renderer);
}

EDebugDrawResult FCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	return EDebugDrawResult::Default;
}

void FCameraDebugBlock::Serialize(FArchive& Ar)
{
	for (FCameraDebugBlockField* StaticField : StaticFields)
	{
		StaticField->SerializeField(this, Ar);
	}

	OnSerialize(Ar);
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

