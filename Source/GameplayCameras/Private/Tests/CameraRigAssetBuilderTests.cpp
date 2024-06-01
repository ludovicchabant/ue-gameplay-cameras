// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAsset.h"
#include "Misc/AutomationTest.h"
#include "Nodes/Blends/SmoothBlendCameraNode.h"
#include "Nodes/Common/OffsetCameraNode.h"
#include "Nodes/Common/LensParametersCameraNode.h"
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
			EAutomationExpectedMessageFlags::Exact,
			1,
			false);
	CameraRig->BuildCameraRig();
	UTEST_EQUAL("Error status", CameraRig->BuildStatus, ECameraBuildStatus::WithErrors);

	return true;
}

namespace UE::Cameras
{
	//extern int32 GArrayCameraNodeEvaluatorSizeof;
	//extern int32 GArrayCameraNodeEvaluatorAlignof;
	//extern int32 GOffsetCameraNodeEvaluatorSizeof;
	//extern int32 GOffsetCameraNodeEvaluatorAlignof;
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

	//int32 TotalSizeof = GArrayCameraNodeEvaluatorSizeof;
	//TotalSizeof = Align(TotalSizeof, GOffsetCameraNodeEvaluatorAlignof) + GOffsetCameraNodeEvaluatorSizeof;
	//UTEST_EQUAL("Evaluator allocation info", CameraRig->AllocationInfo.EvaluatorInfo.TotalSizeof, TotalSizeof);

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

#undef LOCTEXT_NAMESPACE

