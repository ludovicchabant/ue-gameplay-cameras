// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraEvaluationService.h"

namespace UE::Cameras
{

UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(FCameraEvaluationService)

FCameraEvaluationService::FCameraEvaluationService()
{
}

FCameraEvaluationService::~FCameraEvaluationService()
{
}

void FCameraEvaluationService::Initialize(const FCameraEvaluationServiceInitializeParams& Params)
{
	OnInitialize(Params);
}

void FCameraEvaluationService::PreUpdate(const FCameraEvaluationServiceUpdateParams& Params, FCameraEvaluationServiceUpdateResult& OutResult)
{
	OnPreUpdate(Params, OutResult);
}

void FCameraEvaluationService::PostUpdate(const FCameraEvaluationServiceUpdateParams& Params, FCameraEvaluationServiceUpdateResult& OutResult)
{
	OnPostUpdate(Params, OutResult);
}

void FCameraEvaluationService::Teardown(const FCameraEvaluationServiceTeardownParams& Params)
{
	OnTeardown(Params);
}

void FCameraEvaluationService::AddReferencedObjects(FReferenceCollector& Collector)
{
	OnAddReferencedObjects(Collector);
}

void FCameraEvaluationService::NotifyRootCameraNodeEvent(const FRootCameraNodeCameraRigEvent& InEvent)
{
	OnRootCameraNodeEvent(InEvent);
}

bool FCameraEvaluationService::HasAllEvaluationServiceFlags(ECameraEvaluationServiceFlags InFlags) const
{
	return EnumHasAllFlags(PrivateFlags, InFlags);
}

void FCameraEvaluationService::SetEvaluationServiceFlags(ECameraEvaluationServiceFlags InFlags)
{
	PrivateFlags = InFlags;
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FCameraEvaluationService::BuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	OnBuildDebugBlocks(Params, Builder);
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras
