// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraBuildStatus.h"
#include "Core/CameraRigTransition.h"
#include "Core/ObjectTreeGraphObject.h"
#include "Core/ObjectTreeGraphRootObject.h"
#include "CoreTypes.h"
#include "UObject/ObjectPtr.h"

#include "CameraAsset.generated.h"

class UCameraDirector;
class UCameraRigAsset;

namespace UE::Cameras { class FCameraBuildLog; }

/**
 * A complete camera asset.
 */
UCLASS(MinimalAPI)
class UCameraAsset 
	: public UObject
	, public IHasCameraBuildStatus
	, public IObjectTreeGraphObject
	, public IObjectTreeGraphRootObject
{
	GENERATED_BODY()

public:

	/** The camera director to use in this camera. */
	UPROPERTY(Instanced)
	TObjectPtr<UCameraDirector> CameraDirector;

	/** The list of camera rigs used by this camera. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraRigAsset>> CameraRigs;

	/** A list of default enter transitions for all the camera rigs in this asset. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraRigTransition>> EnterTransitions;

	/** A list of default exit transitions for all the camera rigs in this asset. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraRigTransition>> ExitTransitions;

public:

	/** The current build state of this camera asset. */
	UPROPERTY(Transient)
	ECameraBuildStatus BuildStatus = ECameraBuildStatus::Dirty;

	/**
	 * Builds and validates this camera, including all its camera rigs.
	 * Errors and warnings will go to the console.
	 */
	GAMEPLAYCAMERAS_API void BuildCamera();

	/**
	 * Builds and validates this camera, including all its camera rigs.
	 * Errors and warnings will go to the provided build log.
	 */
	GAMEPLAYCAMERAS_API void BuildCamera(UE::Cameras::FCameraBuildLog& InBuildLog);

public:

	// Graph names for ObjectTreeGraph API.
	GAMEPLAYCAMERAS_API static const FName SharedTransitionsGraphName;

	// IHasCameraBuildStatus interface.
	virtual ECameraBuildStatus GetBuildStatus() const override { return BuildStatus; }
	virtual void DirtyBuildStatus() override;

protected:

	// IObjectTreeGraphObject interface.
#if WITH_EDITOR
	virtual void GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const override;
	virtual void OnGraphNodeMoved(int32 NodePosX, int32 NodePosY, bool bMarkDirty) override;
	virtual EObjectTreeGraphObjectSupportFlags GetSupportFlags() const override { return EObjectTreeGraphObjectSupportFlags::CommentText; }
	virtual const FString& GetGraphNodeCommentText() const override;
	virtual void OnUpdateGraphNodeCommentText(const FString& NewComment) override;
#endif

	// IObjectTreeGraphRootObject interface.
#if WITH_EDITOR
	virtual void GetConnectableObjects(FName InGraphName, TSet<UObject*>& OutObjects) const override;
	virtual void AddConnectableObject(FName InGraphName, UObject* InObject) override;
	virtual void RemoveConnectableObject(FName InGraphName, UObject* InObject) override;
#endif

private:

#if WITH_EDITORONLY_DATA

	/** Position of the camera node in the transition editor. */
	UPROPERTY()
	int32 GraphNodePosX = 0;

	/** Position of the camera node in the graph editor. */
	UPROPERTY()
	int32 GraphNodePosY = 0;

	/** User-written comment in the graph editor. */
	UPROPERTY()
	FString GraphNodeComment;

	/**
	 * Similar to AllNodeTreeObjects, but for the transitions graph.
	 */
	UPROPERTY(Instanced, meta=(ObjectTreeGraphHidden=true))
	TArray<TObjectPtr<UObject>> AllSharedTransitionsObjects;

#endif  // WITH_EDITORONLY_DATA
};

