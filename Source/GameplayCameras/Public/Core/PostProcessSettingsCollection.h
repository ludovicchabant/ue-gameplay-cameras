// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/Scene.h"

namespace UE::Cameras
{

/** An entry into an FPostProcessSettingsCollection. */
struct FPostProcessSettingsCollectionEntry
{
	/** The post-process settings. */
	FPostProcessSettings PostProcessSettings;
	/** The blend factor for the post-process settings. */
	float PostProcessBlendWeight = 1.f;
};

FArchive& operator<< (FArchive& Ar, FPostProcessSettingsCollectionEntry& Entry);

/**
 * A collection of post-process settings, with associated blend weights.
 * TODO: the camera system currently supports only one post-process setting.
 */
struct GAMEPLAYCAMERAS_API FPostProcessSettingsCollection
{
	/** Returns whether this collection has any entries. */
	bool IsEmpty() const
	{
		return Entries.IsEmpty();
	}

	/** Gets the entries in this collection. */
	TArrayView<const FPostProcessSettingsCollectionEntry> GetEntries() const
	{
		return Entries;
	}

	/** Gets the entries in this collection. */
	TArrayView<FPostProcessSettingsCollectionEntry> GetEntries() 
	{
		return Entries;
	}

	/** Adds a new entry in the collection. */
	void Add(const FPostProcessSettings& InPostProcess, float InBlendWeight);

	/** Removes all entries in the collection. */
	void Reset();

	/** 
	 * Removes all entries in the collection, replacing them with the entries in the
	 * given other collection.
	 */
	void OverrideAll(const FPostProcessSettingsCollection& OtherCollection);

	/**
	 * Interpolates the entries in this collection towards the entries in the given other
	 * collection. This effectively merges the given collection into this collection, ramping
	 * down the blend weight of existing entries, and ramping up the blend weight of the
	 * new entries.
	 */
	void LerpAll(const FPostProcessSettingsCollection& ToCollection, float BlendFactor);

	/** Serializes this collection into the given archive. */
	void Serialize(FArchive& Ar);

private:

	using FEntry = FPostProcessSettingsCollectionEntry;
	using FCollection = TArray<FEntry, TInlineAllocator<1>>;
	FCollection Entries;
};

}  // namespace UE::Cameras

