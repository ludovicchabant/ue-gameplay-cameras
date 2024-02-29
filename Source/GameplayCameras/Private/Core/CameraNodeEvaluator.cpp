// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraNodeEvaluator.h"

#include "Core/CameraNode.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "HAL/IConsoleManager.h"
#include "UObject/UObjectGlobals.h"

namespace UE::Cameras
{

UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(FCameraNodeEvaluator)

#if UE_GAMEPLAY_CAMERAS_DEBUG

FString GGameplayCamerasDebugNodeTreeFilter;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugNodeTreeFilter(
	TEXT("GameplayCameras.Debug.NodeTree.Filter"),
	GGameplayCamerasDebugNodeTreeFilter,
	TEXT("(Default: "". Filters the debug camera node tree by node name/type."));

class FDefaultCameraNodeDebugBlock : public FCameraDebugBlock
{
public:

	FDefaultCameraNodeDebugBlock() {}
	FDefaultCameraNodeDebugBlock(const UCameraNode* InNode)
	{
		NodeClassName = InNode ? InNode->GetClass()->GetName() : TEXT("<null node>");
	}

protected:

	virtual EDebugDrawResult OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override
	{
		Renderer.AddIndent();
		
		const bool bDoDebugDraw = (GGameplayCamerasDebugNodeTreeFilter.IsEmpty() ||
				NodeClassName.Contains(GGameplayCamerasDebugNodeTreeFilter));
		if (bDoDebugDraw)
		{
			Renderer.AddText(TEXT("[%s] "), *NodeClassName);
		}
		
		return bDoDebugDraw ?  EDebugDrawResult::Default : EDebugDrawResult::SkipChildren;
	}

	virtual void OnPostDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override
	{
		Renderer.RemoveIndent();
	}

	virtual void OnSerialize(FArchive& Ar)
	{
		Ar << NodeClassName;
	}

private:

	FString NodeClassName;
};

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

void FCameraNodeEvaluationResult::Reset()
{
	CameraPose.Reset();
	bIsCameraCut = false;
	bIsValid = false;
}

FCameraNodeEvaluator* FCameraNodeEvaluatorBuildParams::BuildEvaluator(const UCameraNode* InNode) const
{
	FCameraNodeEvaluator* NewEvaluator = InNode->BuildEvaluator(Builder);
	NewEvaluator->Build(*this);
	return NewEvaluator;
}

FCameraNodeEvaluator::FCameraNodeEvaluator()
{
}

void FCameraNodeEvaluator::SetPrivateCameraNode(TObjectPtr<const UCameraNode> InCameraNode)
{
	PrivateCameraNode = InCameraNode;
}

FCameraNodeEvaluatorChildrenView FCameraNodeEvaluator::GetChildren()
{
	return OnGetChildren();
}

void FCameraNodeEvaluator::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (PrivateCameraNode)
	{
		Collector.AddReferencedObject(PrivateCameraNode);
	}

	OnAddReferencedObjects(Collector);

	for (FCameraNodeEvaluator* Child : GetChildren())
	{
		if (Child)
		{
			Child->AddReferencedObjects(Collector);
		}
	}
}

void FCameraNodeEvaluator::Build(const FCameraNodeEvaluatorBuildParams& Params)
{
	OnBuild(Params);
}

void FCameraNodeEvaluator::Initialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	OnInitialize(Params);

	for (FCameraNodeEvaluator* Child : GetChildren())
	{
		if (Child)
		{
			Child->Initialize(Params);
		}
	}
}

void FCameraNodeEvaluator::Run(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	if (!PrivateCameraNode || PrivateCameraNode->bIsEnabled)
	{
#if UE_GAMEPLAY_CAMERAS_DEBUG
		if (OutResult.DebugBlockBuilder)
		{
			CreateDebugBlock(Params, *OutResult.DebugBlockBuilder);
		}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

		OnRun(Params, OutResult);

#if UE_GAMEPLAY_CAMERAS_DEBUG
		if (OutResult.DebugBlockBuilder)
		{
			OutResult.DebugBlockBuilder->EndBlock();
		}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
	}
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FCameraNodeEvaluator::CreateDebugBlock(const FCameraNodeEvaluationParams& Params, FCameraDebugBlockBuilder& Builder)
{
	OnCreateDebugBlock(Params, Builder);
}

void FCameraNodeEvaluator::OnCreateDebugBlock(const FCameraNodeEvaluationParams& Params, FCameraDebugBlockBuilder& Builder)
{
	Builder.StartBlock<FDefaultCameraNodeDebugBlock>(PrivateCameraNode);
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

