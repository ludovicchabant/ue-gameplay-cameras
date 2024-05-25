// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Core/CameraNodeEvaluatorFwd.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableTableFwd.h"
#include "CoreTypes.h"
#include "Logging/TokenizedMessage.h"
#include "Templates/Tuple.h"
#include "UObject/WeakObjectPtr.h"

class FStructProperty;
class UCameraNode;
class UCameraRigAsset;
class UCameraVariableAsset;
struct FCameraVariableTableAllocationInfo;

namespace UE::Cameras
{

namespace Internal { struct FPrivateVariableBuilder; }

/**
 * A message emitted by the camera rig asset builder.
 */
struct GAMEPLAYCAMERAS_API FCameraRigAssetBuildLogMessage
{
	/** Severity of the message. */
	EMessageSeverity::Type Severity = EMessageSeverity::Info;
	/** An optional object that the message relates to. */
	UObject* Object = nullptr;
	/** The actual message. */
	FText Text;

	/** Generates a plain string representation of this message. */
	FString ToString() const;
	/** Sends a string version of this message to the LogCameraSystem console log. */
	void SendToLogging(const FString& InLoggingPrefix) const;
};

/**
 * Build log, populated when building a camera rig asset.
 */
class GAMEPLAYCAMERAS_API FCameraRigAssetBuildLog
{
public:

	/**
	 * Sets a string that will be prefixed to all messages sent to the console.
	 * Only useful when IsForwardingMessagesToLogging is true.
	 * This is generally set to the name of the camera rig asset being built.
	 */
	void SetLoggingPrefix(const FString& InPrefix);

	/** Returns whether build messages are sent to the console. */
	bool IsForwardingMessagesToLogging() const { return bForwardToLogging; }
	/** Sets whether build messages are sent to the console. */
	void SetForwardMessagesToLogging(bool bInForwardToLogging);

	/** Adds a new message. */
	void AddMessage(EMessageSeverity::Type InSeverity, FText&& InText);
	/** Adds a new message. */
	void AddMessage(EMessageSeverity::Type InSeverity, UObject* InObject, FText&& InText);

	/** Gets the list of received messages so far. */
	TArrayView<const FCameraRigAssetBuildLogMessage> GetMessages() const { return Messages; }

private:

	TArray<FCameraRigAssetBuildLogMessage> Messages;

	FString LoggingPrefix;
	bool bForwardToLogging = true;
};

/**
 * A class that can prepare a camera rig for runtime use.
 *
 * This builder class sets up internal camera variables that handle exposed camera
 * rig parameters, computes the allocation information of the camera rig, and
 * does various kinds of validation.
 *
 * Once the build process is done, the BuildStatus proeprty is set on the camera rig.
 */
class FCameraRigAssetBuilder
{
public:

	/** Creates a new camera rig builder. */
	FCameraRigAssetBuilder(FCameraRigAssetBuildLog& InBuildLog);

	/** Builds the given camera rig. */
	void BuildCameraRig(UCameraRigAsset* InCameraRig);

private:

	void BuildCameraRigImpl();

	void FlattenCameraNodeHierarchy();

	void GatherOldDrivenParameters();
	void BuildNewDrivenParameters();
	void DiscardUnusedPrivateVariables();

	void BuildAllocationInfo();
	void BuildAllocationInfo(const UCameraNode* CameraNode);

	void UpdateBuildStatus();

private:

	FCameraRigAssetBuildLog& BuildLog;

	UCameraRigAsset* CameraRig = nullptr;

	TArray<UCameraNode*> FlattenedNodes;

	using FDrivenParameterKey = TTuple<FStructProperty*, UCameraNode*>;
	TMap<FDrivenParameterKey, UCameraVariableAsset*> OldDrivenParameters;

	FCameraRigAllocationInfo AllocationInfo;

	bool bHasErrors;
	bool bHasWarnings;

	friend struct Internal::FPrivateVariableBuilder;
};

}  // namespace UE::Cameras

