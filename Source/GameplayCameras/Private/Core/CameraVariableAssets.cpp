// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraVariableAssets.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraVariableAssets)

UCameraVariableAsset::UCameraVariableAsset(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UCameraVariableAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

#if WITH_EDITORONLY_DATA
	if ((Ar.IsLoading() && VariableId == 0) || Ar.IsSaving())
	{
		VariableId = GetTypeHash(GetFullName());
	}
#endif
}

