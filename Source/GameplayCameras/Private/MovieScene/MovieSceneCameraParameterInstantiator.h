// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Misc/EngineVersionComparison.h"

#if UE_VERSION_NEWER_THAN_OR_EQUAL(5,6,0)

#include "EntitySystem/IMovieSceneEntityDecorator.h"
#include "EntitySystem/MovieSceneEntityInstantiatorSystem.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"

#include "MovieSceneCameraParameterInstantiator.generated.h"

class UMovieSceneSection;
class UMovieSceneTrack;

namespace UE::Cameras
{
	struct FPreAnimatedCameraParameterStorage;
}

/**
 * Decorator for camera parameter sections. The decorator extends the manufactured ECS entities
 * so that we flag the camera parameter as being "animated" and make the camera system apply
 * its value every frame until Sequencer lets go of it.
 */
UCLASS(MinimalAPI)
class UMovieSceneCameraParameterDecoration : public UObject, public IMovieSceneEntityDecorator
{
	GENERATED_BODY()

protected:

	virtual void ExtendEntityImpl(UMovieSceneEntitySystemLinker* EntityLinker, const FEntityImportParams& Params, FImportedEntity* OutImportedEntity) override;
};

/**
 * Sequencer system for flagging camera parameters as "animated" and make the camera system
 * apply their values every frame.
 */
UCLASS(MinimalAPI)
class UMovieSceneCameraParameterInstantiator : public UMovieSceneEntityInstantiatorSystem
{
	GENERATED_BODY()

public:

	UMovieSceneCameraParameterInstantiator(const FObjectInitializer& ObjInit);

#if WITH_EDITORONLY_DATA
	static void OnMovieSceneSectionAddedToTrack(UMovieSceneTrack* Track, UMovieSceneSection* NewSection);
#endif

private:

	virtual void OnLink() override;
	virtual void OnRun(FSystemTaskPrerequisites& InPrerequisites, FSystemSubsequentTasks& Subsequents) override final;

private:

	TSharedPtr<UE::Cameras::FPreAnimatedCameraParameterStorage> PreAnimatedStorage;
};

#endif  // UE_VERSION_NEWER_THAN_OR_EQUAL(5,6,0)

