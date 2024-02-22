// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraModeTransition.h"
#include "CoreTypes.h"
#include "UObject/ObjectPtr.h"

#include "CameraMode.generated.h"

class UCameraNode;

/**
 *
 */
UENUM()
enum class ECameraModeBuildStatus : uint8
{
	Clean,
	CleanWithWarnings,
	WithErrors,
	Dirty
};

/**
 * List of packages that contain the definition of a camera mode.
 * In most cases there's only one, but with nested assets there could be more.
 */
using FCameraModePackages = TArray<const UPackage*, TInlineAllocator<4>>;

/**
 * A camera mode asset, which runs a hierarchy of camera nodes to drive 
 * the behavior of a camera.
 */
UCLASS(MinimalAPI)
class UCameraMode : public UObject
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	GAMEPLAYCAMERAS_API void GatherPackages(FCameraModePackages& OutPackages) const;
#endif  // WITH_EDITOR

public:

	/** Root camera node. */
	UPROPERTY(EditAnywhere, Instanced, Category=Common)
	TObjectPtr<UCameraNode> RootNode;

	/** List of enter transitions for this camera mode. */
	UPROPERTY(EditAnywhere, Category=Blending)
	TArray<FCameraModeTransition> EnterTransitions;

	/** List of exist transitions for this camera mode. */
	UPROPERTY(EditAnywhere, Category=Blending)
	TArray<FCameraModeTransition> ExitTransitions;

	UPROPERTY(Transient)
	ECameraModeBuildStatus BuildStatus = ECameraModeBuildStatus::Dirty;
};

