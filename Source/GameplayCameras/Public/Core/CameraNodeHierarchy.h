// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Core/CameraNode.h"

class UCameraRigAsset;

namespace UE::Cameras
{

/**
 * A utility class that stores a flattened hierarchy of camera nodes.
 * Unconnected camera nodes aren't included, of course.
 */
class GAMEPLAYCAMERAS_API FCameraNodeHierarchy
{
public:

	/** Build an empty hierarchy. */
	FCameraNodeHierarchy();
	/** Build a hierarchy starting from the given camera rig's root node. */
	FCameraNodeHierarchy(UCameraRigAsset* InCameraRig);
	/** Build a hierarchy starting from the given root node. */
	FCameraNodeHierarchy(UCameraNode* InRootCameraNode);

	/** Get the list of camera nodes in depth-first order. */
	TArrayView<UCameraNode* const> GetFlattenedHierarchy() const;

	/** Returns the number of camera nodes in this hierarchy. */
	int32 Num() const;

public:

	/** Build a hierarchy starting from the given camera rig's root node. */
	void Build(UCameraRigAsset* InCameraRig);
	/** Build a hierarchy starting from the given root node. */
	void Build(UCameraNode* InRootCameraNode);
	/** Resets this object to an empty hierarchy. */
	void Reset();

public:

	/** Executes the given predicate on each camera node in depth-first order. */
	template<typename PredicateClass>
	void ForEach(PredicateClass&& Predicate)
	{
		for (UCameraNode* Node : FlattenedHierarchy)
		{
			Predicate(Node);
		}
	}

public:

	// Internal API.

#if WITH_EDITORONLY_DATA
	bool FindMissingConnectableObjects(TArrayView<UObject* const> ConnectableObjects, TSet<UObject*>& OutMissingObjects);
	bool FindMissingConnectableObjects(const TSet<UObject*> ConnectableObjectsSet, TSet<UObject*>& OutMissingObjects);
#endif  // WITH_EDITORONLY_DATA

private:

	TArray<UCameraNode*> FlattenedHierarchy;
};

}  // namespace UE::Cameras

