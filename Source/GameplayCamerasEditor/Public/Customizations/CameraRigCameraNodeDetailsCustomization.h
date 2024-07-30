// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "Core/CameraParameters.h"

class UCameraRigInterfaceParameterWrapper;

namespace UE::Cameras
{

class FParameterOverrideDetailRows;

class FCameraRigCameraNodeDetailsCustomization : public IDetailCustomization
{
public:

	static TSharedRef<IDetailCustomization> MakeInstance();

public:

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	virtual void PendingDelete() override;

private:

	TSharedPtr<FParameterOverrideDetailRows> ParameterOverrideRows;
};

}  // namespace UE::Cameras

