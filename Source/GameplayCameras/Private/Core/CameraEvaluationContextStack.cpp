// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraEvaluationContextStack.h"

#include "Core/CameraDirectorEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/CameraVariableTable.h"

namespace UE::Cameras
{

FCameraEvaluationContextStack::~FCameraEvaluationContextStack()
{
	Reset();
}

TSharedPtr<FCameraEvaluationContext> FCameraEvaluationContextStack::GetActiveContext() const
{
	for (const FContextEntry& Entry : ReverseIterate(Entries))
	{
		if (TSharedPtr<FCameraEvaluationContext> Context = Entry.WeakContext.Pin())
		{
			return Context;
		}
	}
	return nullptr;
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

	// Make a new entry and activate the context. This will build the director evaluator.
	FContextEntry NewEntry;

	FCameraEvaluationContextActivateParams ActivateParams;
	ActivateParams.Evaluator = Evaluator;
	Context->Activate(ActivateParams);
	
	NewEntry.WeakContext = Context;
	Entries.Push(MoveTemp(NewEntry));
}

bool FCameraEvaluationContextStack::AddChildContext(TSharedRef<FCameraEvaluationContext> Context)
{
	TSharedPtr<FCameraEvaluationContext> ActiveContext = GetActiveContext();
	if (ensureMsgf(ActiveContext.IsValid(), TEXT("Can't add child context to the stack, no active context was found!")))
	{
		FCameraDirectorEvaluator* DirectorEvaluator = ActiveContext->GetDirectorEvaluator();
		if (ensureMsgf(DirectorEvaluator, TEXT("Can't add child context, active context has no camera director evaluator!")))
		{
			return DirectorEvaluator->AddChildEvaluationContext(Context);
		}
	}
	return false;
}

bool FCameraEvaluationContextStack::RemoveContext(TSharedRef<FCameraEvaluationContext> Context)
{
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		FContextEntry& Entry = (*It);
		if (Entry.WeakContext == Context)
		{
			FCameraEvaluationContextDeactivateParams DeactivateParams;
			Context->Deactivate(DeactivateParams);

			It.RemoveCurrent();
			return true;
		}
	}
	return false;
}

void FCameraEvaluationContextStack::PopContext()
{
	Entries.Pop();
}

void FCameraEvaluationContextStack::GetAllContexts(TArray<TSharedPtr<FCameraEvaluationContext>>& OutContexts) const
{
	for (const FContextEntry& Entry : Entries)
	{
		if (TSharedPtr<FCameraEvaluationContext> Context = Entry.WeakContext.Pin())
		{
			OutContexts.Add(Context);
		}
	}
}

void FCameraEvaluationContextStack::Reset()
{
	for (FContextEntry& Entry : Entries)
	{
		if (TSharedPtr<FCameraEvaluationContext> Context = Entry.WeakContext.Pin())
		{
			FCameraEvaluationContextDeactivateParams DeactivateParams;
			Context->Deactivate(DeactivateParams);
		}
	}
	Entries.Reset();
}

void FCameraEvaluationContextStack::Initialize(FCameraSystemEvaluator& InEvaluator)
{
	Evaluator = &InEvaluator;
}

void FCameraEvaluationContextStack::AddReferencedObjects(FReferenceCollector& Collector)
{
	for (FContextEntry& Entry : Entries)
	{
		if (TSharedPtr<FCameraEvaluationContext> Context = Entry.WeakContext.Pin())
		{
			Context->AddReferencedObjects(Collector);
		}
	}
}

void FCameraEvaluationContextStack::OnEndCameraSystemUpdate()
{
	// Reset all written-this-frame flags on evaluation contexts, so we properly get those flags set
	// regardless of when, during next frame, they set their variables. This is because various 
	// gameplay systems, Blueprint scripting, whatever, might set variables at any time.
	TArray<TSharedPtr<FCameraEvaluationContext>> ContextsToVisit;
	for (const FContextEntry& Entry : Entries)
	{
		if (TSharedPtr<FCameraEvaluationContext> Context = Entry.WeakContext.Pin())
		{
			ContextsToVisit.Add(Context);
		}
	}
	while (!ContextsToVisit.IsEmpty())
	{
		TSharedPtr<FCameraEvaluationContext> Context = ContextsToVisit.Pop();
		Context->GetInitialResult().VariableTable.ClearAllWrittenThisFrameFlags();

		TArrayView<const TSharedPtr<FCameraEvaluationContext>> ChildrenContexts(Context->GetChildrenContexts());
		for (TSharedPtr<FCameraEvaluationContext> ChildContext : ReverseIterate(ChildrenContexts))
		{
			if (ChildContext)
			{
				ContextsToVisit.Add(ChildContext);
			}
		}
	}
}

}  // namespace UE::Cameras

