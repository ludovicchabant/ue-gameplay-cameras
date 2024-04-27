// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreTypes.h"
#include "Modules/ModuleManager.h"

class FGameplayCamerasUncookedOnlyModule : public IModuleInterface
{
protected:

	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
};

IMPLEMENT_MODULE(FGameplayCamerasUncookedOnlyModule, GameplayCamerasUncookedOnly);

