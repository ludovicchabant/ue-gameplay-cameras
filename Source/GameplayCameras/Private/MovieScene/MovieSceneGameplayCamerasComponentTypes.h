// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Nodes/Framing/CameraFramingZone.h"
#include "EntitySystem/MovieSceneEntityIDs.h"
#include "EntitySystem/MovieScenePropertySystemTypes.h"
#include "EntitySystem/MovieScenePropertyTraits.h"
#include "EntitySystem/MovieScenePropertyMetaData.h"


namespace UE::MovieScene
{
	void UnpackChannelsFromOperational(FCameraFramingZone InZone, const FProperty& Property, FUnpackedChannelValues& OutUnpackedValues);
}

namespace UE::Cameras
{

using FCameraFramingZoneTraits = UE::MovieScene::TDirectPropertyTraits<FCameraFramingZone>;

struct FMovieSceneGameplayCamerasComponentTypes
{
	GAMEPLAYCAMERAS_API ~FMovieSceneGameplayCamerasComponentTypes();

	UE::MovieScene::TComponentTypeID<FGuid> CameraParameterOverrideID;

	UE::MovieScene::TPropertyComponents<FCameraFramingZoneTraits> CameraFramingZone;

	UE::MovieScene::TCustomPropertyRegistration<FCameraFramingZoneTraits, 1> CustomCameraFramingZoneAccessors;

	static GAMEPLAYCAMERAS_API void Destroy();

	static GAMEPLAYCAMERAS_API FMovieSceneGameplayCamerasComponentTypes* Get();

private:

	FMovieSceneGameplayCamerasComponentTypes();
};

} // namespace UE::Cameras

