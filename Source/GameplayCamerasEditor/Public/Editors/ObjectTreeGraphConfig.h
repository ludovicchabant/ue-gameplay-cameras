// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "CoreTypes.h"
#include "EdGraph/EdGraphNode.h"
#include "Misc/Optional.h"
#include "Templates/SubclassOf.h"
#include "Templates/UnrealTypeTraits.h"

#include <initializer_list>

class FText;
class UClass;
class UObject;
class UObjectTreeGraphNode;

DECLARE_DELEGATE_RetVal_OneParam(FText, FOnGetObjectClassDisplayName, const UClass*);
DECLARE_DELEGATE_TwoParams(FOnFormatObjectDisplayName, const UObject*, FText&);

#define OTGCC_FIELD(FieldType, FieldName)\
	public:\
		typename TCallTraits<FieldType>::ConstReference FieldName() const\
			{ return _##FieldName; }\
		FObjectTreeGraphClassConfig& FieldName(typename TCallTraits<FieldType>::ParamType InValue)\
			{ _##FieldName = InValue; return *this; }\
	private:\
		FieldType _##FieldName;

struct FObjectTreeGraphClassConfig
{
public:

	OTGCC_FIELD(TSubclassOf<UObjectTreeGraphNode>, GraphNodeClass)

	OTGCC_FIELD(FText, SelfPinFriendlyName)
	OTGCC_FIELD(EEdGraphPinDirection, SelfPinDirection)
	OTGCC_FIELD(bool, HasSelfPin)

	OTGCC_FIELD(EEdGraphPinDirection, DefaultPropertyPinDirection)

	OTGCC_FIELD(TOptional<FLinearColor>, NodeTitleColor)
	OTGCC_FIELD(TOptional<FLinearColor>, NodeBodyTintColor)

	OTGCC_FIELD(bool, NodeTitleUsesObjectName)
	OTGCC_FIELD(FOnGetObjectClassDisplayName, OnGetObjectClassDisplayName)

	OTGCC_FIELD(bool, CanCreateNew)
	OTGCC_FIELD(bool, CanDuplicate)
	OTGCC_FIELD(bool, CanDelete)

public:

	FObjectTreeGraphClassConfig();

	FObjectTreeGraphClassConfig& OnlyAsRoot();

public:

	TArrayView<const FString> StripDisplayNameSuffixes() const { return _StripDisplayNameSuffixes; }

	FObjectTreeGraphClassConfig& StripDisplayNameSuffix(const FString& InSuffix)
	{
		_StripDisplayNameSuffixes.Add(InSuffix);
		return *this;
	}

	FObjectTreeGraphClassConfig& StripDisplayNameSuffixes(std::initializer_list<FString> InSuffixes)
	{
		_StripDisplayNameSuffixes.Append(InSuffixes);
		return *this;
	}

	const TMap<FName, EEdGraphPinDirection>& PropertyPinDirections() const { return _PropertyPinDirections; }

	FObjectTreeGraphClassConfig& SetPropertyPinDirection(const FName& InPropertyName, EEdGraphPinDirection InDirection)
	{
		_PropertyPinDirections.Add(InPropertyName, InDirection);
		return *this;
	}

	EEdGraphPinDirection GetPropertyPinDirection(const FName& InPropertyName) const
	{
		if (const EEdGraphPinDirection* PinDirection = _PropertyPinDirections.Find(InPropertyName))
		{
			return *PinDirection;
		}
		return _DefaultPropertyPinDirection;
	}

private:

	TArray<FString> _StripDisplayNameSuffixes;
	TMap<FName, EEdGraphPinDirection> _PropertyPinDirections;
};

struct FObjectTreeGraphConfig
{
public:

	TArray<UClass*> ConnectableObjectClasses;
	TArray<UClass*> NonConnectableObjectClasses;
	
	TSubclassOf<UObjectTreeGraphNode> DefaultGraphNodeClass;

	FLinearColor DefaultGraphNodeTitleColor;
	FLinearColor DefaultGraphNodeBodyTintColor;

	FOnFormatObjectDisplayName OnFormatObjectDisplayName;

	TMap<UClass*, FObjectTreeGraphClassConfig> ObjectClassConfigs;

public:

	FObjectTreeGraphConfig();

	bool IsConnectable(UClass* InObjectClass) const;
	void GetConnectableClasses(TArray<UClass*>& OutClasses, bool bPlaceableOnly = false);
	
	const FObjectTreeGraphClassConfig& GetObjectClassConfig(const UClass* InObjectClass) const;

	FText GetDisplayNameText(const UObject* InObject) const;
	FText GetDisplayNameText(const UClass* InClass) const;

private:

	FText GetDisplayNameText(const UClass* InClass, const FObjectTreeGraphClassConfig& InClassConfig) const;
	void FormatDisplayNameText(const UObject* InObject, const FObjectTreeGraphClassConfig& InClassConfig, FText& InOutDisplayNameText) const;
};

