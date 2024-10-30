// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tests/GameplayCamerasTestBuilder.h"

namespace UE::Cameras::Test
{

FCameraRigAssetTestBuilder::FCameraRigAssetTestBuilder(FName Name, UObject* Outer)
	: TCameraRigAssetTestBuilderBase<FCameraRigAssetTestBuilder>(nullptr, Name, Outer)
{
}

FCameraRigAssetTestBuilder::FCameraRigAssetTestBuilder(TSharedPtr<FNamedObjectRegistry> InNamedObjectRegistry, FName Name, UObject* Outer)
	: TCameraRigAssetTestBuilderBase<FCameraRigAssetTestBuilder>(InNamedObjectRegistry, Name, Outer)
{
}

FCameraEvaluationContextTestBuilder::FCameraEvaluationContextTestBuilder(UObject* Owner)
{
	if (Owner == nullptr)
	{
		Owner = GetTransientPackage();
	}

	CameraAsset = NewObject<UCameraAsset>(Owner);

	FCameraEvaluationContextInitializeParams InitParams;
	InitParams.Owner = Owner;
	InitParams.CameraAsset = CameraAsset;
	EvaluationContext = MakeShared<FCameraEvaluationContext>(InitParams);

	TCameraObjectInitializer<FCameraEvaluationContext>::SetObject(EvaluationContext.Get());

	NamedObjectRegistry = MakeShared<FNamedObjectRegistry>();
}

}  // namespace UE::Cameras::Test

