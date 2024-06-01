// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraAsset.h"

#include "Core/CameraAssetBuilder.h"
#include "Core/CameraBuildLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraAsset)

const FName UCameraAsset::SharedTransitionsGraphName("SharedTransitions");

void UCameraAsset::BuildCamera()
{
	using namespace UE::Cameras;

	FCameraBuildLog BuildLog;
	BuildLog.SetForwardMessagesToLogging(true);
	BuildCamera(BuildLog);
}

void UCameraAsset::BuildCamera(UE::Cameras::FCameraBuildLog& InBuildLog)
{
	using namespace UE::Cameras;

	FCameraAssetBuilder Builder(InBuildLog);
	Builder.BuildCamera(this);
}

void UCameraAsset::DirtyBuildStatus()
{
	BuildStatus = ECameraBuildStatus::Dirty;
}

#if WITH_EDITOR

void UCameraAsset::GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePosX;
	NodePosY = GraphNodePosY;
}

void UCameraAsset::OnGraphNodeMoved(int32 NodePosX, int32 NodePosY, bool bMarkDirty)
{
	GraphNodePosX = NodePosX;
	GraphNodePosY = NodePosY;
}

const FString& UCameraAsset::GetGraphNodeCommentText() const
{
	return GraphNodeComment;
}

void UCameraAsset::OnUpdateGraphNodeCommentText(const FString& NewComment)
{
	GraphNodeComment = NewComment;
}

void UCameraAsset::GetConnectableObjects(FName InGraphName, TSet<UObject*>& OutObjects) const
{
	OutObjects.Append(AllSharedTransitionsObjects);
}

void UCameraAsset::AddConnectableObject(FName InGraphName, UObject* InObject)
{
	Modify();

	const int32 Index = AllSharedTransitionsObjects.AddUnique(InObject);
	ensure(Index == AllSharedTransitionsObjects.Num() - 1);
}

void UCameraAsset::RemoveConnectableObject(FName InGraphName, UObject* InObject)
{
	Modify();

	const int32 NumRemoved = AllSharedTransitionsObjects.Remove(InObject);
	ensure(NumRemoved == 1);
}

#endif  // WITH_EDITOR

