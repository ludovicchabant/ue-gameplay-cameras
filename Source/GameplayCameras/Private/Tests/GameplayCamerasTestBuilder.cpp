// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tests/GameplayCamerasTestBuilder.h"

namespace UE::Cameras::Test
{

FCameraRigAssetTestBuilder::FCameraRigAssetTestBuilder(FName Name, UObject* Outer)
{
	if (Outer == nullptr)
	{
		Outer = GetTransientPackage();
	}

	CameraRig = NewObject<UCameraRigAsset>(Outer, Name);
	TCameraObjectInitializer<UCameraRigAsset>::SetObject(CameraRig);
}

TCameraRigTransitionTestBuilder<FCameraRigAssetTestBuilder> FCameraRigAssetTestBuilder::AddEnterTransition()
{
	TCameraRigTransitionTestBuilder<ThisType> TransitionBuilder(*this, CameraRig);
	CameraRig->EnterTransitions.Add(TransitionBuilder.Get());
	return TransitionBuilder;
}

TCameraRigTransitionTestBuilder<FCameraRigAssetTestBuilder> FCameraRigAssetTestBuilder::AddExitTransition()
{
	TCameraRigTransitionTestBuilder<ThisType> TransitionBuilder(*this, CameraRig);
	CameraRig->ExitTransitions.Add(TransitionBuilder.Get());
	return TransitionBuilder;
}

FCameraRigAssetTestBuilder& FCameraRigAssetTestBuilder::ExposeParameter(const FString& ParameterName, UCameraNode* Target, FName TargetPropertyName)
{
	UCameraRigInterfaceParameter* InterfaceParameter = NewObject<UCameraRigInterfaceParameter>(CameraRig);
	InterfaceParameter->InterfaceParameterName = ParameterName;
	InterfaceParameter->Target = Target;
	InterfaceParameter->TargetPropertyName = TargetPropertyName;
	CameraRig->Interface.InterfaceParameters.Add(InterfaceParameter);
	return *this;
}

}  // namespace UE::Cameras::Test

