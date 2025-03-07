// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraRigCameraNodeGraphSchema.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraNodeHierarchy.h"
#include "Core/CameraRigAsset.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GameplayCamerasEditorSettings.h"
#include "Nodes/Common/ArrayCameraNode.h"
#include "Widgets/Notifications/SNotificationList.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigCameraNodeGraphSchema)

#define LOCTEXT_NAMESPACE "CameraRigCameraNodeGraphSchema"

UCameraRigCameraNodeGraphSchema::UCameraRigCameraNodeGraphSchema(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
}

FObjectTreeGraphConfig UCameraRigCameraNodeGraphSchema::BuildGraphConfig() const
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	FObjectTreeGraphConfig GraphConfig;

	BuildBaseGraphConfig(GraphConfig);

	GraphConfig.GraphName = UCameraRigAsset::NodeTreeGraphName;
	GraphConfig.ConnectableObjectClasses.Add(UCameraNode::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigAsset::StaticClass());
	GraphConfig.NonConnectableObjectClasses.Add(UBlendCameraNode::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigAsset::StaticClass())
		.OnlyAsRoot()
		.HasSelfPin(false)
		.NodeTitleUsesObjectName(true)
		.NodeTitleColor(Settings->CameraRigAssetTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UArrayCameraNode::StaticClass())
		.OnSetupNewObject(FOnSetupNewObject::CreateLambda([](UObject* NewObject)
				{
					// Add two new pins by default.
					UArrayCameraNode* ArrayNode = CastChecked<UArrayCameraNode>(NewObject);
					ArrayNode->Children.AddDefaulted();
					ArrayNode->Children.AddDefaulted();
				}));

	return GraphConfig;
}

void UCameraRigCameraNodeGraphSchema::CollectAllObjects(UObjectTreeGraph* InGraph, TSet<UObject*>& OutAllObjects) const
{
	using namespace UE::Cameras;

	Super::CollectAllObjects(InGraph, OutAllObjects);

	// See if we are missing objects from AllNodeTreeObjects... if so, add them and notify the user.
	UCameraRigAsset* CameraRig = Cast<UCameraRigAsset>(InGraph->GetRootObject());
	if (CameraRig)
	{
		FCameraNodeHierarchy Hierarchy(CameraRig);

		TSet<UObject*> AllNodeTreeObjects;
		((IObjectTreeGraphRootObject*)CameraRig)->GetConnectableObjects(UCameraRigAsset::NodeTreeGraphName, AllNodeTreeObjects);

		TSet<UObject*> MissingNodeTreeObjects;
		if (Hierarchy.FindMissingConnectableObjects(AllNodeTreeObjects, MissingNodeTreeObjects))
		{
			FNotificationInfo NotificationInfo(
					FText::Format(
						LOCTEXT("AllNodeTreeObjectsMismatch", 
							"Found {0} nodes missing from the internal list. Please re-save the asset."),
						MissingNodeTreeObjects.Num()));
			NotificationInfo.ExpireDuration = 4.0f;
			FSlateNotificationManager::Get().AddNotification(NotificationInfo);

			for (UObject* MissingObject : MissingNodeTreeObjects)
			{
				((IObjectTreeGraphRootObject*)CameraRig)->AddConnectableObject(UCameraRigAsset::NodeTreeGraphName, MissingObject);
				OutAllObjects.Add(MissingObject);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

