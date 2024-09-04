// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraRigNodeGraphNode.h"

#include "Core/CameraRigAsset.h"
#include "Editors/CameraNodeGraphSchema.h"
#include "Nodes/Common/CameraRigCameraNode.h"

UCameraRigNodeGraphNode::UCameraRigNodeGraphNode(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
}

void UCameraRigNodeGraphNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Add extra input pins for the inner camera rig's exposed parameters.
	UCameraRigCameraNode* CameraRigNode = Cast<UCameraRigCameraNode>(GetObject());
	if (ensure(CameraRigNode) && CameraRigNode->CameraRigReference.IsValid())
	{
		FCameraRigInterface& CameraRigInterface = CameraRigNode->CameraRigReference.GetCameraRig()->Interface;
		for (UCameraRigInterfaceParameter* InterfaceParameter : CameraRigInterface.InterfaceParameters)
		{
			FEdGraphPinType PinType;
			PinType.PinCategory = UCameraNodeGraphSchema::PC_CameraParameter;
			CreatePin(EGPD_Input, PinType, *InterfaceParameter->InterfaceParameterName);
		}
	}
}

