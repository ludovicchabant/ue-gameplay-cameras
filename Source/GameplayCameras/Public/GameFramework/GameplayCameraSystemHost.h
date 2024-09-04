// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Templates/SharedPointerFwd.h"

#include "GameplayCameraSystemHost.generated.h"

class APlayerController;

namespace UE::Cameras
{
	class FCameraSystemEvaluator;
}

/**
 * A class that hosts a camera system evaluator so that it can be accessed in a game world.
 *
 * The host doesn't stay alive very long if nothing references it. Gameplay camera components and actors
 * are meant to hold a reference to it while they use it. When nobody uses it, the host is meant to be
 * collectable by the GC.
 */
UCLASS(MinimalAPI)
class UGameplayCameraSystemHost : public UObject
{
	GENERATED_BODY()

public:

	/** Creates a new camera system host. */
	GAMEPLAYCAMERAS_API UGameplayCameraSystemHost(const FObjectInitializer& ObjectInitializer);

	// UObject interface.
	static GAMEPLAYCAMERAS_API void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

public:

	using FCameraSystemEvaluator = UE::Cameras::FCameraSystemEvaluator;

	/** Get the player controller that this host is hooked up to. */
	GAMEPLAYCAMERAS_API APlayerController* GetPlayerController();

	/** Gets the camera system evaluator. */
	GAMEPLAYCAMERAS_API TSharedPtr<FCameraSystemEvaluator> GetCameraSystemEvaluator();

protected:

	// UObject interface.
	GAMEPLAYCAMERAS_API virtual void BeginDestroy() override;

public:

	/**
	 * Finds a camera system host under the given player controller, or creates one if none was found.
	 * If the "auto-spawn camera system actor" setting is enabled, this will also spawn an actor that 
	 * references the newly created host, and sets itself as the active view-target.
	 */
	static GAMEPLAYCAMERAS_API UGameplayCameraSystemHost* FindOrCreateHost(APlayerController* PlayerController, const TCHAR* HostName = nullptr);

	/**
	 * Finds a camera system host under the given player controller.
	 */
	static GAMEPLAYCAMERAS_API UGameplayCameraSystemHost* FindHost(APlayerController* PlayerController, const TCHAR* HostName = nullptr, bool bAllowNull = true);

private:

	/** Default host name to use when creating a new host. */
	static const TCHAR* DefaultHostName;

	/** The camera system evaluator. */
	TSharedPtr<FCameraSystemEvaluator> Evaluator;
};

