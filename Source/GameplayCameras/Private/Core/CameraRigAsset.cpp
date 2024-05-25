// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAsset.h"

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

void UCameraRigAsset::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GameplayTags);
}

void UCameraRigAsset::BuildCameraRig()
{
	using namespace UE::Cameras;

	FCameraRigAssetBuilder Builder;
	Builder.BuildCameraRig(this);
}

void UCameraRigAsset::PostLoad()
{
#if WITH_EDITORONLY_DATA
	if (AllNodes.IsEmpty())
	{
		// Rebuild the AllNodes array from scratch if this is old data.
		TArray<UObject*> AllObjects;
		UPackage* Package = GetOutermost();
		GetObjectsWithPackage(Package, AllObjects);
		for (UObject* Obj : AllObjects)
		{
			if (UCameraNode* CameraNode = Cast<UCameraNode>(Obj))
			{
				AllNodes.Add(CameraNode);
			}
		}
	}
#endif  // WITH_EDITORONLY_DATA

	Super::PostLoad();
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

void UCameraRigAsset::AddConnectableObject(UObject* InObject)
{
	// Camera nodes and rig parameters are shown on the graph editor. Handle the former case. The latter are
	// already added to the interface struct even when not connected.
	if (UCameraNode* CameraNode = Cast<UCameraNode>(InObject))
	{
		Modify();
		const int32 Index = AllNodes.AddUnique(CameraNode);
		ensure(Index == AllNodes.Num() - 1);
	}
}

void UCameraRigAsset::RemoveConnectableObject(UObject* InObject)
{
	if (UCameraNode* CameraNode = Cast<UCameraNode>(InObject))
	{
		Modify();
		const int32 NumRemoved = AllNodes.Remove(CameraNode);
		ensure(NumRemoved == 1);
	}
}

#endif  // WITH_EDITOR
