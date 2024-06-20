// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigProxyAsset.h"

#include "Misc/Guid.h"

UCameraRigProxyAsset::UCameraRigProxyAsset(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	Guid = FGuid::NewGuid();
}

#if WITH_EDITORONLY_DATA

void UCameraRigProxyAsset::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

	if (DuplicateMode == EDuplicateMode::Normal)
	{
		Guid = FGuid::NewGuid();
	}
}

#endif

