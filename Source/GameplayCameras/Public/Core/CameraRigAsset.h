// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluatorFwd.h"
#include "Core/CameraRigTransition.h"
#include "Core/CameraVariableTableFwd.h"
#include "Core/ObjectTreeGraphObject.h"
#include "CoreTypes.h"
#include "UObject/ObjectPtr.h"

#include "CameraRigAsset.generated.h"

class UCameraNode;

/**
 *
 */
UENUM()
enum class ECameraRigBuildStatus : uint8
{
	Clean,
	CleanWithWarnings,
	WithErrors,
	Dirty
};

/**
 * Structure describing various allocations needed by a camera node.
 */
USTRUCT()
struct FCameraRigAllocationInfo
{
	GENERATED_BODY()

	/** Allocation info for node evaluators. */
	UPROPERTY()
	FCameraNodeEvaluatorAllocationInfo EvaluatorInfo;

	/** Allocation info for the camera variable. */
	UPROPERTY()
	FCameraVariableTableAllocationInfo VariableTableInfo;
};

/**
 * List of packages that contain the definition of a camera rig.
 * In most cases there's only one, but with nested assets there could be more.
 */
using FCameraRigPackages = TArray<const UPackage*, TInlineAllocator<4>>;

/**
 * A camera rig asset, which runs a hierarchy of camera nodes to drive 
 * the behavior of a camera.
 */
UCLASS(MinimalAPI)
class UCameraRigAsset
	: public UObject
	, public IObjectTreeGraphObject
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	GAMEPLAYCAMERAS_API void GatherPackages(FCameraRigPackages& OutPackages) const;
#endif  // WITH_EDITOR

public:

	/** Root camera node. */
	UPROPERTY(EditAnywhere, Instanced, Category=Common)
	TObjectPtr<UCameraNode> RootNode;

	/** List of enter transitions for this camera rig. */
	UPROPERTY(EditAnywhere, Category=Blending)
	TArray<TObjectPtr<UCameraRigTransition>> EnterTransitions;

	/** List of exist transitions for this camera rig. */
	UPROPERTY(EditAnywhere, Category=Blending)
	TArray<TObjectPtr<UCameraRigTransition>> ExitTransitions;

	UPROPERTY()
	FCameraRigAllocationInfo AllocationInfo;

public:

	UPROPERTY(Transient)
	ECameraRigBuildStatus BuildStatus = ECameraRigBuildStatus::Dirty;

protected:

	// IObjectTreeGraphObject interface.
#if WITH_EDITOR
	virtual void GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const override;
	virtual void OnGraphNodeMoved(int32 NodePosX, int32 NodePosY) override;
	virtual EObjectTreeGraphObjectSupportFlags GetSupportFlags() const override { return EObjectTreeGraphObjectSupportFlags::CommentText; }
	virtual const FString& GetGraphNodeCommentText() const override;
	virtual void OnUpdateGraphNodeCommentText(const FString& NewComment) override;
#endif

private:

#if WITH_EDITORONLY_DATA

	UPROPERTY()
	int32 GraphNodePosX = 0;

	UPROPERTY()
	int32 GraphNodePosY = 0;

	UPROPERTY()
	FString GraphNodeComment;

#endif  // WITH_EDITORONLY_DATA
};

