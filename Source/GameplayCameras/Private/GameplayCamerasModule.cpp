// Copyright Epic Games, Inc. All Rights Reserved.

#include "IGameplayCamerasModule.h"

#include "Debug/CameraDebugColors.h"
#include "GameplayCameras.h"
#include "Logging/MessageLog.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "GameplayCamerasModule"

DEFINE_LOG_CATEGORY(LogCameraSystem);

IGameplayCamerasModule& IGameplayCamerasModule::Get()
{
	return FModuleManager::LoadModuleChecked<IGameplayCamerasModule>("GameplayCameras");
}

class FGameplayCamerasModule : public IGameplayCamerasModule
{
public:

	// IModuleInterface interface
	virtual void StartupModule() override
	{
#if UE_GAMEPLAY_CAMERAS_DEBUG
		UE::Cameras::FCameraDebugColors::RegisterBuiltinColorSchemes();
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
	}

	virtual void ShutdownModule() override
	{
	}

public:

	// IGameplayCamerasModule interface
	virtual void RegisterBlendableStruct(const UScriptStruct* StructType, UE::Cameras::FBlendableStructTypeErasedInterpolator Interpolator) override
	{
		using namespace UE::Cameras;

		if (!ensure(EnumHasAllFlags(StructType->StructFlags, STRUCT_IsPlainOldData)))
		{
			return;
		}

		const bool bAlreadyRegistered = BlendableStructs.ContainsByPredicate([StructType](const FBlendableStructInfo& Item)
				{
					return Item.StructType == StructType;
				});
		if (ensure(!bAlreadyRegistered))
		{
			FBlendableStructInfo& NewInfo = BlendableStructs.Emplace_GetRef();
			NewInfo.StructType = StructType;
			NewInfo.Interpolator = Interpolator;
		}
	}

	virtual TConstArrayView<UE::Cameras::FBlendableStructInfo> GetBlendableStructs() const override
	{
		return BlendableStructs;
	}

	virtual void UnregisterBlendableStruct(const UScriptStruct* StructType) override
	{
		using namespace UE::Cameras;

		BlendableStructs.RemoveAll([StructType](const FBlendableStructInfo& Item)
				{
					return Item.StructType == StructType;
				});
	}

#if WITH_EDITOR
	virtual TSharedPtr<IGameplayCamerasLiveEditManager> GetLiveEditManager() const override
	{
		return LiveEditManager;
	}

	virtual void SetLiveEditManager(TSharedPtr<IGameplayCamerasLiveEditManager> InLiveEditManager) override
	{
		LiveEditManager = InLiveEditManager;
	}
#endif

private:

	TArray<UE::Cameras::FBlendableStructInfo> BlendableStructs;

#if WITH_EDITOR
	TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager;
#endif
};

IMPLEMENT_MODULE(FGameplayCamerasModule, GameplayCameras);

#undef LOCTEXT_NAMESPACE

