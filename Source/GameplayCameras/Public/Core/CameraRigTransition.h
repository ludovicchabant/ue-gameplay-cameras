// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Core/ObjectTreeGraphObject.h"
#include "CoreTypes.h"
#include "UObject/ObjectPtr.h"

#include "CameraRigTransition.generated.h"

class UBlendCameraNode;
class UCameraAsset;
class UCameraRigAsset;

/**
 * Parameter structure for camera transitions.
 */
struct FCameraRigTransitionConditionMatchParams
{
	/** The previous camera rig. */
	const UCameraRigAsset* FromCameraRig = nullptr;
	/** The previous camera asset. */
	const UCameraAsset* FromCameraAsset = nullptr;

	/** The next camera rig. */
	const UCameraRigAsset* ToCameraRig = nullptr;
	/** The next camera asset. */
	const UCameraAsset* ToCameraAsset = nullptr;
};

/**
 * Base class for a camera transition condition.
 */
UCLASS(Abstract, DefaultToInstanced, MinimalAPI, meta=(ObjectTreeGraphCategory="Transition Conditions"))
class UCameraRigTransitionCondition 
	: public UObject
	, public IObjectTreeGraphObject
{
	GENERATED_BODY()

public:

	/** Evaluates whether this transition should be used. */
	bool TransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const;

protected:

	/** Evaluates whether this transition should be used. */
	virtual bool OnTransitionMatches(const FCameraRigTransitionConditionMatchParams& Params) const { return false; }

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

private:

#if WITH_EDITORONLY_DATA

	/** Position of the transition condition node in the transition graph editor. */
	UPROPERTY()
	FIntVector2 GraphNodePos = FIntVector2::ZeroValue;

	/** User-written comment in the transition graph editor. */
	UPROPERTY()
	FString GraphNodeComment;


	// Deprecated properties.

	UPROPERTY()
	int32 GraphNodePosX_DEPRECATED = 0;
	UPROPERTY()
	int32 GraphNodePosY_DEPRECATED = 0;

#endif  // WITH_EDITORONLY_DATA
};

/**
 * A camera transition.
 */
UCLASS(MinimalAPI)
class UCameraRigTransition 
	: public UObject
	, public IObjectTreeGraphObject
{
	GENERATED_BODY()

public:

	/** The list of conditions that must pass for this transition to be used. */
	UPROPERTY(Instanced, meta=(ObjectTreeGraphPinDirection=Input))
	TArray<TObjectPtr<UCameraRigTransitionCondition>> Conditions;

	/** The blend to use to blend a given camera rig in or out. */
	UPROPERTY(Instanced)
	TObjectPtr<UBlendCameraNode> Blend;

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

private:

#if WITH_EDITORONLY_DATA

	/** Position of the transition node in the transition graph editor. */
	UPROPERTY()
	FIntVector2 GraphNodePos = FIntVector2::ZeroValue;

	/** User-written comment in the transition graph editor. */
	UPROPERTY()
	FString GraphNodeComment;


	// Deprecated properties.

	UPROPERTY()
	int32 GraphNodePosX_DEPRECATED = 0;
	UPROPERTY()
	int32 GraphNodePosY_DEPRECATED = 0;

#endif  // WITH_EDITORONLY_DATA
};

