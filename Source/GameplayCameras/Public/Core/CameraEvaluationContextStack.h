// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "GameplayCameras.h"
#include "UObject/WeakObjectPtr.h"

class UCameraAsset;
class UCameraDirector;

namespace UE::Cameras
{

class FCameraDirectorEvaluator;
class FCameraEvaluationContext;
class FCameraSystemEvaluator;

/**
 * A simple stack of evaluation contexts. The top one is the active one.
 */
struct FCameraEvaluationContextStack
{
public:

	~FCameraEvaluationContextStack();

public:

	/** Gets the active (top) context. */
	TSharedPtr<FCameraEvaluationContext> GetActiveContext() const;

	/** Returns whether the given context exists in the stack. */
	bool HasContext(TSharedRef<FCameraEvaluationContext> Context) const;

	/** Push a new context on the stack and instantiate its director. */
	void PushContext(TSharedRef<FCameraEvaluationContext> Context);

	bool AddChildContext(TSharedRef<FCameraEvaluationContext> Context);

	/** Remove an existing context from the stack. */
	bool RemoveContext(TSharedRef<FCameraEvaluationContext> Context);

	/** Pop the active (top) context. */
	void PopContext();

	/** The number of contexts on the stack. */
	int32 NumContexts() const { return Entries.Num(); }

	/** Empties the stack of all contexts. */
	void Reset();

public:

	// Internal API
	void Initialize(FCameraSystemEvaluator& InEvaluator);
	void AddReferencedObjects(FReferenceCollector& Collector);

private:

	struct FContextEntry
	{
		TWeakPtr<FCameraEvaluationContext> WeakContext;
	};

	/** The entries in the stack. */
	TArray<FContextEntry> Entries;

	/** The owner evaluator. */
	FCameraSystemEvaluator* Evaluator = nullptr;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	friend class FCameraDirectorTreeDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

}  // namespace UE::Cameras

