// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraVariableCollection.h"

#include "Core/CameraVariableAssets.h"
#include "GameplayCameras.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraVariableCollection)

UCameraVariableCollection::UCameraVariableCollection(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UCameraVariableCollection::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	for (UCameraVariableAsset* Variable : Variables)
	{
		if (!Variable->HasAnyFlags(RF_Public))
		{
			UE_LOG(LogCameraSystem, Warning, TEXT("Adding missing RF_Public flag on variable '%s'."), *GetPathNameSafe(Variable));
			Variable->SetFlags(RF_Public);
		}
	}
#endif  // WITH_EDITOR
}

