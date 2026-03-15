// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraVariableCollection.h"

#include "AssetRegistry/AssetData.h"
#include "Core/CameraVariableAssets.h"
#include "GameplayCameras.h"
#include "UObject/AssetRegistryTagsContext.h"
#include "UObject/ObjectRedirector.h"
#include "UObject/ObjectSaveContext.h"

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
			UE_LOGF(LogCameraSystem, Warning, "Adding missing RF_Public flag on variable '%ls'.", *GetPathNameSafe(Variable));
			Variable->SetFlags(RF_Public);
		}
	}

	CleanUpStrayObjects();
#endif  // WITH_EDITOR
}

void UCameraVariableCollection::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
{
	Context.AddTag(FAssetRegistryTag(
				TEXT("NumVariables"), 
				FString::FromInt(Variables.Num()),
				FAssetRegistryTag::TT_Numerical));

	const UEnum* VariableTypeEnum = StaticEnum<ECameraVariableType>();
	for (int32 Index = 0; Index < Variables.Num(); ++Index)
	{
		UCameraVariableAsset* Variable = Variables[Index];
		if (Variable)
		{
			Context.AddTag(FAssetRegistryTag(
						FName(FString::Printf(TEXT("VariableName%d"), Index)),
						Variable->DisplayName,
						FAssetRegistryTag::TT_Hidden));
			Context.AddTag(FAssetRegistryTag(
						FName(FString::Printf(TEXT("VariableGuid%d"), Index)),
						Variable->GetGuid().ToString(EGuidFormats::Digits),
						FAssetRegistryTag::TT_Hidden));
			Context.AddTag(FAssetRegistryTag(
						FName(FString::Printf(TEXT("VariableType%d"), Index)),
						VariableTypeEnum->GetNameStringByValue((int64)Variable->GetVariableType()),
						FAssetRegistryTag::TT_Hidden));
		}
	}

	Super::GetAssetRegistryTags(Context);
}

void UCameraVariableCollection::GetVariablesForType(const FAssetData& AssetData, ECameraVariableType DesiredType, TArray<FSoftCameraVariablePtr>& OutVariables)
{
	TSoftObjectPtr<UCameraVariableCollection> SoftVariableCollection(AssetData.GetSoftObjectPath());

	int32 NumVariables = 0;
	if (AssetData.GetTagValue<int32>(TEXT("NumVariables"), NumVariables))
	{
		const UEnum* VariableTypeEnum = StaticEnum<ECameraVariableType>();

		for (int32 Index = 0; Index < NumVariables; ++Index)
		{
			FString VariableTypeStr;
			bool bMatchesDesiredType = false;
			if (AssetData.GetTagValue<FString>(FName(FString::Printf(TEXT("VariableType%d"), Index)), VariableTypeStr))
			{
				ECameraVariableType VariableType = (ECameraVariableType)VariableTypeEnum->GetValueByNameString(VariableTypeStr);
				bMatchesDesiredType = (VariableType == DesiredType);
			}

			if (bMatchesDesiredType)
			{
				const FGuid VariableGuid = AssetData.GetTagValueRef<FGuid>(FName(FString::Printf(TEXT("VariableGuid%d"), Index)));
				const FString VariableName = AssetData.GetTagValueRef<FString>(FName(FString::Printf(TEXT("VariableName%d"), Index)));
				OutVariables.Add(FSoftCameraVariablePtr{ SoftVariableCollection, VariableGuid, VariableName });
			}
		}
	}
	else if (UCameraVariableCollection* LoadedAsset = Cast<UCameraVariableCollection>(AssetData.GetAsset()))
	{
		for (UCameraVariableAsset* Variable : LoadedAsset->Variables)
		{
			const bool bMatchesDesiredType = (Variable->GetVariableType() == DesiredType);
			if (bMatchesDesiredType)
			{
				OutVariables.Add(FSoftCameraVariablePtr{ SoftVariableCollection, Variable->GetGuid(), Variable->DisplayName });
			}
		}
	}
}

#if WITH_EDITOR

void UCameraVariableCollection::CleanUpStrayObjects()
{
	UPackage* CollectionPackage = GetOutermost();
	if (!CollectionPackage || CollectionPackage == GetTransientPackage())
	{
		return;
	}

	TSet<UObject*> StrayObjects;
	TSet<UCameraVariableAsset*> KnownVariables(Variables);

	TArray<UObject*> ObjectsInPackage;
	GetObjectsWithPackage(CollectionPackage, ObjectsInPackage);
	for (UObject* Object : ObjectsInPackage)
	{
		UCameraVariableAsset* Variable = Cast<UCameraVariableAsset>(Object);
		if (!Variable)
		{
			continue;
		}
		if (KnownVariables.Contains(Variable))
		{
			continue;
		}

		Variable->ClearFlags(RF_Public | RF_Standalone);
		StrayObjects.Add(Variable);
	}

	if (StrayObjects.Num() > 0)
	{
		// Also clean-up any redirectors to these objects.
		for (UObject* Object : ObjectsInPackage)
		{
			if (UObjectRedirector* Redirector = Cast<UObjectRedirector>(Object))
			{
				if (StrayObjects.Contains(Redirector->DestinationObject))
				{
					Redirector->ClearFlags(RF_Public | RF_Standalone);
					Redirector->DestinationObject = nullptr;
				}
			}
		}

		UE_LOGF(LogCameraSystem, Warning,
				"Cleaned up %d stray camera variables in camera variable collection '%ls'. Please resave the asset.",
				StrayObjects.Num(), *GetPathNameSafe(this));
	}
}

#endif  // WITH_EDITOR

UCameraVariableAsset* FSoftCameraVariablePtr::Get() const
{
	if (UCameraVariableCollection* ActualVariableCollection = VariableCollection.Get())
	{
		TObjectPtr<UCameraVariableAsset>* FoundItem = ActualVariableCollection->Variables.FindByPredicate(
				[this](UCameraVariableAsset* Item)
				{
					return Item && Item->GetGuid() == VariableGuid;
				});
		if (FoundItem)
		{
			return FoundItem->Get();
		}
	}
	return nullptr;
}

