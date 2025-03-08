// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraShakeAsset.h"

#include "Build/CameraBuildLog.h"
#include "Build/CameraShakeAssetBuilder.h"
#include "Core/ShakeCameraNode.h"
#include "UObject/AssetRegistryTagsContext.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraShakeAsset)

void UCameraShakeAsset::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject | RF_NeedLoad | RF_WasLoaded) && 
			!Guid.IsValid())
	{
		Guid = FGuid::NewGuid();
	}
}

void UCameraShakeAsset::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

	if (DuplicateMode == EDuplicateMode::Normal)
	{
		Guid = FGuid::NewGuid();
	}
}

void UCameraShakeAsset::BuildCameraShake()
{
	using namespace UE::Cameras;

	FCameraBuildLog BuildLog;
	BuildLog.SetForwardMessagesToLogging(true);
	BuildCameraShake(BuildLog);
}

void UCameraShakeAsset::BuildCameraShake(UE::Cameras::FCameraBuildLog& InBuildLog)
{
	using namespace UE::Cameras;

	FCameraShakeAssetBuilder Builder(InBuildLog);
	Builder.BuildCameraShake(this);
}

UCameraNode* UCameraShakeAsset::GetRootNode()
{
	return RootNode;
}

void UCameraShakeAsset::DirtyBuildStatus()
{
	BuildStatus = ECameraBuildStatus::Dirty;
}

void UCameraShakeAsset::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
#if WITH_EDITOR

	const bool bIsUserObject = !HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject);
	const bool bIsEditorAutoSave = ((ObjectSaveContext.GetSaveFlags() & SAVE_FromAutosave) != 0);
	if (bIsUserObject && !bIsEditorAutoSave)
	{
		// Build when saving/cooking.
		BuildCameraShake();
	}

#endif

	Super::PreSave(ObjectSaveContext);
}

#if WITH_EDITOR

void UCameraShakeAsset::GetGraphNodePosition(FName InGraphName, int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePos.X;
	NodePosY = GraphNodePos.Y;
}

void UCameraShakeAsset::OnGraphNodeMoved(FName InGraphName, int32 NodePosX, int32 NodePosY, bool bMarkDirty)
{
	Modify(bMarkDirty);

	GraphNodePos.X = NodePosX;
	GraphNodePos.Y = NodePosY;
}

EObjectTreeGraphObjectSupportFlags UCameraShakeAsset::GetSupportFlags(FName InGraphName) const
{
	return EObjectTreeGraphObjectSupportFlags::CommentText;
}

const FString& UCameraShakeAsset::GetGraphNodeCommentText(FName InGraphName) const
{
	return GraphNodeComment;
}

void UCameraShakeAsset::OnUpdateGraphNodeCommentText(FName InGraphName, const FString& NewComment)
{
	Modify();

	GraphNodeComment = NewComment;
}

void UCameraShakeAsset::GetGraphNodeName(FName InGraphName, FText& OutName) const
{
	OutName = FText::FromString(GetName());
}

void UCameraShakeAsset::GetConnectableObjects(FName InGraphName, TSet<UObject*>& OutObjects) const
{
	OutObjects.Append(AllNodeObjects);
}

void UCameraShakeAsset::AddConnectableObject(FName InGraphName, UObject* InObject)
{
	using namespace UE::Cameras;

	Modify();

	const int32 Index = AllNodeObjects.AddUnique(InObject);
	ensure(Index == AllNodeObjects.Num() - 1);
}

void UCameraShakeAsset::RemoveConnectableObject(FName InGraphName, UObject* InObject)
{
	using namespace UE::Cameras;

	Modify();

	const int32 NumRemoved = AllNodeObjects.Remove(InObject);
	ensure(NumRemoved == 1);
}

#endif  // WITH_EDITOR

