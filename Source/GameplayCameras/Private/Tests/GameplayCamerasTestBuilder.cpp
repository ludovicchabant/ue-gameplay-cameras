// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tests/GameplayCamerasTestBuilder.h"

namespace UE::Cameras::Test
{

FCameraRigAssetTestBuilder::FCameraRigAssetTestBuilder(FName Name, UObject* Outer)
{
	Initialize(nullptr, Name, Outer);
}

FCameraRigAssetTestBuilder::FCameraRigAssetTestBuilder(TSharedRef<FNamedObjectRegistry> InNamedObjectRegistry, FName Name, UObject* Outer)
{
	Initialize(InNamedObjectRegistry, Name, Outer);
}

void FCameraRigAssetTestBuilder::Initialize(TSharedPtr<FNamedObjectRegistry> InNamedObjectRegistry, FName Name, UObject* Outer)
{
	if (Outer == nullptr)
	{
		Outer = GetTransientPackage();
	}

	CameraRig = NewObject<UCameraRigAsset>(Outer, Name);
	TCameraObjectInitializer<UCameraRigAsset>::SetObject(CameraRig);

	NamedObjectRegistry = InNamedObjectRegistry;
	if (!NamedObjectRegistry)
	{
		NamedObjectRegistry = MakeShared<FNamedObjectRegistry>();
	}
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
	NamedObjectRegistry->Register(InterfaceParameter, ParameterName);
	CameraRig->Interface.InterfaceParameters.Add(InterfaceParameter);
	return *this;
}

FCameraRigAssetTestBuilder& FCameraRigAssetTestBuilder::ExposeParameter(const FString& ParameterName, const FString& TargetName, FName TargetPropertyName)
{
	UCameraNode* Target = NamedObjectRegistry->Get<UCameraNode>(TargetName);
	ensure(Target);
	return ExposeParameter(ParameterName, Target, TargetPropertyName);
}

}  // namespace UE::Cameras::Test

