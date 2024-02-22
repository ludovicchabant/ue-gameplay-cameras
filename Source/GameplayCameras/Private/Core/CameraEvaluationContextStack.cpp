// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraEvaluationContextStack.h"

#include "Core/CameraAsset.h"
#include "Core/CameraDirector.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraSystemEvaluator.h"
#include "UObject/Package.h"

FCameraEvaluationContextInfo FCameraEvaluationContextStack::GetActiveContext() const
{
	for (const FContextEntry& Entry : ReverseIterate(Entries))
	{
		if (TSharedPtr<FCameraEvaluationContext> Context = Entry.WeakContext.Pin())
		{
			return FCameraEvaluationContextInfo{ Context, Entry.CameraDirector, Entry.Evaluator };
		}
	}
	return FCameraEvaluationContextInfo();
}

bool FCameraEvaluationContextStack::HasContext(TSharedRef<FCameraEvaluationContext> Context) const
{
	for (const FContextEntry& Entry : ReverseIterate(Entries))
	{
		if (Context == Entry.WeakContext)
		{
			return true;
		}
	}
	return false;
}

void FCameraEvaluationContextStack::PushContext(TSharedRef<FCameraEvaluationContext> Context)
{
	checkf(Evaluator, TEXT("Can't push context when no evaluator is set! Did you call Initialize?"));

	// If we're pushing an existing context, move it to the top.
	const int32 ExistingIndex = Entries.IndexOfByPredicate(
			[Context](const FContextEntry& Entry) { return Entry.WeakContext == Context; });
	if (ExistingIndex != INDEX_NONE)
	{
		if (ExistingIndex < Entries.Num() - 1)
		{
			FContextEntry EntryCopy(MoveTemp(Entries[ExistingIndex]));
			Entries.RemoveAt(ExistingIndex);
			Entries.Add(MoveTemp(EntryCopy));
		}
		return;
	}

	// Make a new entry and build the director evaluator using the entry's storage.
	FContextEntry NewEntry;

	FCameraDirectorEvaluatorBuilder Builder(NewEntry.EvaluatorStorage);
	const UCameraDirector* CameraDirector = Context->GetCameraAsset()->CameraDirector;
	FCameraDirectorEvaluator* DirectorEvaluator = CameraDirector->BuildEvaluator(Builder);
	
	NewEntry.WeakContext = Context;
	NewEntry.CameraDirector = CameraDirector;
	NewEntry.Evaluator = DirectorEvaluator;
	Entries.Push(MoveTemp(NewEntry));
}

bool FCameraEvaluationContextStack::RemoveContext(TSharedRef<FCameraEvaluationContext> Context)
{
	const int32 NumRemoved = Entries.RemoveAll(
			[Context](FContextEntry& Entry) { return Entry.WeakContext == Context; });
	return (NumRemoved > 0);
}

void FCameraEvaluationContextStack::PopContext()
{
	Entries.Pop();
}

void FCameraEvaluationContextStack::Initialize(TSharedRef<FCameraSystemEvaluator> InEvaluator)
{
	Evaluator = InEvaluator;
}

void FCameraEvaluationContextStack::AddReferencedObjects(FReferenceCollector& Collector)
{
	for (FContextEntry& Entry : Entries)
	{
		Collector.AddReferencedObject(Entry.CameraDirector);
	}
}

