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

/**
 * A structure providing optional configuration options for a given object class.
 */
struct FObjectTreeGraphClassConfig
{
public:

	/** The subclass of graph nodes to create. */
	OTGCC_FIELD(TSubclassOf<UObjectTreeGraphNode>, GraphNodeClass)

	/** The name of the self pin. */
	OTGCC_FIELD(FName, SelfPinName)
	/** The display name of the self pin. */
	OTGCC_FIELD(FText, SelfPinFriendlyName)
	/** The direction of the self pin. */
	OTGCC_FIELD(EEdGraphPinDirection, SelfPinDirection)
	/** Whether graph nodes for this class have a self pin. */
	OTGCC_FIELD(bool, HasSelfPin)

	/** Default direction of property pins. */
	OTGCC_FIELD(EEdGraphPinDirection, DefaultPropertyPinDirection)

	/** Color of the graph node's title. */
	OTGCC_FIELD(TOptional<FLinearColor>, NodeTitleColor)
	/** Color of the graph node's body. */
	OTGCC_FIELD(TOptional<FLinearColor>, NodeBodyTintColor)

	/** Whether the graph node title uses the underlying object's name instead of its class name. */
	OTGCC_FIELD(bool, NodeTitleUsesObjectName)
	/** A custom call back to get the object's display name used in the graph node title. */
	OTGCC_FIELD(FOnGetObjectClassDisplayName, OnGetObjectClassDisplayName)

	/** Whether users can create new objects of this class in the graph. */
	OTGCC_FIELD(bool, CanCreateNew)
	/** Whether users can duplicate objects of this class in the graph. */
	OTGCC_FIELD(bool, CanDuplicate)
	/** Whether users can delete objects of this class in the graph. */
	OTGCC_FIELD(bool, CanDelete)

public:

	FObjectTreeGraphClassConfig();

	/** A shortcut for disabling CanCreateNew, CanDuplicate, and CanDelete. */
	FObjectTreeGraphClassConfig& OnlyAsRoot();

public:

	/** Gets the name suffixes to strip. */
	TArrayView<const FString> StripDisplayNameSuffixes() const { return _StripDisplayNameSuffixes; }

	/** Adds a new suffix to strip from the display name. */
	FObjectTreeGraphClassConfig& StripDisplayNameSuffix(const FString& InSuffix)
	{
		_StripDisplayNameSuffixes.Add(InSuffix);
		return *this;
	}

	/** Adds multiple suffixes to strip from the display name. */
	FObjectTreeGraphClassConfig& StripDisplayNameSuffixes(std::initializer_list<FString> InSuffixes)
	{
		_StripDisplayNameSuffixes.Append(InSuffixes);
		return *this;
	}

	/** Gets the custom property pin directions for given named properties. */
	const TMap<FName, EEdGraphPinDirection>& PropertyPinDirections() const { return _PropertyPinDirections; }

	/** Adds a new custom property pin direction for a given named property. */
	FObjectTreeGraphClassConfig& SetPropertyPinDirection(const FName& InPropertyName, EEdGraphPinDirection InDirection)
	{
		_PropertyPinDirections.Add(InPropertyName, InDirection);
		return *this;
	}

	/** Gets the custom property pin direction for a given named property. */
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

/**
 * A structure that provides all the information needed to build, edit, and maintain an 
 * object tree graph.
 */
struct FObjectTreeGraphConfig
{
public:

	/**
	 * The list of connectable object classes.
	 *
	 * Objects whose class is connectable (which includes sub-classes) will be eligible 
	 * to be nodes in the graph. Properties on those objects that point to other connectable
	 * objects (either with a direct object property or an array property) will show up
	 * as pins on the object's node.
	 */
	TArray<UClass*> ConnectableObjectClasses;
	/**
	 * The list of unconnectable object classes.
	 *
	 * This serves as an exception list to the ConnectableObjectClasses list.
	 */
	TArray<UClass*> NonConnectableObjectClasses;
	
	/**
	 * The default graph node class to use in the graph. Defaults to UObjectTreeGraphNode.
	 */
	TSubclassOf<UObjectTreeGraphNode> DefaultGraphNodeClass;

	/** The default title color for an object's graph node. */
	FLinearColor DefaultGraphNodeTitleColor;
	/** The default body color for an object's graph node. */
	FLinearColor DefaultGraphNodeBodyTintColor;

	/** A custom callback to format an object's display name. */
	FOnFormatObjectDisplayName OnFormatObjectDisplayName;

	/** Advanced, optional bits of configuration for specific classes and sub-classes of objects. */
	TMap<UClass*, FObjectTreeGraphClassConfig> ObjectClassConfigs;

public:

	/** Creates a new graph config. */
	FObjectTreeGraphConfig();

	/**
	 * Returns whether the given class is connectable.
	 *
	 * It is connectable if it, or one of its parent classes, is inside ConnectableObjectClasses,
	 * and nor it or any of its parent classes is in NonConnectableObjectClasses.
	 */
	bool IsConnectable(UClass* InObjectClass) const;

	/**
	 * Returns whether the given object reference property is connectable.
	 *
	 * It is connectable if the property's reference type is for a connectable class, and if the
	 * property doesn't have the ObjectTreeGraphHidden metadata.
	 */
	bool IsConnectable(FObjectProperty* InObjectProperty) const;

	/**
	 * Returns whether the given object array property is connectable.
	 *
	 * It is connectable if the array's item type is for a connectable class, and if the array
	 * property doesn't have the ObjectTreeGraphHidden metadata.
	 */
	bool IsConnectable(FArrayProperty* InArrayProperty) const;

	/**
	 * Gets all possible known connectable classes.
	 *
	 * @param bPlaceableOnly  If set, only return those that can be created.
	 */
	void GetConnectableClasses(TArray<UClass*>& OutClasses, bool bPlaceableOnly = false);
	
	/** Gets the advanced class-specific configuration for the given class. */
	const FObjectTreeGraphClassConfig& GetObjectClassConfig(const UClass* InObjectClass) const;

	/** Computes the display name of the given object. */
	FText GetDisplayNameText(const UObject* InObject) const;
	/** Computes the display name of the given object class. */
	FText GetDisplayNameText(const UClass* InClass) const;

private:

	FText GetDisplayNameText(const UClass* InClass, const FObjectTreeGraphClassConfig& InClassConfig) const;
	void FormatDisplayNameText(const UObject* InObject, const FObjectTreeGraphClassConfig& InClassConfig, FText& InOutDisplayNameText) const;
};

