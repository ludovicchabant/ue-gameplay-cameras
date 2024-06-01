// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAsset.h"

#include "Core/CameraBuildLog.h"
#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAssetBuilder.h"
#include "Core/CameraVariableAssets.h"
#include "Templates/UnrealTypeTraits.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigAsset)

#if WITH_EDITOR

void UCameraRigInterfaceParameter::GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePosX;
	NodePosY = GraphNodePosY;
}

void UCameraRigInterfaceParameter::OnGraphNodeMoved(int32 NodePosX, int32 NodePosY, bool bMarkDirty)
{
	GraphNodePosX = NodePosX;
	GraphNodePosY = NodePosY;
}

#endif

UCameraRigInterfaceParameter* FCameraRigInterface::FindInterfaceParameterByName(const FString& ParameterName) const
{
	const TObjectPtr<UCameraRigInterfaceParameter>* FoundItem = InterfaceParameters.FindByPredicate([&ParameterName](UCameraRigInterfaceParameter* Item)
			{
				return Item->InterfaceParameterName == ParameterName;
			});
	return FoundItem ? *FoundItem : nullptr;
}

bool FCameraRigInterface::HasInterfaceParameter(const FString& ParameterName) const
{
	return FindInterfaceParameterByName(ParameterName) != nullptr;
}

const FName UCameraRigAsset::NodeTreeGraphName(TEXT("NodeTree"));
const FName UCameraRigAsset::TransitionsGraphName(TEXT("Transitions"));

void UCameraRigAsset::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GameplayTags);
}

void UCameraRigAsset::BuildCameraRig()
{
	using namespace UE::Cameras;

	FCameraBuildLog BuildLog;
	BuildLog.SetForwardMessagesToLogging(true);
	BuildCameraRig(BuildLog);
}

void UCameraRigAsset::BuildCameraRig(UE::Cameras::FCameraBuildLog& InBuildLog)
{
	using namespace UE::Cameras;

	FCameraRigAssetBuilder Builder(InBuildLog);
	Builder.BuildCameraRig(this);
}

void UCameraRigAsset::DirtyBuildStatus()
{
	BuildStatus = ECameraBuildStatus::Dirty;
}

void UCameraRigAsset::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
#if WITH_EDITOR
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		// Build on save.
		BuildCameraRig();
	}
#endif

	Super::PreSave(ObjectSaveContext);
}

#if WITH_EDITOR

void UCameraRigAsset::GatherPackages(FCameraRigPackages& OutPackages) const
{
	TArray<UCameraNode*> NodeStack;
	if (RootNode)
	{
		NodeStack.Add(RootNode);
	}
	while (!NodeStack.IsEmpty())
	{
		UCameraNode* CurrentNode = NodeStack.Pop();
		const UPackage* CurrentPackage = CurrentNode->GetOutermost();
		OutPackages.AddUnique(CurrentPackage);

		FCameraNodeChildrenView CurrentChildren = CurrentNode->GetChildren();
		for (UCameraNode* CurrentChild : ReverseIterate(CurrentChildren))
		{
			NodeStack.Add(CurrentChild);
		}
	}
}

void UCameraRigAsset::GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePosX;
	NodePosY = GraphNodePosY;
}

void UCameraRigAsset::OnGraphNodeMoved(int32 NodePosX, int32 NodePosY, bool bMarkDirty)
{
	GraphNodePosX = NodePosX;
	GraphNodePosY = NodePosY;
}

const FString& UCameraRigAsset::GetGraphNodeCommentText() const
{
	return GraphNodeComment;
}

void UCameraRigAsset::OnUpdateGraphNodeCommentText(const FString& NewComment)
{
	GraphNodeComment = NewComment;
}

void UCameraRigAsset::GetConnectableObjects(FName InGraphName, TSet<UObject*>& OutObjects) const
{
	if (InGraphName == NodeTreeGraphName)
	{
		OutObjects.Append(AllNodeTreeObjects);
	}
	else if (InGraphName == TransitionsGraphName)
	{
		OutObjects.Append(AllTransitionsObjects);
	}
}

void UCameraRigAsset::AddConnectableObject(FName InGraphName, UObject* InObject)
{
	Modify();

	if (InGraphName == NodeTreeGraphName)
	{
		const int32 Index = AllNodeTreeObjects.AddUnique(InObject);
		ensure(Index == AllNodeTreeObjects.Num() - 1);
	}
	else if (InGraphName == TransitionsGraphName)
	{
		const int32 Index = AllTransitionsObjects.AddUnique(InObject);
		ensure(Index == AllTransitionsObjects.Num() - 1);
	}
}

void UCameraRigAsset::RemoveConnectableObject(FName InGraphName, UObject* InObject)
{
	Modify();

	if (InGraphName == NodeTreeGraphName)
	{
		const int32 NumRemoved = AllNodeTreeObjects.Remove(InObject);
		ensure(NumRemoved == 1);
	}
	else if (InGraphName == TransitionsGraphName)
	{
		const int32 NumRemoved = AllTransitionsObjects.Remove(InObject);
		ensure(NumRemoved == 1);
	}
}

#endif  // WITH_EDITOR
