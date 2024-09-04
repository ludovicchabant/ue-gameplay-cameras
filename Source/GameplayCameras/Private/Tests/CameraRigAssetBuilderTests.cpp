// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAsset.h"
#include "Misc/AutomationTest.h"
#include "Nodes/Blends/SmoothBlendCameraNode.h"
#include "Nodes/Common/CameraRigCameraNode.h"
#include "Nodes/Common/LensParametersCameraNode.h"
#include "Nodes/Common/OffsetCameraNode.h"
#include "Tests/GameplayCamerasTestBuilder.h"

#define LOCTEXT_NAMESPACE "CameraRigAssetBuilderTests"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCameraRigAssetBuilderNullTest, "System.Engine.GameplayCameras.CameraRigAssetBuilder.Null", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCameraRigAssetBuilderNullTest::RunTest(const FString& Parameters)
{
	using namespace UE::Cameras::Test;

	UCameraRigAsset* CameraRig = FCameraRigAssetTestBuilder(TEXT("InvalidTest")).Get();
	UTEST_EQUAL("Dirty status", CameraRig->BuildStatus, ECameraBuildStatus::Dirty);

	FStringFormatOrderedArguments ErrorArgs;
	ErrorArgs.Add(CameraRig->GetPathName());
	AddExpectedMessage(
			FString::Format(TEXT("Camera rig '{0}' has no root node."), ErrorArgs),
			ELogVerbosity::Error,
			EAutomationExpectedMessageFlags::Contains,
			1,
			false);
	CameraRig->BuildCameraRig();
	UTEST_EQUAL("Error status", CameraRig->BuildStatus, ECameraBuildStatus::WithErrors);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCameraRigAssetBuilderSimpleAllocationTest, "System.Engine.GameplayCameras.CameraRigAssetBuilder.SimpleAllocation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCameraRigAssetBuilderSimpleAllocationTest::RunTest(const FString& Parameters)
{
	using namespace UE::Cameras;
	using namespace UE::Cameras::Test;

	UCameraRigAsset* CameraRig = FCameraRigAssetTestBuilder()
		.MakeRootNode<UArrayCameraNode>()
			.AddChild<UOffsetCameraNode>(&UArrayCameraNode::Children).Done()
			.Done()
		.Get();

	UTEST_EQUAL("No evaluator allocation info", CameraRig->AllocationInfo.EvaluatorInfo.TotalSizeof, 0);
	CameraRig->BuildCameraRig();

	int32 TotalSizeof = UArrayCameraNode::GetEvaluatorAllocationInfo().Key;
	const TTuple<int32, int32> OffsetEvaluatorInfo = UOffsetCameraNode::GetEvaluatorAllocationInfo();
	TotalSizeof = Align(TotalSizeof, OffsetEvaluatorInfo.Value) + OffsetEvaluatorInfo.Key;
	UTEST_EQUAL("Evaluator allocation info", CameraRig->AllocationInfo.EvaluatorInfo.TotalSizeof, TotalSizeof);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCameraRigAssetBuilderSimpleParameterTest, "System.Engine.GameplayCameras.CameraRigAssetBuilder.SimpleParameter", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCameraRigAssetBuilderSimpleParameterTest::RunTest(const FString& Parameters)
{
	using namespace UE::Cameras::Test;

	UOffsetCameraNode* OffsetNode = nullptr;
	UCameraRigAsset* CameraRig = FCameraRigAssetTestBuilder(TEXT("SimpleTest"))
		.MakeRootNode<UArrayCameraNode>()
			.AddChild<UOffsetCameraNode>(&UArrayCameraNode::Children)
				.Pin(OffsetNode)
				.Done()
			.Done()
		.ExposeParameter(TEXT("Test"), OffsetNode, GET_MEMBER_NAME_CHECKED(UOffsetCameraNode, TranslationOffset))
		.Get();

	CameraRig->BuildCameraRig();

	UCameraRigInterfaceParameter* Parameter = CameraRig->Interface.InterfaceParameters[0];
	UTEST_EQUAL("Test parameter", Parameter->InterfaceParameterName, TEXT("Test"));
	UTEST_NOT_NULL("Test parameter variable", Parameter->PrivateVariable.Get());
	UTEST_EQUAL("Test parameter variable name", Parameter->PrivateVariable->GetName(), "Override_SimpleTest_Test");
	UTEST_EQUAL("Test node parameter", (UCameraVariableAsset*)OffsetNode->TranslationOffset.Variable, Parameter->PrivateVariable.Get());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCameraRigAssetBuilderReassignParameterTest, "System.Engine.GameplayCameras.CameraRigAssetBuilder.ReassignParameter", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCameraRigAssetBuilderReassignParameterTest::RunTest(const FString& Parameters)
{
	using namespace UE::Cameras::Test;

	UOffsetCameraNode* OffsetNode = nullptr;
	ULensParametersCameraNode* LensParametersNode = nullptr;
	UCameraRigAsset* CameraRig = FCameraRigAssetTestBuilder(TEXT("SimpleTest"))
		.MakeRootNode<UArrayCameraNode>()
			.AddChild<UOffsetCameraNode>(&UArrayCameraNode::Children)
				.Pin(OffsetNode)
				.Done()
			.AddChild<ULensParametersCameraNode>(&UArrayCameraNode::Children)
				.Pin(LensParametersNode)
				.Done()
			.Done()
		.ExposeParameter(TEXT("Test1"), OffsetNode, GET_MEMBER_NAME_CHECKED(UOffsetCameraNode, TranslationOffset))
		.ExposeParameter(TEXT("Test2"), LensParametersNode, GET_MEMBER_NAME_CHECKED(ULensParametersCameraNode, FocalLength))
		.ExposeParameter(TEXT("Test3"), LensParametersNode, GET_MEMBER_NAME_CHECKED(ULensParametersCameraNode, Aperture))
		.Get();

	UCameraRigInterfaceParameter* Test1Parameter = CameraRig->Interface.InterfaceParameters[0];
	UCameraRigInterfaceParameter* Test2Parameter = CameraRig->Interface.InterfaceParameters[1];
	UCameraRigInterfaceParameter* Test3Parameter = CameraRig->Interface.InterfaceParameters[2];

	CameraRig->BuildCameraRig();

	{
		UTEST_EQUAL_EXPR(Test1Parameter->PrivateVariable->GetName(), "Override_SimpleTest_Test1");
		UTEST_TRUE_EXPR(Test1Parameter->PrivateVariable->IsA<UVector3dCameraVariable>());
		UTEST_EQUAL_EXPR((UCameraVariableAsset*)OffsetNode->TranslationOffset.Variable, Test1Parameter->PrivateVariable.Get());

		UTEST_EQUAL_EXPR(Test2Parameter->PrivateVariable->GetName(), "Override_SimpleTest_Test2");
		UTEST_TRUE_EXPR(Test2Parameter->PrivateVariable->IsA<UFloatCameraVariable>());
		UTEST_EQUAL_EXPR((UCameraVariableAsset*)LensParametersNode->FocalLength.Variable, Test2Parameter->PrivateVariable.Get());

		UTEST_EQUAL_EXPR(Test3Parameter->PrivateVariable->GetName(), "Override_SimpleTest_Test3");
		UTEST_TRUE_EXPR(Test3Parameter->PrivateVariable->IsA<UFloatCameraVariable>());
		UTEST_EQUAL_EXPR((UCameraVariableAsset*)LensParametersNode->Aperture.Variable, Test3Parameter->PrivateVariable.Get());
	}

	Test1Parameter->Target = LensParametersNode;
	Test1Parameter->TargetPropertyName = GET_MEMBER_NAME_CHECKED(ULensParametersCameraNode, FocalLength);
	Test2Parameter->Target = LensParametersNode;
	Test2Parameter->TargetPropertyName = GET_MEMBER_NAME_CHECKED(ULensParametersCameraNode, Aperture);
	Test3Parameter->Target = OffsetNode;
	Test3Parameter->TargetPropertyName = GET_MEMBER_NAME_CHECKED(UOffsetCameraNode, TranslationOffset);

	CameraRig->BuildCameraRig();

	{
		UTEST_EQUAL_EXPR(Test1Parameter->PrivateVariable->GetName(), "Override_SimpleTest_Test1");
		UTEST_TRUE_EXPR(Test1Parameter->PrivateVariable->IsA<UFloatCameraVariable>());
		UTEST_EQUAL_EXPR((UCameraVariableAsset*)LensParametersNode->FocalLength.Variable, Test1Parameter->PrivateVariable.Get());

		UTEST_EQUAL_EXPR(Test2Parameter->PrivateVariable->GetName(), "Override_SimpleTest_Test2");
		UTEST_TRUE_EXPR(Test2Parameter->PrivateVariable->IsA<UFloatCameraVariable>());
		UTEST_EQUAL_EXPR((UCameraVariableAsset*)LensParametersNode->Aperture.Variable, Test2Parameter->PrivateVariable.Get());

		UTEST_EQUAL_EXPR(Test3Parameter->PrivateVariable->GetName(), "Override_SimpleTest_Test3");
		UTEST_TRUE_EXPR(Test3Parameter->PrivateVariable->IsA<UVector3dCameraVariable>());
		UTEST_EQUAL_EXPR((UCameraVariableAsset*)OffsetNode->TranslationOffset.Variable, Test3Parameter->PrivateVariable.Get());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCameraRigAssetBuilderDrivenOverridesTest, "System.Engine.GameplayCameras.CameraRigAssetBuilder.DrivenOverrides", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCameraRigAssetBuilderDrivenOverridesTest::RunTest(const FString& Parameters)
{
	using namespace UE::Cameras::Test;

	TSharedRef<FNamedObjectRegistry> Registry = MakeShared<FNamedObjectRegistry>();

	// Make a camera rig with an offset node (10, 20, 30) and a focal length node (20). Expose both parameters
	// as interface parameters.
	UCameraRigAsset* InnerCameraRig = FCameraRigAssetTestBuilder(Registry, TEXT("InnerCameraRig"))
		.MakeArrayRootNode()
			.AddArrayChild<UOffsetCameraNode>().Named(TEXT("Offset"))
				.SetParameter(&UOffsetCameraNode::TranslationOffset, FVector3d(10, 20, 30))
				.Done()
			.AddArrayChild<ULensParametersCameraNode>().Named(TEXT("Lens"))
				.SetParameter(&ULensParametersCameraNode::FocalLength, 20.f)
				.Done()
			.Done()
		.ExposeParameter(TEXT("OffsetParam"), TEXT("Offset"), GET_MEMBER_NAME_CHECKED(UOffsetCameraNode, TranslationOffset))
		.ExposeParameter(TEXT("FocalLengthParam"), TEXT("Lens"), GET_MEMBER_NAME_CHECKED(ULensParametersCameraNode, FocalLength))
		.Get();

	// Make a camera rig that uses the previous one, with overrides on both the offset (now 15, 25, 35)
	// and the focal length (now 25). Expose the offset further up as an interface parameter.
	UCameraRigCameraNode* MiddlePrefabNode = nullptr;

	UCameraRigAsset* MiddleCameraRig = FCameraRigAssetTestBuilder(Registry, TEXT("MiddleCameraRig"))
		.MakeRootNode<UCameraRigCameraNode>()
			.Pin(MiddlePrefabNode)
			.Setup([InnerCameraRig](UCameraRigCameraNode* Node, FNamedObjectRegistry* Registry)
					{
						Node->CameraRigReference.SetCameraRig(InnerCameraRig);

						FCameraRigParameterOverrides& ParameterOverrides = Node->CameraRigReference.GetParameterOverrides();

						auto* OffsetParam = Registry->Get<UCameraRigInterfaceParameter>(TEXT("OffsetParam"));
						auto& OffsetParamOverride = ParameterOverrides.FindOrAddParameterOverride<FVector3dCameraRigParameterOverride>(OffsetParam);
						OffsetParamOverride.Value = FVector3d(15, 25, 35);

						auto* FocalLengthParam = Registry->Get<UCameraRigInterfaceParameter>(TEXT("FocalLengthParam"));
						auto& FocalLengthParamOverride = ParameterOverrides.FindOrAddParameterOverride<FFloatCameraRigParameterOverride>(FocalLengthParam);
						FocalLengthParamOverride.Value = 25.f;
					})
			.Done()
			.ExposeParameter(TEXT("MiddleOffsetParam"), MiddlePrefabNode, TEXT("OffsetParam"))
		.Get();

	// Make another camera rig that uses the previous one, which makes a total of 3 nesting levels of camera rigs.
	// This level overrides the offset parameter some more (now 20, 50, 70).
	UCameraRigCameraNode* OuterPrefabNode = nullptr;

	UCameraRigAsset* OuterCameraRig = FCameraRigAssetTestBuilder(Registry, TEXT("OuterCameraRig"))
		.MakeRootNode<UCameraRigCameraNode>()
			.Pin(OuterPrefabNode)
			.Setup([MiddleCameraRig](UCameraRigCameraNode* Node, FNamedObjectRegistry* Registry)
					{
						Node->CameraRigReference.SetCameraRig(MiddleCameraRig);

						FCameraRigParameterOverrides& ParameterOverrides = Node->CameraRigReference.GetParameterOverrides();
						auto* MiddleOffsetParam = Registry->Get<UCameraRigInterfaceParameter>(TEXT("MiddleOffsetParam"));
						auto& MiddleOffsetParamOverride = ParameterOverrides.FindOrAddParameterOverride<FVector3dCameraRigParameterOverride>(MiddleOffsetParam);
						MiddleOffsetParamOverride.Value = FVector3d(20, 50, 70);
					})
			.Done()
		.Get();

	OuterCameraRig->BuildCameraRig();

	UCameraRigInterfaceParameter* OffsetParam = InnerCameraRig->Interface.InterfaceParameters[0];
	UCameraRigInterfaceParameter* FocalLengthParam = InnerCameraRig->Interface.InterfaceParameters[1];

	UTEST_EQUAL_EXPR(OffsetParam->PrivateVariable->GetName(), "Override_InnerCameraRig_OffsetParam");
	UTEST_EQUAL_EXPR(FocalLengthParam->PrivateVariable->GetName(), "Override_InnerCameraRig_FocalLengthParam");

	// Test that the inner nodes are driven by the interface parameters.
	{
		UOffsetCameraNode* OffsetNode = Registry->Get<UOffsetCameraNode>("Offset");
		UTEST_EQUAL_EXPR((UCameraVariableAsset*)OffsetNode->TranslationOffset.Variable.Get(), OffsetParam->PrivateVariable.Get());
		UTEST_EQUAL_EXPR(OffsetNode->TranslationOffset.Variable->DefaultValue, OffsetNode->TranslationOffset.Value);
		UTEST_EQUAL_EXPR(OffsetNode->TranslationOffset.Variable->DefaultValue, FVector3d(10, 20, 30));

		ULensParametersCameraNode* LensNode = Registry->Get<ULensParametersCameraNode>("Lens");
		UTEST_EQUAL_EXPR((UCameraVariableAsset*)LensNode->FocalLength.Variable.Get(), FocalLengthParam->PrivateVariable.Get());
		UTEST_EQUAL_EXPR(LensNode->FocalLength.Variable->DefaultValue, LensNode->FocalLength.Value);
		UTEST_EQUAL_EXPR(LensNode->FocalLength.Variable->DefaultValue, 20.f);
	}

	// Test that the middle prefab node is driving the inner interface parameters, and that one of those
	// overrides is in turn driven by the middle camera rig's interface parameter.
	{
		FCameraRigParameterOverrides& ParameterOverrides = MiddlePrefabNode->CameraRigReference.GetParameterOverrides();

		FVector3dCameraRigParameterOverride* OffsetParamOverride = 
			ParameterOverrides.FindParameterOverride<FVector3dCameraRigParameterOverride>(OffsetParam->Guid);
		UTEST_NOT_NULL("OffsetParamOverride", OffsetParamOverride);

		UTEST_EQUAL_EXPR(OffsetParamOverride->InterfaceParameterName, "OffsetParam");
		UTEST_EQUAL_EXPR(OffsetParamOverride->PrivateVariableGuid, OffsetParam->PrivateVariable->GetGuid());
		UTEST_EQUAL_EXPR(OffsetParamOverride->Value.Value, FVector3d(15, 25, 35));

		FFloatCameraRigParameterOverride* FocalLengthParamOverride =
			ParameterOverrides.FindParameterOverride<FFloatCameraRigParameterOverride>(FocalLengthParam->Guid);
		UTEST_NOT_NULL("FocalLengthParamOverride", FocalLengthParamOverride);

		UTEST_EQUAL_EXPR(FocalLengthParamOverride->InterfaceParameterName, "FocalLengthParam");
		UTEST_EQUAL_EXPR(FocalLengthParamOverride->PrivateVariableGuid, FocalLengthParam->PrivateVariable->GetGuid());
		UTEST_EQUAL_EXPR(FocalLengthParamOverride->Value.Value, 25.f);
	}

	UCameraRigInterfaceParameter* MiddleOffsetParam = MiddleCameraRig->Interface.InterfaceParameters[0];
	{
		FCameraRigParameterOverrides& ParameterOverrides = MiddlePrefabNode->CameraRigReference.GetParameterOverrides();

		FVector3dCameraRigParameterOverride* OffsetParamOverride = 
			ParameterOverrides.FindParameterOverride<FVector3dCameraRigParameterOverride>(OffsetParam->Guid);
		UTEST_EQUAL_EXPR((UCameraVariableAsset*)OffsetParamOverride->Value.Variable.Get(), MiddleOffsetParam->PrivateVariable.Get());
		UTEST_EQUAL_EXPR(OffsetParamOverride->Value.Variable->DefaultValue, FVector3d(15, 25, 35));
	}

	// Test that the outer prefab node is driving the middle interface parameters.
	{
		FCameraRigParameterOverrides& ParameterOverrides = OuterPrefabNode->CameraRigReference.GetParameterOverrides();

		FVector3dCameraRigParameterOverride* OffsetParamOverride =
			ParameterOverrides.FindParameterOverride<FVector3dCameraRigParameterOverride>(MiddleOffsetParam->Guid);
		UTEST_NOT_NULL("OffsetParamOverride", OffsetParamOverride);

		UTEST_EQUAL_EXPR(OffsetParamOverride->InterfaceParameterName, "MiddleOffsetParam");
		UTEST_EQUAL_EXPR(OffsetParamOverride->PrivateVariableGuid, MiddleOffsetParam->PrivateVariable->GetGuid());
		UTEST_EQUAL_EXPR(OffsetParamOverride->Value.Value, FVector3d(20, 50, 70));
	}

	return true;
}

#undef LOCTEXT_NAMESPACE

