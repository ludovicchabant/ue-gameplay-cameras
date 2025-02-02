// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Tracks/MovieScenePropertyTrack.h"

#include "MovieSceneCameraFramingZoneTrack.generated.h"

UCLASS( MinimalAPI )
class UMovieSceneCameraFramingZoneTrack : public UMovieScenePropertyTrack
{
	GENERATED_BODY()

public:

	UMovieSceneCameraFramingZoneTrack(const FObjectInitializer& Init);
	
	// UMovieSceneTrack interface.
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual UMovieSceneSection* CreateNewSection() override;
};

