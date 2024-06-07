// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/ObjectTreeGraphSearch.h"

#include "Core/ObjectTreeGraphRootObject.h"
#include "Editors/ObjectTreeGraphConfig.h"

FObjectTreeGraphSearch::FObjectTreeGraphSearch()
{
}

void FObjectTreeGraphSearch::AddRootObject(UObject* InObject, const FObjectTreeGraphConfig* InGraphConfig)
{
	RootObjectInfos.Add(FRootObjectInfo{ InObject, InGraphConfig });
}

void FObjectTreeGraphSearch::Search(TArrayView<FString> InTokens, TArray<FObjectTreeGraphSearchResult>& OutResults) const
{
	for (const FRootObjectInfo& RootObjectInfo : RootObjectInfos)
	{
		SearchRootObject(RootObjectInfo, InTokens, OutResults);
	}
}

void FObjectTreeGraphSearch::SearchRootObject(const FRootObjectInfo& InRootObjectInfo, TArrayView<FString> InTokens, TArray<FSearchResult>& OutResults) const
{
	UObject* RootObject = InRootObjectInfo.WeakRootObject.Get();
	if (!RootObject || !InRootObjectInfo.GraphConfig)
	{
		return;
	}

	FSearchState State;
	State.GraphConfig = InRootObjectInfo.GraphConfig;
	State.Tokens = InTokens;
	State.RootObject = RootObject;
	State.ObjectStack.Add(RootObject);

	if (IObjectTreeGraphRootObject* RootObjectInterface = Cast<IObjectTreeGraphRootObject>(RootObject))
	{
		TSet<UObject*> ConnectableObjects;
		RootObjectInterface->GetConnectableObjects(State.GraphConfig->GraphName, ConnectableObjects);
		for (UObject* ConnectableObject : ConnectableObjects)
		{
			State.ObjectStack.Add(ConnectableObject);
		}
	}

	while (State.ObjectStack.Num() > 0)
	{
		UObject* CurObject = State.ObjectStack.Pop();
		if (CurObject && !State.VisitedObjects.Contains(CurObject))
		{
			State.VisitedObjects.Add(CurObject);

			SearchObject(CurObject, State);
		}
	}

	OutResults.Append(State.Results);
}

void FObjectTreeGraphSearch::SearchObject(UObject* InObject, FSearchState& InOutState) const
{
	UClass* ObjectClass = InObject->GetClass();
	const FObjectTreeGraphConfig& GraphConfig(*InOutState.GraphConfig);

	const FObjectTreeGraphClassConfig& ObjectClassConfig = GraphConfig.GetObjectClassConfig(ObjectClass);

	if (MatchObject(InObject, InOutState))
	{
		InOutState.Results.Add(FSearchResult{ InOutState.RootObject, InOutState.GraphConfig, InObject });
	}

	for (TFieldIterator<FProperty> PropertyIt(ObjectClass); PropertyIt; ++PropertyIt)
	{
		if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(*PropertyIt))
		{
			if (!GraphConfig.IsConnectable(ObjectProperty))
			{
				continue;
			}

			if (MatchObjectProperty(InObject, *PropertyIt, InOutState))
			{
				InOutState.Results.Add(
						FSearchResult{ InOutState.RootObject, InOutState.GraphConfig, InObject, PropertyIt->GetFName() });
			}

			TObjectPtr<UObject> ConnectedObject;
			ObjectProperty->GetValue_InContainer(InObject, &ConnectedObject);
			if (ConnectedObject)
			{
				InOutState.ObjectStack.Add(ConnectedObject);
			}
		}
		else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(*PropertyIt))
		{
			if (!GraphConfig.IsConnectable(ArrayProperty))
			{
				continue;
			}

			if (MatchObjectProperty(InObject, *PropertyIt, InOutState))
			{
				InOutState.Results.Add(
						FSearchResult{ InOutState.RootObject, InOutState.GraphConfig, InObject, PropertyIt->GetFName() });
			}

			FObjectProperty* InnerProperty = CastFieldChecked<FObjectProperty>(ArrayProperty->Inner);
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(InObject));

			const int32 ArrayNum = ArrayHelper.Num();
			for (int32 Index = 0; Index < ArrayNum; ++Index)
			{
				UObject* ConnectedObject = InnerProperty->GetObjectPropertyValue(ArrayHelper.GetRawPtr(Index));
				if (ConnectedObject)
				{
					InOutState.ObjectStack.Add(ConnectedObject);
				}
			}
		}
	}
}

bool FObjectTreeGraphSearch::MatchObject(UObject* InObject, const FSearchState& InState) const
{
	const FObjectTreeGraphConfig& GraphConfig(*InState.GraphConfig);

	const FString DisplayNameText = GraphConfig.GetDisplayNameText(InObject).ToString();
	if (MatchString(DisplayNameText, InState))
	{
		return true;
	}

	return false;
}

bool FObjectTreeGraphSearch::MatchObjectProperty(UObject* InObject, FProperty* InProperty, const FSearchState& InState) const
{
	const FString PropertyName = InProperty->GetName();
	if (MatchString(PropertyName, InState))
	{
		return true;
	}

	return false;
}

bool FObjectTreeGraphSearch::MatchString(const FString& InString, const FSearchState& InState) const
{
	if (InString.IsEmpty())
	{
		return false;
	}

	for (const FString& Token : InState.Tokens)
	{
		if (!InString.Contains(Token))
		{
			return false;
		}
	}

	return true;
}

