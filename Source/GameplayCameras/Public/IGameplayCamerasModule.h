// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Templates/SharedPointer.h"

namespace UE::Cameras
{

#if WITH_EDITOR
class IGameplayCamerasLiveEditManager;
#endif

}  // namespace UE::Cameras

class IGameplayCamerasModule : public IModuleInterface
{
public:

	/**
	 * Singleton-like access to ICameraModule
	 *
	 * @return The ICameraModule instance, loading the module on demand if needed
	 */
	GAMEPLAYCAMERAS_API static IGameplayCamerasModule& Get();

#if WITH_EDITOR
	using IGameplayCamerasLiveEditManager = UE::Cameras::IGameplayCamerasLiveEditManager;

	/**
	 * Gets the live edit manager.
	 */
	virtual TSharedPtr<IGameplayCamerasLiveEditManager> GetLiveEditManager() const = 0;

	/**
	 * Sets the live edit manager.
	 */
	virtual void SetLiveEditManager(TSharedPtr<IGameplayCamerasLiveEditManager> InLiveEditManager) = 0;
#endif
};

#if UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2
#include "CoreMinimal.h"
#endif
