// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAsset.h"

#include "Core/CameraAsset.h"
#include "Core/CameraBuildLog.h"
#include "Core/CameraNode.h"
#include "Core/CameraRigAssetBuilder.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigAsset)

void FCameraRigAllocationInfo::Append(const FCameraRigAllocationInfo& OtherAllocationInfo)
{
	const FCameraNodeEvaluatorAllocationInfo& OtherEvaluatorInfo(OtherAllocationInfo.EvaluatorInfo);
	EvaluatorInfo.MaxAlignof = FMath::Max(EvaluatorInfo.MaxAlignof, OtherEvaluatorInfo.MaxAlignof);
	EvaluatorInfo.TotalSizeof = Align(EvaluatorInfo.TotalSizeof, OtherEvaluatorInfo.MaxAlignof) + OtherEvaluatorInfo.TotalSizeof;

	const FCameraVariableTableAllocationInfo& OtherVariableTableInfo(OtherAllocationInfo.VariableTableInfo);
	VariableTableInfo.AutoResetVariables.Append(OtherVariableTableInfo.AutoResetVariables);
	VariableTableInfo.VariableDefinitions.Append(OtherVariableTableInfo.VariableDefinitions);
}

bool operator==(const FCameraRigAllocationInfo& A, const FCameraRigAllocationInfo& B)
{
	return A.EvaluatorInfo == B.EvaluatorInfo
		&& A.VariableTableInfo == B.VariableTableInfo;
}

#if WITH_EDITOR

void UCameraRigInterfaceParameter::GetGraphNodePosition(FName InGraphName, int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePos.X;
	NodePosY = GraphNodePos.Y;
}

void UCameraRigInterfaceParameter::OnGraphNodeMoved(FName InGraphName, int32 NodePosX, int32 NodePosY, bool bMarkDirty)
{
	Modify(bMarkDirty);

	GraphNodePos.X = NodePosX;
	GraphNodePos.Y = NodePosY;
}

#endif

void UCameraRigInterfaceParameter::PostLoad()
{
	if (!Guid.IsValid())
	{
		Guid = FGuid::NewGuid();
	}

	Super::PostLoad();
}

void UCameraRigInterfaceParameter::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject | RF_NeedLoad | RF_WasLoaded) && 
			!Guid.IsValid())
	{
		Guid = FGuid::NewGuid();
	}
}

void UCameraRigInterfaceParameter::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

	if (DuplicateMode == EDuplicateMode::Normal)
	{
		Guid = FGuid::NewGuid();
	}
}

UCameraRigInterfaceParameter* FCameraRigInterface::FindInterfaceParameterByName(const FString& ParameterName) const
{
	const TObjectPtr<UCameraRigInterfaceParameter>* FoundItem = InterfaceParameters.FindByPredicate(
			[&ParameterName](UCameraRigInterfaceParameter* Item)
			{
				return Item->InterfaceParameterName == ParameterName;
			});
	return FoundItem ? *FoundItem : nullptr;
}

UCameraRigInterfaceParameter* FCameraRigInterface::FindInterfaceParameterByGuid(const FGuid& ParameterGuid) const
{
	const TObjectPtr<UCameraRigInterfaceParameter>* FoundItem = InterfaceParameters.FindByPredicate(
			[&ParameterGuid](UCameraRigInterfaceParameter* Item)
			{
				return Item->Guid == ParameterGuid;
			});
	return FoundItem ? *FoundItem : nullptr;
}

bool FCameraRigInterface::HasInterfaceParameter(const FString& ParameterName) const
{
	return FindInterfaceParameterByName(ParameterName) != nullptr;
}

const FName UCameraRigAsset::NodeTreeGraphName(TEXT("NodeTree"));
const FName UCameraRigAsset::TransitionsGraphName(TEXT("Transitions"));

void UCameraRigAsset::PostLoad()
{
#if WITH_EDITOR

	UCameraAsset* OuterCameraAsset = GetTypedOuter<UCameraAsset>();
	if (OuterCameraAsset && !HasAllFlags(RF_Public | RF_Transactional))
	{
		Modify();

		SetFlags(RF_Public | RF_Transactional);
	}

#endif

#if WITH_EDITORONLY_DATA

	if (GraphNodePosX_DEPRECATED != 0 || GraphNodePosY_DEPRECATED != 0)
	{
		NodeGraphNodePos = FIntVector2(GraphNodePosX_DEPRECATED, GraphNodePosY_DEPRECATED);

		GraphNodePosX_DEPRECATED = 0;
		GraphNodePosY_DEPRECATED = 0;
	}

#endif

	if (!Guid.IsValid())
	{
		Guid = FGuid::NewGuid();
	}

	Super::PostLoad();
}

void UCameraRigAsset::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject | RF_NeedLoad | RF_WasLoaded) && 
			!Guid.IsValid())
	{
		Guid = FGuid::NewGuid();
	}
}

void UCameraRigAsset::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

	if (DuplicateMode == EDuplicateMode::Normal)
	{
		Guid = FGuid::NewGuid();
	}
}

void UCameraRigAsset::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GameplayTags);
}

FString UCameraRigAsset::GetDisplayName() const
{
	if (!Interface.DisplayName.IsEmpty())
	{
		return Interface.DisplayName;
	}
	return GetName();
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

	if (GetOutermostObject() == this &&
			!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		// Build when saving/cooking, if we are a standalone camera rig (i.e. not a camera rig inside a 
		// camera asset, since those are built along with the camera asset).
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
			if (CurrentChild)
			{
				NodeStack.Add(CurrentChild);
			}
		}
	}
}

void UCameraRigAsset::GetGraphNodePosition(FName InGraphName, int32& NodePosX, int32& NodePosY) const
{
	if (InGraphName == NodeTreeGraphName)
	{
		NodePosX = NodeGraphNodePos.X;
		NodePosY = NodeGraphNodePos.Y;
	}
	else if (InGraphName == TransitionsGraphName)
	{
		NodePosX = TransitionGraphNodePos.X;
		NodePosY = TransitionGraphNodePos.Y;
	}
}

void UCameraRigAsset::OnGraphNodeMoved(FName InGraphName, int32 NodePosX, int32 NodePosY, bool bMarkDirty)
{
	Modify(bMarkDirty);

	if (InGraphName == NodeTreeGraphName)
	{
		NodeGraphNodePos.X = NodePosX;
		NodeGraphNodePos.Y = NodePosY;
	}
	else if (InGraphName == TransitionsGraphName)
	{
		TransitionGraphNodePos.X = NodePosX;
		TransitionGraphNodePos.Y = NodePosY;
	}
}

EObjectTreeGraphObjectSupportFlags UCameraRigAsset::GetSupportFlags(FName InGraphName) const
{
	if (UPackage* OuterPackage = Cast<UPackage>(GetOuter()))
	{
		// Can't rename a standalone rig prefab -- you have to rename it in the content browser.
		return EObjectTreeGraphObjectSupportFlags::CommentText;
	}
	else
	{
		return EObjectTreeGraphObjectSupportFlags::CommentText | EObjectTreeGraphObjectSupportFlags::CustomRename;
	}
}

const FString& UCameraRigAsset::GetGraphNodeCommentText(FName InGraphName) const
{
	if (InGraphName == NodeTreeGraphName)
	{
		return NodeGraphNodeComment;
	}
	else if (InGraphName == TransitionsGraphName)
	{
		return TransitionGraphNodeComment;
	}

	static FString InvalidString;
	return InvalidString;
}

void UCameraRigAsset::OnUpdateGraphNodeCommentText(FName InGraphName, const FString& NewComment)
{
	Modify();

	if (InGraphName == NodeTreeGraphName)
	{
		NodeGraphNodeComment = NewComment;
	}
	else if (InGraphName == TransitionsGraphName)
	{
		TransitionGraphNodeComment = NewComment;
	}
}

void UCameraRigAsset::GetGraphNodeName(FName InGraphName, FText& OutName) const
{
	OutName = FText::FromString(GetDisplayName());
}

void UCameraRigAsset::OnRenameGraphNode(FName InGraphName, const FString& NewName)
{
	Interface.DisplayName = NewName;
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

