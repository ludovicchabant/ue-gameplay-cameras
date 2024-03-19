// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumClassFlags.h"
#include "UObject/Interface.h"

#include "ObjectTreeGraphObject.generated.h"

UINTERFACE(MinimalAPI)
class UObjectTreeGraphObject : public UInterface
{
	GENERATED_BODY()
};

#if WITH_EDITOR

enum class EObjectTreeGraphObjectSupportFlags
{
	None,
	CommentText = 1 << 0,
	CustomRename = 1 << 1
};
ENUM_CLASS_FLAGS(EObjectTreeGraphObjectSupportFlags);

#endif  // WITH_EDITOR


class IObjectTreeGraphObject
{
	GENERATED_BODY()

#if WITH_EDITOR

public:

	virtual void GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const {}
	virtual void OnGraphNodeMoved(int32 NodePosX, int32 NodePosY) {}

	virtual EObjectTreeGraphObjectSupportFlags GetSupportFlags() const { return EObjectTreeGraphObjectSupportFlags::None; }
	bool HasSupportFlags(EObjectTreeGraphObjectSupportFlags InFlags) const { return EnumHasAllFlags(GetSupportFlags(), InFlags); }

	virtual const FString& GetGraphNodeCommentText() const { static const FString EmptyString; return EmptyString; }
	virtual void OnUpdateGraphNodeCommentText(const FString& NewComment) {}

	virtual const FString& GetGraphNodeName() const { static const FString EmptyString; return EmptyString; }
	virtual void OnRenameGraphNode(const FString& NewName) {}

#endif  // WITH_EDITOR
};

