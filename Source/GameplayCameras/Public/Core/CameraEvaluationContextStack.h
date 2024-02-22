// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Core/CameraDirectorEvaluator.h"
#include "UObject/WeakObjectPtr.h"

class FCameraDirectorEvaluator;
class FCameraEvaluationContext;
class FCameraSystemEvaluator;
class UCameraAsset;
class UCameraDirector;

/** Information about a running camera evaluation context. */
struct FCameraEvaluationContextInfo
{
	/** The evaluation context. */
	TSharedPtr<FCameraEvaluationContext> EvaluationContext;

	/** The instantiated camera director running in this context. */
	TObjectPtr<const UCameraDirector> CameraDirector;

	/** The evaluator for the running camera director. */
	FCameraDirectorEvaluator* Evaluator = nullptr;

	/** Returns whether this structure has a valid context and director. */
	bool IsValid() const { return EvaluationContext && CameraDirector; }
};

/**
 * A simple stack of evaluation contexts. The top one is the active one.
 */
struct FCameraEvaluationContextStack
{
public:

	/** Gets the active (top) context. */
	FCameraEvaluationContextInfo GetActiveContext() const;

	/** Returns whether the given context exists in the stack. */
	bool HasContext(TSharedRef<FCameraEvaluationContext> Context) const;

	/** Push a new context on the stack and instantiate its director. */
	void PushContext(TSharedRef<FCameraEvaluationContext> Context);

	/** Remove an existing context from the stack. */
	bool RemoveContext(TSharedRef<FCameraEvaluationContext> Context);

	/** Pop the active (top) context. */
	void PopContext();

public:

	// Internal API
	void Initialize(TSharedRef<FCameraSystemEvaluator> InEvaluator);
	void AddReferencedObjects(FReferenceCollector& Collector);

private:

	struct FContextEntry
	{
		TWeakPtr<FCameraEvaluationContext> WeakContext;
		TObjectPtr<const UCameraDirector> CameraDirector;
		FCameraDirectorEvaluatorStorage EvaluatorStorage;
		FCameraDirectorEvaluator* Evaluator = nullptr;
	};

	/** The entries in the stack. */
	TArray<FContextEntry> Entries;

	/** The owner evaluator. */
	TSharedPtr<FCameraSystemEvaluator> Evaluator;
};

