// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/NameTypes.h"
#include "UObject/UnrealType.h"

class IGameplayCamerasModule;
class UCameraDirector;
class UCameraNode;
class UObject;

/**
 * Parameter structure for instantiating an object.
 */
struct FCameraRuntimeInstantiationParams
{
	UObject* InstantiationOuter = nullptr;
	FName InstantiationName = NAME_None;
};

/**
 * Utility class for instanting source objects into the game.
 */
class FCameraRuntimeInstantiator
{
public:

	/** 
	 * Instantiates a tree of camera nodes by cloning it, and keeping track of the relationship between
	 * the clones and their source assets.
	 */
	static UCameraNode* InstantiateCameraNodeTree(const UCameraNode* InRootNode, const FCameraRuntimeInstantiationParams& Params);

	/** 
	 * Instantiates a camera directory by cloning it, and keeping track of the relationship between
	 * the clone and its source asset.
	 */
	static UCameraDirector* InstantiateCameraDirector(const UCameraDirector* InCameraDirector, const FCameraRuntimeInstantiationParams& Params);

	/**
	 * Take a property change applied on a source object and try to replicate it on any related instantiated objects.
	 */
	static void ForwardPropertyChange(const UObject* Object, const FPropertyChangedEvent& PropertyChangedEvent);

private:

	static UObject* InstantiateObject(const UObject* InSource, const FCameraRuntimeInstantiationParams& Params);
	static IGameplayCamerasModule& GetGameplayCamerasModule();

private:

	static IGameplayCamerasModule* GameplayCamerasModule;
};

