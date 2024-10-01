// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraParameterReader.h"
#include "Core/CameraParameters.h"
#include "Core/CameraVariableReferences.h"
#include "Nodes/Framing/CameraFramingZone.h"
#include "Math/CameraFramingZoneMath.h"
#include "Math/CriticalDamper.h"

#include "BaseFramingCameraNode.generated.h"

class FArchive;
class UVector3dCameraVariable;

/**
 * The base class for a standard scren-space framing camera node.
 */
UCLASS(MinimalAPI, Abstract, meta=(CameraNodeCategories="Framing"))
class UBaseFramingCameraNode : public UCameraNode
{
	GENERATED_BODY()

public:

	/**
	 * A camera variable providing the location of the target to frame. If unspecified,
	 * the player pawn's location will be used by default.
	 */
	UPROPERTY(EditAnywhere, Category="Target")
	FVector3dCameraVariableReference TargetLocation;

	/** The ideal horizontal screen-space position of the target. */
	UPROPERTY(EditAnywhere, Category="Framing Target")
	FDoubleCameraParameter HorizontalFraming;

	/** The ideal vertical screen-space position of the target. */
	UPROPERTY(EditAnywhere, Category="Framing Target")
	FDoubleCameraParameter VerticalFraming;

	/** The damping factor for how fast the framing recenters on the target. */
	UPROPERTY(EditAnywhere, Category="Framing Target")
	FFloatCameraParameter ReframeDampingFactor;

	/** 
	 * If valid, the recentering damping factor will interpolate between LowReframeDampingFactor 
	 * and ReframeDampingFactor as the target moves between the ideal target position and the
	 * boundaries of the hard-zone. If invalid, no interpolation occurs and the damping factor
	 * is always equal to ReframeDampingFactor. */
	UPROPERTY(EditAnywhere, Category="Framing Target")
	FFloatCameraParameter LowReframeDampingFactor;

	/**
	 * The distance from the ideal framing position at which we can disengage reframing.
	 * This should be a very small value, but if it is too small the reframing will keep "chasing"
	 * the target for a long time even if it stays in the dead zone.
	 */
	UPROPERTY(EditAnywhere, Category="Framing Target")
	FFloatCameraParameter ReframeUnlockRadius;

	/** 
	 * The margins of the dead zone, i.e. the zone inside which the target can freely move.
	 * Margins are expressed in screen percentages from the edges.
	 */
	UPROPERTY(EditAnywhere, Category="Framing Zones")
	FCameraFramingZone DeadZone;

	/**
	 * The margins of the soft zone, i.e. the zone inside which the reframing will engage, in order
	 * to bring the target back towards the ideal framing position. If the target is outside of the
	 * soft zone, it will be forcibly and immedialy brought back to its edges, so this zone also 
	 * defines the "hard" or "safe" zone of framing.
	 * Margins are expressed in screen percentages from the edges.
	 */
	UPROPERTY(EditAnywhere, Category="Framing Zones")
	FCameraFramingZone SoftZone;

public:

	UBaseFramingCameraNode(const FObjectInitializer& ObjectInit);
};

namespace UE::Cameras
{

class FCameraVariableTable;

/**
 * Utility struct for reading a framing zone's margin parameters.
 */
struct FCameraFramingZoneParameterReader
{
public:

	TCameraParameterReader<double> LeftMargin;
	TCameraParameterReader<double> TopMargin;
	TCameraParameterReader<double> RightMargin;
	TCameraParameterReader<double> BottomMargin;

public:

	void Initialize(const FCameraFramingZone& FramingZone);
	FFramingZoneMargins GetZoneMargins(const FCameraVariableTable& VariableTable) const;
};

/**
 * The base class for a framing camera node evaluator.
 *
 * This evaluator does nothing per se but provides utility functions to be called in 
 * a sub-class' OnRun method. Namely:
 *
 * - UpdateFramingState() : computes the current state of the framing node. The result
 *			can be obtained from the State field.
 *
 * - ComputeDesiredState() : one the current state has been written, this method computes
 *			the desired framing state for the current tick, including the desired framing
 *			correction. It is up to the sub-class to implement the necessary logic to
 *			honor this correction. For instance, a dolly shot would translate left/right 
 *			(and maybe up/down too) to try and reframe things accordingly, whereas a panning
 *			shot would rotate the camera left/right/up/down to accomplish the same.
 *
 * - RegisterNewFraming() : once the framing correction has been executed by the sub-class,
 *			it's important to register the new camera transform with RegisterNewFraming, 
 *			otherwise the reframing will always act only on the incoming camera pose! If this
 *			incoming camera pose is fixed (e.g. the previous nodes are only fixed offsets)
 *			then if RegisterNewFraming isn't called, the reframing will always do the same
 *			thing every frame!
 */
class FBaseFramingCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FBaseFramingCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

protected:

	/** Gets the target location. */
	TOptional<FVector3d> AcquireTargetLocation(const FCameraNodeEvaluationParams& Params, const FCameraNodeEvaluationResult& InResult);
	/** Updates the framing state for the current tick, see State member field. */
	void UpdateFramingState(const FCameraNodeEvaluationParams& Params, const FCameraNodeEvaluationResult& OutResult, const FVector3d& TargetLocation, const FTransform3d& LastFraming);
	/** Computes the desired reframing for the current tick, see Desired member field. */
	void ComputeDesiredState(float DeltaTime);

private:

	FVector2d GetHardReframeCoords() const;

protected:

	/** The current location of the target. */
	enum class ETargetFramingState
	{
		/**
		 * The target is in the dead zone, i.e. it can roam freely unless we have an 
		 * active reframing to finish.
		 */
		InDeadZone,
		/**
		 * The target is in the soft zone, i.e. we will attempt to gently bring it back 
		 * to the ideal framing position.
		 */
		InSoftZone,
		/**
		 * The target is in the hard zone, i.e. it has exited the soft zone and we need
		 * to bring it back ASAP.
		 */
		InHardZone
	};

	/** Utility structure for all the parameter readers we need every frame. */
	struct FReaders
	{
		TCameraParameterReader<double> HorizontalFraming;
		TCameraParameterReader<double> VerticalFraming;
		TCameraParameterReader<float> ReframeDampingFactor;
		TCameraParameterReader<float> LowReframeDampingFactor;
		TCameraParameterReader<float> ReframeUnlockRadius;

		FCameraFramingZoneParameterReader DeadZoneMargin;
		FCameraFramingZoneParameterReader SoftZoneMargin;
	};
	FReaders Readers;

	/** Utility struct for storing the current known state. */
	struct FState
	{
		/** World position of the tracked target. */
		FVector3d WorldTarget;

		/** Screen-space position of the ideal framing position. */
		FVector2d IdealTarget;
		/** Current reframing damping factor. */
		float ReframeDampingFactor;
		/** Current low reframing damping factor. */
		float LowReframeDampingFactor;
		/** Current reframe unlock radius. */
		float ReframeUnlockRadius;
		/** Current coordinates of the dead zone. */
		FFramingZone DeadZone;
		/** Current coordinates of the soft zone. */
		FFramingZone SoftZone;

		/** Current screen-space position of the tracked target. */
		FVector2d ScreenTarget;
		/** Current state of the tracked target. */
		ETargetFramingState TargetFramingState;
		/** Whether we are actively trying to bring the target back to the ideal position. */
		bool bIsReframingTarget = false;

		/** The damper for reframing from the soft zone. */
		FCriticalDamper ReframeDamper;

		void Serialize(FArchive& Ar);
	};
	FState State;

	/** Utility struct for the desired reframing to be done in the current tick. */
	struct FDesired
	{
		/**
		 * The desired screen-space position of the tracked target. For instance, if the target
		 * is in the soft zone, this desired position will be the next step to get us closer to
		 * the ideal position.
		 */
		FVector2d ScreenTarget;
		/** 
		 * The screen-space correction we want this tick.
		 * This is effectively equal to: Desired.ScreenTarget - State.ScreenTarget
		 */
		FVector2d FramingCorrection;
		/**
		 * Whether we have any correction to do.
		 */
		bool bHasCorrection = false;

		void Serialize(FArchive& Ar);
	};
	FDesired Desired;

	friend class FBaseFramingCameraDebugBlock;
	friend FArchive& operator <<(FArchive& Ar, FState& State);
	friend FArchive& operator <<(FArchive& Ar, FDesired& Desired);
};

FArchive& operator <<(FArchive& Ar, FBaseFramingCameraNodeEvaluator::FState& State);
FArchive& operator <<(FArchive& Ar, FBaseFramingCameraNodeEvaluator::FDesired& Desired);

}  // namespace UE::Cameras

