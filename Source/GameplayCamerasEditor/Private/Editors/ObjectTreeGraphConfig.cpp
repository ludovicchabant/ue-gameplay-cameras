// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/ObjectTreeGraphConfig.h"

#include "Algo/AnyOf.h"
#include "Core/ObjectTreeGraphObject.h"
#include "UObject/Class.h"
#include "UObject/Object.h"
#include "UObject/UObjectIterator.h"

#define LOCTEXT_NAMESPACE "ObjectTreeGraphConfig"

FObjectTreeGraphClassConfig::FObjectTreeGraphClassConfig()
	: _SelfPinFriendlyName(LOCTEXT("SelfPinFriendlyName", "self"))
	, _SelfPinDirection(EGPD_Input)
	, _HasSelfPin(true)
	, _DefaultPropertyPinDirection(EGPD_Output)
	, _NodeTitleUsesObjectName(false)
	, _CanCreateNew(true)
	, _CanDuplicate(true)
	, _CanDelete(true)
{
}

FObjectTreeGraphClassConfig& FObjectTreeGraphClassConfig::OnlyAsRoot()
{
	_CanCreateNew = false;
	_CanDuplicate = false;
	_CanDelete = false;
	return *this;
}

FObjectTreeGraphConfig::FObjectTreeGraphConfig()
	: DefaultGraphNodeTitleColor(FLinearColor(0.549f, 0.745f, 0.698f))
	, DefaultGraphNodeBodyTintColor(FLinearColor::White)
{
}

bool FObjectTreeGraphConfig::IsConnectable(UClass* InObjectClass) const
{
	const bool bIsConnectable = Algo::AnyOf(ConnectableObjectClasses, [InObjectClass](UClass* Item)
			{
				return InObjectClass->IsChildOf(Item);
			});
	if (!bIsConnectable)
	{
		return false;
	}

	const bool bIsExcluded = Algo::AnyOf(NonConnectableObjectClasses, [InObjectClass](UClass* Item)
			{
				return InObjectClass->IsChildOf(Item);
			});
	if (bIsExcluded)
	{
		return false;
	}

	return true;
}

void FObjectTreeGraphConfig::GetConnectableClasses(TArray<UClass*>& OutClasses, bool bPlaceableOnly)
{
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		if (!IsConnectable(*ClassIt))
		{
			continue;
		}

		if (bPlaceableOnly)
		{
			if (ClassIt->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_Hidden))
			{
				continue;
			}

			const FObjectTreeGraphClassConfig& ClassConfig = GetObjectClassConfig(*ClassIt);
			if (!ClassConfig.CanCreateNew())
			{
				continue;
			}
		}

		OutClasses.Add(*ClassIt);
	}
}

const FObjectTreeGraphClassConfig& FObjectTreeGraphConfig::GetObjectClassConfig(const UClass* InObjectClass) const
{
	static const FObjectTreeGraphClassConfig DefaultClassConfig;
	
	while (InObjectClass)
	{
		const FObjectTreeGraphClassConfig* ClassConfig = ObjectClassConfigs.Find(InObjectClass);
		if (ClassConfig)
		{
			return *ClassConfig;
		}

		InObjectClass = InObjectClass->GetSuperClass();
	}

	return DefaultClassConfig;
}

FText FObjectTreeGraphConfig::GetDisplayNameText(const UObject* InObject) const
{
	if (InObject)
	{
		FText DisplayNameText;

		const IObjectTreeGraphObject* GraphObject = Cast<IObjectTreeGraphObject>(InObject);
		if (GraphObject && GraphObject->HasSupportFlags(EObjectTreeGraphObjectSupportFlags::CustomRename))
		{
			DisplayNameText = FText::FromString(GraphObject->GetGraphNodeName());
		}
		if (!DisplayNameText.IsEmpty())
		{
			FormatDisplayNameText(InObject, DisplayNameText);
			return DisplayNameText;
		}

		const FObjectTreeGraphClassConfig& ClassConfig = GetObjectClassConfig(InObject->GetClass());
		if (ClassConfig.NodeTitleUsesObjectName())
		{
			DisplayNameText = FText::FromString(InObject->GetName());
			FormatDisplayNameText(InObject, DisplayNameText);
			return DisplayNameText;
		}

		return GetDisplayNameText(InObject->GetClass());
	}
	return FText::GetEmpty();
}

FText FObjectTreeGraphConfig::GetDisplayNameText(const UClass* InClass) const
{
	if (InClass)
	{
		const FObjectTreeGraphClassConfig& ClassConfig = GetObjectClassConfig(InClass);
		if (ClassConfig.OnGetObjectClassDisplayName().IsBound())
		{
			return ClassConfig.OnGetObjectClassDisplayName().Execute(InClass);
		}

		FText DisplayNameText = InClass->GetDisplayNameText();
		FormatDisplayNameText(InClass, DisplayNameText);
		return DisplayNameText;
	}
	return FText::GetEmpty();
}

void FObjectTreeGraphConfig::FormatDisplayNameText(const UObject* InObject, FText& InOutDisplayNameText) const
{
	if (StripDisplayNameSuffixes.Num() > 0)
	{
		FString DisplayName = InOutDisplayNameText.ToString();
		for (const FString& StripSuffix : StripDisplayNameSuffixes)
		{
			if (DisplayName.RemoveFromEnd(StripSuffix))
			{
				DisplayName.TrimEndInline();
				break;
			}
		}

		InOutDisplayNameText = FText::FromString(DisplayName);
	}

	OnFormatObjectDisplayName.ExecuteIfBound(InObject, InOutDisplayNameText);
}

#undef LOCTEXT_NAMESPACE

