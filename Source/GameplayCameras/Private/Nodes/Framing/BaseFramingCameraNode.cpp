// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Framing/BaseFramingCameraNode.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraParameterReader.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameplayCameras.h"
#include "HAL/IConsoleManager.h"
#include "Math/CameraPoseMath.h"
#include "Math/ColorList.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseFramingCameraNode)

namespace UE::Cameras
{

float GFramingUnlockRadiusEpsilon = 1.e-4;
static FAutoConsoleVariableRef CVarFramingUnlockRadiusEpsilon(
	TEXT("GameplayCameras.Framing.UnlockRadiusEpsilon"),
	GFramingUnlockRadiusEpsilon,
	TEXT("(Default: 0.0001. The epsilon to determine whether we have reached the framing unlock circle."));

UE_DECLARE_CAMERA_DEBUG_BLOCK_START(GAMEPLAYCAMERAS_API, FBaseFramingCameraDebugBlock)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FBaseFramingCameraNodeEvaluator::FState, State);
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FBaseFramingCameraNodeEvaluator::FDesired, Desired);
UE_DECLARE_CAMERA_DEBUG_BLOCK_END()

UE_DEFINE_CAMERA_DEBUG_BLOCK_WITH_FIELDS(FBaseFramingCameraDebugBlock)

void FCameraFramingZoneParameterReader::Initialize(const FCameraFramingZone& FramingZone)
{
	LeftMargin.Initialize(FramingZone.LeftMargin);
	TopMargin.Initialize(FramingZone.TopMargin);
	RightMargin.Initialize(FramingZone.RightMargin);
	BottomMargin.Initialize(FramingZone.BottomMargin);
}

FFramingZoneMargins FCameraFramingZoneParameterReader::GetZoneMargins(const FCameraVariableTable& VariableTable) const
{
	FFramingZoneMargins Margins;
	Margins.LeftMargin = LeftMargin.Get(VariableTable);
	Margins.TopMargin = TopMargin.Get(VariableTable);
	Margins.RightMargin = RightMargin.Get(VariableTable);
	Margins.BottomMargin = BottomMargin.Get(VariableTable);
	return Margins;
}

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBaseFramingCameraNodeEvaluator)

void FBaseFramingCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UBaseFramingCameraNode* BaseFramingNode = GetCameraNodeAs<UBaseFramingCameraNode>();

	Readers.HorizontalFraming.Initialize(BaseFramingNode->HorizontalFraming);
	Readers.VerticalFraming.Initialize(BaseFramingNode->VerticalFraming);
	Readers.ReframeDampingFactor.Initialize(BaseFramingNode->ReframeDampingFactor);
	Readers.LowReframeDampingFactor.Initialize(BaseFramingNode->LowReframeDampingFactor);
	Readers.ReframeUnlockRadius.Initialize(BaseFramingNode->ReframeUnlockRadius);

	Readers.DeadZoneMargin.Initialize(BaseFramingNode->DeadZone);
	Readers.SoftZoneMargin.Initialize(BaseFramingNode->SoftZone);
}

TOptional<FVector3d> FBaseFramingCameraNodeEvaluator::AcquireTargetLocation(const FCameraNodeEvaluationParams& Params, const FCameraNodeEvaluationResult& InResult)
{
	const UBaseFramingCameraNode* FramingNode = GetCameraNodeAs<UBaseFramingCameraNode>();
	if (FramingNode->TargetLocation)
	{
		FVector3d TargetLocation;
		const bool bGotTargetLocation = InResult.VariableTable.TryGetValue(FramingNode->TargetLocation.Get(), TargetLocation);
		return bGotTargetLocation ? TOptional<FVector3d>(TargetLocation) : TOptional<FVector3d>();
	}
	else if (APlayerController* PlayerController = Params.EvaluationContext->GetPlayerController())
	{
		APawn* Pawn = PlayerController->GetPawn();
		FVector3d TargetLocation = Pawn->GetActorLocation();
		return TOptional<FVector3d>(TargetLocation);
	}

	return TOptional<FVector3d>();
}

void FBaseFramingCameraNodeEvaluator::UpdateFramingState(const FCameraNodeEvaluationParams& Params, const FCameraNodeEvaluationResult& OutResult, const FVector3d& TargetLocation, const FTransform3d& LastFraming)
{
	// Get screen-space coordinates of the ideal framing point. These are in 0..1 UI space.
	State.IdealTarget.X = Readers.HorizontalFraming.Get(OutResult.VariableTable);
	State.IdealTarget.Y = Readers.VerticalFraming.Get(OutResult.VariableTable);

	// Update the damping factors and unlock radius in case they are driven by a variable.
	State.ReframeDampingFactor = Readers.ReframeDampingFactor.Get(OutResult.VariableTable);
	State.LowReframeDampingFactor = Readers.LowReframeDampingFactor.Get(OutResult.VariableTable);
	State.ReframeUnlockRadius = Readers.ReframeUnlockRadius.Get(OutResult.VariableTable);

	// Get the effective margins of the framing zones for this frame.
	const FFramingZoneMargins DeadZoneMargins = Readers.DeadZoneMargin.GetZoneMargins(OutResult.VariableTable);
	const FFramingZoneMargins SoftZoneMargins = Readers.SoftZoneMargin.GetZoneMargins(OutResult.VariableTable);

	// Compute the UI space coordinates of the framing zones.
	State.DeadZone = FFramingZone(DeadZoneMargins);
	State.SoftZone = FFramingZone(SoftZoneMargins);

	// We are going to reframing things iteratively, so we'll use a temporary pose defined by last frame's
	// shot transform.
	FCameraPose TempPose(OutResult.CameraPose);
	TempPose.SetTransform(LastFraming);

	// Get the target in screen-space.
	APlayerController* PlayerController = Params.EvaluationContext->GetPlayerController();
	const double AspectRatio = FCameraPoseMath::GetEffectiveAspectRatio(TempPose, PlayerController);
	const TOptional<FVector2d> ScreenTarget = FCameraPoseMath::ProjectWorldToScreen(TempPose, AspectRatio, TargetLocation, true);
	State.WorldTarget = TargetLocation;
	State.ScreenTarget = ScreenTarget.Get(FVector2d(0.5, 0.5));

	// Update the reframe damper's damping factor.
	if (State.LowReframeDampingFactor <= 0)
	{
		State.ReframeDamper.SetW0(State.ReframeDampingFactor);
	}
	else
	{
		const FVector2d IdealToCurrent(State.ScreenTarget - State.IdealTarget);
		const double UnlockEdgeToCurrent(IdealToCurrent.Length() - State.ReframeUnlockRadius);

		const FVector2d HardZonePoint = State.SoftZone.ComputeClosestIntersection(State.IdealTarget, IdealToCurrent);
		const double UnlockEdgeToHardZone = FMath::Max(
				FVector2d::Distance(State.IdealTarget, HardZonePoint) - State.ReframeUnlockRadius,
				UE_DOUBLE_SMALL_NUMBER);

		const double Alpha = FMath::Clamp(UnlockEdgeToCurrent / UnlockEdgeToHardZone, 0.0, 1.1);
		State.ReframeDamper.SetW0(FMath::Lerp(State.LowReframeDampingFactor, State.ReframeDampingFactor, Alpha));
	}

	// Make sure our framing zones are hierarchically correct: soft zone contains the dead zone, which contains
	// the ideal target.
	State.DeadZone.ClampBounds(State.IdealTarget);
	State.SoftZone.ClampBounds(State.DeadZone);

	const bool bIsInSoftZone = State.SoftZone.Contains(State.ScreenTarget);
	const bool bIsInDeadZone = State.DeadZone.Contains(State.ScreenTarget);
	if (!ScreenTarget.IsSet() || !bIsInSoftZone)
	{
		// Target is out of view or outside the soft zone -- it's therefore in the hard zone and we will
		// do everything we can to put it back in the soft zone ASAP.
		State.TargetFramingState = ETargetFramingState::InHardZone;
		State.bIsReframingTarget = true;
	}
	else if (!bIsInDeadZone)
	{
		// Target is in the soft zone so we will gently reframe it towards the ideal framing.
		State.TargetFramingState = ETargetFramingState::InSoftZone;
		State.bIsReframingTarget = true;
	}
	else
	{
		// Target is in the dead zone.
		State.TargetFramingState = ETargetFramingState::InDeadZone;

		// Even though the target is free to move inside the dead zone, we might still want to continue reframing
		// it towards the ideal position... if we didn't do that, reframing from the soft zone would stop entirely 
		// once we reach the edge of the dead zone, and we would never really ever see the target near the ideal
		// position. So if we re-enter the dead zone from the soft zone, we keep reframing until we hit a smaller
		// "unlock reframing" zone defined by the ReframeUnlockRadius.
		//
		// If reframing wasn't active in the first place (e.g. the target is freely moving inside the dead zone), 
		// we don't do anything and let it be.
		if (State.bIsReframingTarget)
		{
			// Since screen-space positions are in 0..1 space, we need to modulate vertical coordinates by the
			// aspect ratio, otherwise we end up comparing against a squished ellipse instead of a circle.
			const double DistanceToIdeal = FVector2d::Distance(
					FVector2d(State.ScreenTarget.X, (State.ScreenTarget.Y - 0.5) / AspectRatio + 0.5),
					FVector2d(State.IdealTarget.X, (State.IdealTarget.Y - 0.5) / AspectRatio + 0.5));
			// Add an epsilon to the comparison to avoid being stuck in reframing mode because of
			// floating point precision issues.
			if (DistanceToIdeal <= (State.ReframeUnlockRadius + GFramingUnlockRadiusEpsilon))
			{
				State.bIsReframingTarget = false;
			}
		}
	}
}

void FBaseFramingCameraNodeEvaluator::ComputeDesiredState(float DeltaTime)
{
	// If we  don't have any reframing to do, bail out.
	FVector2d IdealToTarget(State.ScreenTarget - State.IdealTarget);
	double DistanceToGo = IdealToTarget.Length();
	if (!State.bIsReframingTarget || DistanceToGo == 0.f)
	{
		ensure(State.TargetFramingState == ETargetFramingState::InDeadZone);
		Desired.ScreenTarget = State.ScreenTarget;
		Desired.FramingCorrection = FVector2d::ZeroVector;
		Desired.bHasCorrection = false;
		return;
	}

	if (State.TargetFramingState == ETargetFramingState::InHardZone)
	{
		// Bring the target immediately to edge of the soft zone, in the direction of the 
		// ideal position. From there, follow-up with applying the soft zone effect.
		Desired.ScreenTarget = GetHardReframeCoords();

		IdealToTarget = (Desired.ScreenTarget - State.IdealTarget);
		DistanceToGo = IdealToTarget.Length();
	}

	// Move the target towards the ideal position using some damping.
	// Remove the radius of the unlock zone from the distance we pass to the damper, 
	// otherwise the damper won't ever get to smoothly ease out to zero.
	double DampingDistanceToGo = DistanceToGo - State.ReframeUnlockRadius;
	const double NewDampedDistanceToGo = State.ReframeDamper.Update(DampingDistanceToGo, DeltaTime);
	const double NewDistanceToGo = NewDampedDistanceToGo + State.ReframeUnlockRadius;

	// Compute where we want the target this frame.
	const FVector2d InvReframeDir(IdealToTarget / DistanceToGo);
	const FVector2d NewScreenTarget = State.IdealTarget + InvReframeDir * NewDistanceToGo;
	Desired.ScreenTarget = NewScreenTarget;

	Desired.FramingCorrection = Desired.ScreenTarget - State.ScreenTarget;
	Desired.bHasCorrection = true;
}

FVector2d FBaseFramingCameraNodeEvaluator::GetHardReframeCoords() const
{
	// The target is in the hard zone and must be brought back to the edge of the soft zone.
	// Let's compute the diagonal between the target and the ideal framing point, and bring
	// the target where that diagonal intersects the soft zone.
	const FVector2d Diagonal(State.IdealTarget - State.ScreenTarget);
	if (Diagonal.X == 0.f && Diagonal.Y == 0.f)
	{
		// Somehow we're already on the desired framing. This shouldn't happen, we're supposed
		// to be in the hard zone right now...
		ensure(false);
		return State.ScreenTarget;
	}
	if (Diagonal.X == 0.f)
	{
		// The target is directly above/below the ideal position.
		return FVector2d(
				State.IdealTarget.X,
				(Diagonal.Y > 0 ? State.SoftZone.TopBound : State.SoftZone.BottomBound)
				);
	}
	else if (Diagonal.Y == 0.f)
	{
		// The target is directly to the left/right of the ideal position.
		return FVector2d(
				(Diagonal.X > 0 ? State.SoftZone.LeftBound : State.SoftZone.RightBound),
				State.IdealTarget.Y
				);
	}
	else
	{
		// The diagonal's equation is P = V*d + P0
		//
		//		V = the unit direction vector of the diagonal
		//		d = the distance
		//		P0 = a known reference point (we can use the ideal target for this)
		//
		// We want to find P where P is on the edge of the soft zone. This means we know
		// either P.x or P.y depending on the edge. 
		//
		// Let's say we deal with a vertical edge... then we know about P.x, because we
		// want P to be on that edge. We can therefor compute d:
		//
		//		P.x = V.x*d + P0.x
		//		d = (P.x - P0.x) / V.x
		//
		// If it's a horizontal edge, we can do the same:
		//
		//		P.y = V.y*d + P0.y
		//		d = (P.y - P0.y) / V.y
		//
		// Keep going around the zone until we're sure the point is on the edge.
		//
		const FVector2d P0(State.IdealTarget);
		const FVector2d V(Diagonal.GetSafeNormal());
		double D = 0.f;
		FVector2d P(State.ScreenTarget);

		if (P.X < State.SoftZone.LeftBound)
		{
			D = (State.SoftZone.LeftBound - P0.X) / V.X;
			P = V * D + P0;
		}
		if (P.Y < State.SoftZone.TopBound)
		{
			D = (State.SoftZone.TopBound - P0.Y) / V.Y;
			P = V * D + P0;
		}
		if (P.X > State.SoftZone.RightBound)
		{
			D = (State.SoftZone.RightBound - P0.X) / V.X;
			P = V * D + P0;
		}
		if (P.X > State.SoftZone.BottomBound)
		{
			D = (State.SoftZone.BottomBound - P0.Y) / V.Y;
			P = V * D + P0;
		}

		return P;
	}
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FBaseFramingCameraNodeEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	FBaseFramingCameraDebugBlock& DebugBlock = Builder.AttachDebugBlock<FBaseFramingCameraDebugBlock>();

	DebugBlock.State = State;
	DebugBlock.Desired = Desired;
}

void FBaseFramingCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	FString StateString(TEXT("Unknown"));
	switch (State.TargetFramingState)
	{
		case FBaseFramingCameraNodeEvaluator::ETargetFramingState::InDeadZone:
			StateString = TEXT("Dead Zone");
			break;
		case FBaseFramingCameraNodeEvaluator::ETargetFramingState::InSoftZone:
			StateString = TEXT("Soft Zone");
			break;
		case FBaseFramingCameraNodeEvaluator::ETargetFramingState::InHardZone:
			StateString = TEXT("Hard Zone");
			break;
	}

	Renderer.AddText(TEXT("state [%s]"), *StateString);
	if (State.bIsReframingTarget)
	{
		Renderer.AddText(TEXT("[REFRAMING]"));
	}

	Renderer.AddText(TEXT(" (damping = %0.3f, factor = %0.1f)"), State.ReframeDamper.GetX0(), State.ReframeDamper.GetW0());

	if (Renderer.HasCanvas())
	{
		const FVector2D CanvasSize = Renderer.GetCanvasSize();

		const float ReframeUnlockRadius(State.ReframeUnlockRadius * CanvasSize.X);
		const FVector2D IdealTarget(State.IdealTarget.X * CanvasSize.X, State.IdealTarget.Y * CanvasSize.Y);

		Renderer.Draw2DBox(
				State.SoftZone.GetCanvasPosition(CanvasSize), 
				State.SoftZone.GetCanvasSize(CanvasSize),
				FLinearColor::Red,
				1.f);
		Renderer.Draw2DBox(
				State.DeadZone.GetCanvasPosition(CanvasSize), 
				State.DeadZone.GetCanvasSize(CanvasSize),
				FLinearColor::Green,
				1.f);
		Renderer.Draw2DCircle(IdealTarget, ReframeUnlockRadius, FLinearColor(FColorList::PaleGreen), 1.f);

		const FVector2D FramingCorrection(Desired.FramingCorrection.X * CanvasSize.X, Desired.FramingCorrection.Y * CanvasSize.Y);

		Renderer.AddText(TEXT("  correction (%0.1f ; %0.1f)"),
				State.ReframeDamper.GetX0(),
				FramingCorrection.X, FramingCorrection.Y);

		const FVector2D ScreenTarget(State.ScreenTarget.X * CanvasSize.X, State.ScreenTarget.Y * CanvasSize.Y);
		const FVector2D NextScreenTarget(Desired.ScreenTarget.X * CanvasSize.X, Desired.ScreenTarget.Y * CanvasSize.Y);

		Renderer.AddText(TEXT("  target (%0.1f; %0.1f)"), ScreenTarget.X, ScreenTarget.Y);
		Renderer.Draw2DLine(ScreenTarget, NextScreenTarget, FLinearColor(FColorList::Salmon), 1.f);
		Renderer.Draw2DCircle(ScreenTarget, 2.f, FLinearColor(FColorList::Orange), 2.f);
		Renderer.Draw2DCircle(IdealTarget, 2.f, FLinearColor::Green, 2.f);
	}
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

void FBaseFramingCameraNodeEvaluator::FState::Serialize(FArchive& Ar)
{
	Ar << IdealTarget;
	Ar << ReframeDampingFactor;
	Ar << ReframeUnlockRadius;
	Ar << DeadZone;
	Ar << SoftZone;
	Ar << ScreenTarget;
	Ar << TargetFramingState;
	Ar << bIsReframingTarget;
	Ar << ReframeDamper;
}

void FBaseFramingCameraNodeEvaluator::FDesired::Serialize(FArchive& Ar)
{
	Ar << ScreenTarget;
	Ar << FramingCorrection;
	Ar << bHasCorrection;
}

FArchive& operator <<(FArchive& Ar, FBaseFramingCameraNodeEvaluator::FState& State)
{
	State.Serialize(Ar);
	return Ar;
}

FArchive& operator <<(FArchive& Ar, FBaseFramingCameraNodeEvaluator::FDesired& Desired)
{
	Desired.Serialize(Ar);
	return Ar;
}

}  // namespace UE::Cameras

UBaseFramingCameraNode::UBaseFramingCameraNode(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
	, DeadZone(0.45f)
	, SoftZone(0.05f)
{
	HorizontalFraming.Value = 0.5f;
	VerticalFraming.Value = 0.5f;
	ReframeDampingFactor.Value = 10.f;
	LowReframeDampingFactor.Value = -1.f;
	ReframeUnlockRadius.Value = 0.005f;
}

