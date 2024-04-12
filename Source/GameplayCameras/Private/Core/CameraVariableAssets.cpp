// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraVariableAssets.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraVariableAssets)

UCameraVariableAsset::UCameraVariableAsset(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

void UCameraVariableAsset::RegenerateVariableID()
{
	VariableID = FCameraVariableID::FromHashValue(GetTypeHash(GetFullName()));
}

FCameraVariableDefinition UCameraVariableAsset::GetVariableDefinition() const
{
	FCameraVariableDefinition VariableDefinition;
	VariableDefinition.VariableID = GetVariableID();
	VariableDefinition.VariableType = GetVariableType();
#if WITH_EDITORONLY_DATA
	VariableDefinition.VariableName = GetName();
#endif
	return VariableDefinition;
}

void UCameraVariableAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

#if WITH_EDITORONLY_DATA
	if ((Ar.IsLoading() && !VariableID.IsValid()) || Ar.IsSaving())
	{
		RegenerateVariableID();
	}
#endif
}

void UCameraVariableAsset::PostInitProperties()
{
	RegenerateVariableID();
	Super::PostInitProperties();
}

void UCameraVariableAsset::PostRename(UObject* OldOuter, const FName OldName)
{
	RegenerateVariableID();
	Super::PostRename(OldOuter, OldName);
}

void UCameraVariableAsset::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	RegenerateVariableID();
	Super::PostDuplicate(DuplicateMode);
}

