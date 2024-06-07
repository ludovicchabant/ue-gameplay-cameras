// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "UObject/WeakObjectPtr.h"

class UObject;
struct FObjectTreeGraphConfig;

/** Search result for an object tree graph search. */
struct FObjectTreeGraphSearchResult
{
	/** The root object that the current search result was found in. */
	UObject* RootObject = nullptr;

	/** The graph config for the object hierarchy this result was found in. */
	const FObjectTreeGraphConfig* GraphConfig = nullptr;

	/** The object found to match the search. */
	UObject* Object = nullptr;

	/** 
	 * The specific object property that matched the search. 
	 * None if the object itself matches.
	 */
	FName PropertyName;
};


/**
 * A utility class that can search a series of string tokens across an
 * object tree graph.
 */
class FObjectTreeGraphSearch
{
public:

	FObjectTreeGraphSearch();

public:

	/** Adds a root object to search through. */
	void AddRootObject(UObject* InObject, const FObjectTreeGraphConfig* InGraphConfig);

	/** Searchs for the given string tokens. */
	void Search(TArrayView<FString> InTokens, TArray<FObjectTreeGraphSearchResult>& OutResults) const;

private:

	using FSearchResult = FObjectTreeGraphSearchResult;

	struct FSearchState
	{
		const FObjectTreeGraphConfig* GraphConfig = nullptr;
		TArrayView<FString> Tokens;

		UObject* RootObject = nullptr;
		TArray<UObject*> ObjectStack;
		TSet<UObject*> VisitedObjects;

		TArray<FSearchResult> Results;
	};

	struct FRootObjectInfo
	{
		TWeakObjectPtr<> WeakRootObject;
		const FObjectTreeGraphConfig* GraphConfig = nullptr;
	};

	void SearchRootObject(const FRootObjectInfo& InRootObjectInfo, TArrayView<FString> InTokens, TArray<FSearchResult>& OutResults) const;
	void SearchObject(UObject* InObject, FSearchState& InOutState) const;

	bool MatchObject(UObject* InObject, const FSearchState& InState) const;
	bool MatchObjectProperty(UObject* InObject, FProperty* InProperty, const FSearchState& InState) const;
	bool MatchString(const FString& InString, const FSearchState& InState) const;

private:

	TArray<FRootObjectInfo> RootObjectInfos;
};

