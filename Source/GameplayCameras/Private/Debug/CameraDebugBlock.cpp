// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDebugBlock.h"

#include "Debug/CameraDebugRenderer.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

bool FCameraDebugBlockDrawParams::IsCategoryActive(const FString& InCategory) const
{
	return ActiveCategories.Contains(InCategory);
}

UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(FCameraDebugBlock)

TArray<FCameraDebugBlockField*> FCameraDebugBlock::StaticFields;

void FCameraDebugBlock::Attach(FCameraDebugBlock* InAttachment)
{
	Attachments.Add(InAttachment);
}

void FCameraDebugBlock::AddChild(FCameraDebugBlock* InChild)
{
	Children.Add(InChild);
}


void FCameraDebugBlock::DebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	Renderer.ResetVisitFlags();

	OnDebugDraw(Params, Renderer);
	
	ECameraDebugDrawVisitFlags VisitFlags = Renderer.GetVisitFlags();

	if (!EnumHasAnyFlags(VisitFlags, ECameraDebugDrawVisitFlags::SkipAttachedBlocks) && !Attachments.IsEmpty())
	{
		for (FCameraDebugBlock* Attachment : Attachments)
		{
			Attachment->DebugDraw(Params, Renderer);
		}
	}

	if (!EnumHasAnyFlags(VisitFlags, ECameraDebugDrawVisitFlags::SkipChildrenBlocks) && !Children.IsEmpty())
	{
		Renderer.AddIndent();

		for (FCameraDebugBlock* Child : Children)
		{
			Child->DebugDraw(Params, Renderer);
		}

		Renderer.RemoveIndent();
	}

	OnPostDebugDraw(Params, Renderer);
}

void FCameraDebugBlock::Serialize(FArchive& Ar)
{
	OnSerialize(Ar);

	for (FCameraDebugBlockField* StaticField : StaticFields)
	{
		StaticField->SerializeField(this, Ar);
	}
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

