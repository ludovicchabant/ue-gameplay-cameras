// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluatorBuilder.h"
#include "Core/CameraVariableTableFwd.h"
#include "Core/ObjectChildrenView.h"
#include "Core/ObjectTreeGraphObject.h"
#include "CoreTypes.h"
#include "UObject/Object.h"

#include "CameraNode.generated.h"

struct FCameraRigAllocationInfo;

/** View on a camera node's children. */
using FCameraNodeChildrenView = UE::Cameras::TObjectChildrenView<TObjectPtr<UCameraNode>>;

/**
 * The base class for a camera node.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, meta=(ObjectTreeGraphCategory="Camera Nodes"))
class GAMEPLAYCAMERAS_API UCameraNode 
	: public UObject
	, public IObjectTreeGraphObject
{
	GENERATED_BODY()

public:
	
	using FCameraNodeEvaluatorBuilder = UE::Cameras::FCameraNodeEvaluatorBuilder;

	/** Get the list of children under this node. */
	FCameraNodeChildrenView GetChildren();

	/** Gets optional info about this node's required allocations at runtime. */
	void BuildAllocationInfo(FCameraRigAllocationInfo& AllocationInfo) const;

	/** Builds the evaluator for this node. */
	FCameraNodeEvaluatorPtr BuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const;

protected:

	/** Get the list of children under this node. */
	virtual FCameraNodeChildrenView OnGetChildren() { return FCameraNodeChildrenView(); }

	/** Gets optional info about this node's required allocations at runtime. */
	virtual void OnBuildAllocationInfo(FCameraRigAllocationInfo& AllocationInfo) const {}

	/** Builds the evaluator for this node. */
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const { return nullptr; }

protected:

	// IObjectTreeGraphObject interface.
#if WITH_EDITOR
	virtual void GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const override;
	virtual void OnGraphNodeMoved(int32 NodePosX, int32 NodePosY) override;
	virtual EObjectTreeGraphObjectSupportFlags GetSupportFlags() const override { return EObjectTreeGraphObjectSupportFlags::CommentText; }
	virtual const FString& GetGraphNodeCommentText() const override;
	virtual void OnUpdateGraphNodeCommentText(const FString& NewComment) override;
#endif

public:

	/** Specifies whether this node is enabled. */
	UPROPERTY(EditAnywhere, Category=Common)
	bool bIsEnabled = true;

#if WITH_EDITORONLY_DATA

	UPROPERTY()
	int32 GraphNodePosX = 0;

	UPROPERTY()
	int32 GraphNodePosY = 0;

	UPROPERTY()
	FString GraphNodeComment;

#endif  // WITH_EDITORONLY_DATA
};

