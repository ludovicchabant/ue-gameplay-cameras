// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraAsset.h"
#include "Core/CameraObjectInterfaceParameterDefinition.h"
#include "Core/CameraParameters.h"
#include "Misc/AssertionMacros.h"
#include "Misc/AutomationTest.h"
#include "Nodes/Common/LensParametersCameraNode.h"
#include "Nodes/Common/OffsetCameraNode.h"
#include "StructUtils/PropertyBag.h"
#include "Tests/GameplayCamerasTestBuilder.h"
#include "Tests/GameplayCamerasTestObjects.h"

#define LOCTEXT_NAMESPACE "CameraAssetBuilderTests"

namespace UE::Cameras::Test
{

template<typename ParameterType>
bool CheckDefaultParameter(const FInstancedPropertyBag& PropertyBag, const FName ParameterName, typename TCallTraits<typename ParameterType::ValueType>::ParamType ExpectedValue)
{
	TValueOrError<ParameterType*, EPropertyBagResult> ValueOrError = PropertyBag.GetValueStruct<ParameterType>(ParameterName);
	if (!ValueOrError.HasError() || ValueOrError.HasValue())
	{
		const ParameterType* Value = ValueOrError.GetValue();
		return Value->Value == ExpectedValue;
	}
	return false;
}

}  // namespace UE::Cameras::Test

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCameraAssetBuilderDefaultParametersTest, "System.Engine.GameplayCameras.CameraAssetBuilder.DefaultParameters", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCameraAssetBuilderDefaultParametersTest::RunTest(const FString& Parameters)
{
	using namespace UE::Cameras::Test;

	UFixedTestCameraDirector* CameraDirector = nullptr;
	UCameraAsset* Camera = FCameraAssetTestBuilder()
		.MakeDirector<UFixedTestCameraDirector>()
			.Pin(CameraDirector)
			.Done()
		.Get();

	UCameraRigAsset* CameraRigOne = FCameraRigAssetTestBuilder(TEXT("One"))
		.MakeRootNode<UOffsetCameraNode>().Named(TEXT("Offset"))
			.SetParameter(&UOffsetCameraNode::TranslationOffset, FVector3d(10.0, 20.0, 0.0))
			.Done()
		.AddBlendableParameter(TEXT("TranslationOffset"), ECameraVariableType::Vector3d, TEXT("Offset"), GET_MEMBER_NAME_CHECKED(UOffsetCameraNode, TranslationOffset))
		.Get();
	CameraDirector->AddCameraRig(CameraRigOne, TEXT("One"));

	UCameraRigAsset* CameraRigTwo = FCameraRigAssetTestBuilder(TEXT("Two"))
		.MakeRootNode<ULensParametersCameraNode>().Named(TEXT("Lens"))
			.SetParameter(&ULensParametersCameraNode::FocalLength, 55.0f)
			.Done()
		.AddBlendableParameter(TEXT("FocalLength"), ECameraVariableType::Float, TEXT("Lens"), GET_MEMBER_NAME_CHECKED(ULensParametersCameraNode, FocalLength))
		.Get();
	CameraDirector->AddCameraRig(CameraRigTwo, TEXT("Two"));

	Camera->BuildCamera();

	TConstArrayView<FCameraObjectInterfaceParameterDefinition> ParameterDefinitions = Camera->GetParameterDefinitions();
	UTEST_EQUAL("NumParameters", ParameterDefinitions.Num(), 2);
	UTEST_EQUAL("Parameter_Name", ParameterDefinitions[0].ParameterName, FName("TranslationOffset"));
	UTEST_EQUAL("Parameter_Type", ParameterDefinitions[0].ParameterType, ECameraObjectInterfaceParameterType::Blendable);
	UTEST_EQUAL("Parameter_VariableType", ParameterDefinitions[0].VariableType, ECameraVariableType::Vector3d);
	UTEST_EQUAL("Parameter_Name", ParameterDefinitions[1].ParameterName, FName("FocalLength"));
	UTEST_EQUAL("Parameter_Type", ParameterDefinitions[1].ParameterType, ECameraObjectInterfaceParameterType::Blendable);
	UTEST_EQUAL("Parameter_VariableType", ParameterDefinitions[1].VariableType, ECameraVariableType::Float);

	const FInstancedPropertyBag& DefaultParameters = Camera->GetDefaultParameters();
	UTEST_TRUE("Parameter_Default", CheckDefaultParameter<FVector3dCameraParameter>(DefaultParameters, TEXT("TranslationOffset"), FVector3d(10.0, 20.0, 0.0)));
	UTEST_TRUE("Parameter_Default", CheckDefaultParameter<FFloatCameraParameter>(DefaultParameters, TEXT("FocalLength"), 55.f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCameraAssetBuilderDefaultParametersCollisionTest, "System.Engine.GameplayCameras.CameraAssetBuilder.DefaultParametersCollision", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCameraAssetBuilderDefaultParametersCollisionTest::RunTest(const FString& Parameters)
{
	using namespace UE::Cameras::Test;

	UFixedTestCameraDirector* CameraDirector = nullptr;
	UCameraAsset* Camera = FCameraAssetTestBuilder()
		.MakeDirector<UFixedTestCameraDirector>()
			.Pin(CameraDirector)
			.Done()
		.Get();

	UCameraRigAsset* CameraRigOne = FCameraRigAssetTestBuilder(TEXT("One"))
		.MakeRootNode<UOffsetCameraNode>().Named(TEXT("Offset"))
			.SetParameter(&UOffsetCameraNode::TranslationOffset, FVector3d(10.0, 20.0, 0.0))
			.Done()
		.AddBlendableParameter(TEXT("TranslationOffset"), ECameraVariableType::Vector3d, TEXT("Offset"), GET_MEMBER_NAME_CHECKED(UOffsetCameraNode, TranslationOffset))
		.AddBlendableParameter(TEXT("RotationOffset"), ECameraVariableType::Rotator3d, TEXT("Offset"), GET_MEMBER_NAME_CHECKED(UOffsetCameraNode, RotationOffset))
		.Get();
	CameraDirector->AddCameraRig(CameraRigOne, TEXT("One"));

	UCameraRigAsset* CameraRigTwo = FCameraRigAssetTestBuilder(TEXT("Two"))
		.MakeRootNode<ULensParametersCameraNode>().Named(TEXT("Lens"))
			.SetParameter(&ULensParametersCameraNode::FocalLength, 55.0f)
			.Done()
		.AddBlendableParameter(TEXT("FocalLength"), ECameraVariableType::Float, TEXT("Lens"), GET_MEMBER_NAME_CHECKED(ULensParametersCameraNode, FocalLength))
		.AddBlendableParameter(TEXT("Aperture"), ECameraVariableType::Float, TEXT("Lens"), GET_MEMBER_NAME_CHECKED(ULensParametersCameraNode, Aperture))
		.Get();
	CameraDirector->AddCameraRig(CameraRigTwo, TEXT("Two"));

	UCameraRigAsset* CameraRigThree = FCameraRigAssetTestBuilder(TEXT("Three"))
		.MakeArrayRootNode()
			.AddChild<UOffsetCameraNode>(&UArrayCameraNode::Children).Named(TEXT("Offset2"))
				.SetParameter(&UOffsetCameraNode::TranslationOffset, FVector3d(0.0, 10.0, 20.0))
				.Done()
			.AddChild<ULensParametersCameraNode>(&UArrayCameraNode::Children).Named(TEXT("Lens2"))
				.SetParameter(&ULensParametersCameraNode::FocalLength, 35.0f)
				.Done()
			.Done()
		.AddBlendableParameter(TEXT("TranslationOffset"), ECameraVariableType::Vector3d, TEXT("Offset2"), GET_MEMBER_NAME_CHECKED(UOffsetCameraNode, TranslationOffset))
		.AddBlendableParameter(TEXT("FocalLength"), ECameraVariableType::Float, TEXT("Lens2"), GET_MEMBER_NAME_CHECKED(ULensParametersCameraNode, FocalLength))
		.Get();
	CameraDirector->AddCameraRig(CameraRigThree, TEXT("Three"));

	Camera->BuildCamera();

	TConstArrayView<FCameraObjectInterfaceParameterDefinition> ParameterDefinitions = Camera->GetParameterDefinitions();
	UTEST_EQUAL("NumParameters", ParameterDefinitions.Num(), 6);
	UTEST_EQUAL("Parameter_Name", ParameterDefinitions[0].ParameterName, FName("One_TranslationOffset"));
	UTEST_EQUAL("Parameter_Type", ParameterDefinitions[0].ParameterType, ECameraObjectInterfaceParameterType::Blendable);
	UTEST_EQUAL("Parameter_VariableType", ParameterDefinitions[0].VariableType, ECameraVariableType::Vector3d);
	UTEST_EQUAL("Parameter_Name", ParameterDefinitions[1].ParameterName, FName("RotationOffset"));
	UTEST_EQUAL("Parameter_Type", ParameterDefinitions[1].ParameterType, ECameraObjectInterfaceParameterType::Blendable);
	UTEST_EQUAL("Parameter_VariableType", ParameterDefinitions[1].VariableType, ECameraVariableType::Rotator3d);
	UTEST_EQUAL("Parameter_Name", ParameterDefinitions[2].ParameterName, FName("Two_FocalLength"));
	UTEST_EQUAL("Parameter_Type", ParameterDefinitions[2].ParameterType, ECameraObjectInterfaceParameterType::Blendable);
	UTEST_EQUAL("Parameter_VariableType", ParameterDefinitions[2].VariableType, ECameraVariableType::Float);
	UTEST_EQUAL("Parameter_Name", ParameterDefinitions[3].ParameterName, FName("Aperture"));
	UTEST_EQUAL("Parameter_Type", ParameterDefinitions[3].ParameterType, ECameraObjectInterfaceParameterType::Blendable);
	UTEST_EQUAL("Parameter_VariableType", ParameterDefinitions[3].VariableType, ECameraVariableType::Float);
	UTEST_EQUAL("Parameter_Name", ParameterDefinitions[4].ParameterName, FName("Three_TranslationOffset"));
	UTEST_EQUAL("Parameter_Type", ParameterDefinitions[4].ParameterType, ECameraObjectInterfaceParameterType::Blendable);
	UTEST_EQUAL("Parameter_VariableType", ParameterDefinitions[4].VariableType, ECameraVariableType::Vector3d);
	UTEST_EQUAL("Parameter_Name", ParameterDefinitions[5].ParameterName, FName("Three_FocalLength"));
	UTEST_EQUAL("Parameter_Type", ParameterDefinitions[5].ParameterType, ECameraObjectInterfaceParameterType::Blendable);
	UTEST_EQUAL("Parameter_VariableType", ParameterDefinitions[5].VariableType, ECameraVariableType::Float);

	const FInstancedPropertyBag& DefaultParameters = Camera->GetDefaultParameters();
	UTEST_TRUE("Parameter_Default", CheckDefaultParameter<FVector3dCameraParameter>(DefaultParameters, TEXT("One_TranslationOffset"), FVector3d(10.0, 20.0, 0.0)));
	UTEST_TRUE("Parameter_Default", CheckDefaultParameter<FRotator3dCameraParameter>(DefaultParameters, TEXT("RotationOffset"), FRotator(0.0, 0.0, 0.0)));
	UTEST_TRUE("Parameter_Default", CheckDefaultParameter<FFloatCameraParameter>(DefaultParameters, TEXT("Two_FocalLength"), 55.f));
	UTEST_TRUE("Parameter_Default", CheckDefaultParameter<FFloatCameraParameter>(DefaultParameters, TEXT("Aperture"), 0.f));
	UTEST_TRUE("Parameter_Default", CheckDefaultParameter<FVector3dCameraParameter>(DefaultParameters, TEXT("Three_TranslationOffset"), FVector3d(0.0, 10.0, 20.0)));
	UTEST_TRUE("Parameter_Default", CheckDefaultParameter<FFloatCameraParameter>(DefaultParameters, TEXT("Three_FocalLength"), 35.f));

	return true;
}

#undef LOCTEXT_NAMESPACE

