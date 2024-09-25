// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraBuildStatus.h"
#include "Core/CameraEventHandler.h"
#include "Core/CameraRigTransition.h"
#include "Core/ObjectTreeGraphObject.h"
#include "Core/ObjectTreeGraphRootObject.h"
#include "CoreTypes.h"
#include "UObject/ObjectPtr.h"

#include "CameraAsset.generated.h"

class UCameraAsset;
class UCameraDirector;
class UCameraRigAsset;

namespace UE::Cameras
{
	class FCameraBuildLog;

	/**
	 * Interface for listening to changes on a camera asset.
	 */
	class ICameraAssetEventHandler
	{
	public:
		virtual ~ICameraAssetEventHandler() {}

		/** Called when the camera director has been changed. */
		virtual void OnCameraDirectorChanged(UCameraAsset* InCameraAsset, const TCameraPropertyChangedEvent<UCameraDirector*>& Event) {}
		/** Changed when the camera rigs have been changed. */
		virtual void OnCameraRigsChanged(UCameraAsset* InCameraAsset, const TCameraArrayChangedEvent<UCameraRigAsset*>& Event) {}
		/* Changed when the enter transitions have been changed. */
		virtual void OnEnterTransitionsChanged(UCameraAsset* InCameraAsset, const TCameraArrayChangedEvent<UCameraRigTransition*>& Event) {}
		/* Changed when the exit transitions have been changed. */
		virtual void OnExitTransitionsChanged(UCameraAsset* InCameraAsset, const TCameraArrayChangedEvent<UCameraRigTransition*>& Event) {}
	};
}

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

	/** Gets the camera director. */
	UCameraDirector* GetCameraDirector() const { return CameraDirector; }
	/** Sets the camera director. */
	GAMEPLAYCAMERAS_API void SetCameraDirector(UCameraDirector* InCameraDirector);

	/** Gets the camera rigs. */
	TArrayView<const TObjectPtr<UCameraRigAsset>> GetCameraRigs() const { return CameraRigs; }
	/** Adds a a camera rig. */
	GAMEPLAYCAMERAS_API void AddCameraRig(UCameraRigAsset* InCameraRig);
	/** Removes a camera rig. */
	GAMEPLAYCAMERAS_API int32 RemoveCameraRig(UCameraRigAsset* InCameraRig);

	/** Gets the enter transitions. */
	TArrayView<const TObjectPtr<UCameraRigTransition>> GetEnterTransitions() const { return EnterTransitions; }
	/** Adds an enter transition. */
	GAMEPLAYCAMERAS_API void AddEnterTransition(UCameraRigTransition* InTransition);
	/** Removes an enter transition. */
	GAMEPLAYCAMERAS_API int32 RemoveEnterTransition(UCameraRigTransition* InTransition);

	/** Gets the exit transitions. */
	TArrayView<const TObjectPtr<UCameraRigTransition>> GetExitTransitions() const { return ExitTransitions; }
	/** Adds an exit transition. */
	GAMEPLAYCAMERAS_API void AddExitTransition(UCameraRigTransition* InTransition);
	/** Removes an exit transition. */
	GAMEPLAYCAMERAS_API int32 RemoveExitTransition(UCameraRigTransition* InTransition);

public:

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

	/** Sets the build status. */
	void SetBuildStatus(ECameraBuildStatus InBuildStatus) { BuildStatus = InBuildStatus; }

protected:

	// IObjectTreeGraphObject interface.
#if WITH_EDITOR
	virtual void GetGraphNodePosition(FName InGraphName, int32& NodePosX, int32& NodePosY) const override;
	virtual void OnGraphNodeMoved(FName InGraphName, int32 NodePosX, int32 NodePosY, bool bMarkDirty) override;
	virtual EObjectTreeGraphObjectSupportFlags GetSupportFlags(FName InGraphName) const override { return EObjectTreeGraphObjectSupportFlags::CommentText; }
	virtual const FString& GetGraphNodeCommentText(FName InGraphName) const override;
	virtual void OnUpdateGraphNodeCommentText(FName InGraphName, const FString& NewComment) override;
#endif

	// IObjectTreeGraphRootObject interface.
#if WITH_EDITOR
	virtual void GetConnectableObjects(FName InGraphName, TSet<UObject*>& OutObjects) const override;
	virtual void AddConnectableObject(FName InGraphName, UObject* InObject) override;
	virtual void RemoveConnectableObject(FName InGraphName, UObject* InObject) override;
#endif

	// UObject interface.
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	/** Event handlers to be notified of data changes. */
	UE::Cameras::TCameraEventHandlerContainer<UE::Cameras::ICameraAssetEventHandler> EventHandlers;

private:

#if WITH_EDITOR
	void CleanUpStrayObjects();
#endif

private:

	/** The camera director to use in this camera. */
	UPROPERTY(Instanced)
	TObjectPtr<UCameraDirector> CameraDirector;

	/** The list of camera rigs used by this camera. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraRigAsset>> CameraRigs;

	/** A list of default enter transitions for all the camera rigs in this asset. */
	UPROPERTY(meta=(ObjectTreeGraphPinDirection=Input))
	TArray<TObjectPtr<UCameraRigTransition>> EnterTransitions;

	/** A list of default exit transitions for all the camera rigs in this asset. */
	UPROPERTY(meta=(ObjectTreeGraphPinDirection=Output))
	TArray<TObjectPtr<UCameraRigTransition>> ExitTransitions;

	/** The current build state of this camera asset. */
	UPROPERTY(Transient)
	ECameraBuildStatus BuildStatus = ECameraBuildStatus::Dirty;

#if WITH_EDITORONLY_DATA

	/** Position of the camera node in the shared transitions graph editor. */
	UPROPERTY()
	FIntVector2 TransitionGraphNodePos = FIntVector2::ZeroValue;

	/** User-written comment in the transition graph editor. */
	UPROPERTY()
	FString TransitionGraphNodeComment;

	/** All nodes used in the shared transitions graph editor. */
	UPROPERTY(Instanced, meta=(ObjectTreeGraphHidden=true))
	TArray<TObjectPtr<UObject>> AllSharedTransitionsObjects;

	// Only specified here so that the schema can use GET_MEMBER_NAME_CHECKED...
	friend class UCameraSharedTransitionGraphSchema;

#endif  // WITH_EDITORONLY_DATA
};

