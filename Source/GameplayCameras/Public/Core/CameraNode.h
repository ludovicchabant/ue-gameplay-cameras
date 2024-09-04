// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluatorBuilder.h"
#include "Core/ObjectChildrenView.h"
#include "Core/ObjectTreeGraphObject.h"
#include "CoreTypes.h"
#include "UObject/Object.h"

#include "CameraNode.generated.h"

namespace UE::Cameras
{
	class FCameraBuildLog;
	struct FCameraRigBuildContext;
}

/** View on a camera node's children. */
using FCameraNodeChildrenView = UE::Cameras::TObjectChildrenView<TObjectPtr<UCameraNode>>;

/**
 * The base class for a camera node.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, meta=(CameraNodeCategories="Miscellaneous"))
class GAMEPLAYCAMERAS_API UCameraNode 
	: public UObject
	, public IObjectTreeGraphObject
{
	GENERATED_BODY()

public:
	
	using FCameraBuildLog = UE::Cameras::FCameraBuildLog;
	using FCameraRigBuildContext = UE::Cameras::FCameraRigBuildContext;
	using FCameraNodeEvaluatorBuilder = UE::Cameras::FCameraNodeEvaluatorBuilder;

	/** Get the list of children under this node. */
	FCameraNodeChildrenView GetChildren();

	/** Optional build step executed at the beginning of the build process. */
	void PreBuild(FCameraBuildLog& BuildLog);

	/** Gets optional info about this node's required allocations at runtime. */
	void Build(FCameraRigBuildContext& BuildContext);

	/** Builds the evaluator for this node. */
	FCameraNodeEvaluatorPtr BuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const;

protected:

	/** Get the list of children under this node. */
	virtual FCameraNodeChildrenView OnGetChildren() { return FCameraNodeChildrenView(); }

	/** Optional build step executed at the beginning of the build process. */
	virtual void OnPreBuild(FCameraBuildLog& BuildLog) {}

	/** Gets optional info about this node's required allocations at runtime. */
	virtual void OnBuild(FCameraRigBuildContext& BuildContext) {}

	/** Builds the evaluator for this node. */
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const { return nullptr; }

protected:

	// UObject interface.
	virtual void PostLoad() override;

	// IObjectTreeGraphObject interface.
#if WITH_EDITOR
	virtual void GetGraphNodePosition(FName InGraphName, int32& NodePosX, int32& NodePosY) const override;
	virtual void OnGraphNodeMoved(FName InGraphName, int32 NodePosX, int32 NodePosY, bool bMarkDirty) override;
	virtual EObjectTreeGraphObjectSupportFlags GetSupportFlags(FName InGraphName) const override { return EObjectTreeGraphObjectSupportFlags::CommentText; }
	virtual const FString& GetGraphNodeCommentText(FName InGraphName) const override;
	virtual void OnUpdateGraphNodeCommentText(FName InGraphName, const FString& NewComment) override;
#endif

public:

	/** Specifies whether this node is enabled. */
	UPROPERTY(EditAnywhere, Category=Common)
	bool bIsEnabled = true;

#if WITH_EDITORONLY_DATA

	/** Position of the camera node in the node graph editor. */
	UPROPERTY()
	FIntVector2 GraphNodePos = FIntVector2::ZeroValue;

	/** User-written comment in the node graph editor. */
	UPROPERTY()
	FString GraphNodeComment;


	// Deprecated properties.

	UPROPERTY()
	int32 GraphNodePosX_DEPRECATED = 0;
	UPROPERTY()
	int32 GraphNodePosY_DEPRECATED = 0;

#endif  // WITH_EDITORONLY_DATA
};

