// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Widgets/SCompoundWidget.h"

class FTabManager;

namespace UE::Cameras
{

class SCameraPoseStatsDebugPanel : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCameraPoseStatsDebugPanel) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);
};

}  // namespace UE::Cameras

