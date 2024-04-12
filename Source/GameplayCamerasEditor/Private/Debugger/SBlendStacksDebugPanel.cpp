// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debugger/SBlendStacksDebugPanel.h"

#include "Debugger/SDebugWidgetUtils.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "SBlendStacksDebugPanel"

namespace UE::Cameras
{

void SBlendStacksDebugPanel::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SDebugWidgetUtils::CreateConsoleVariableCheckBox(
						LOCTEXT("ShowUnchanged", "Show unchanged properties"),
						TEXT("GameplayCameras.Debug.BlendStack.ShowUnchanged"))
			]
		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SDebugWidgetUtils::CreateConsoleVariableCheckBox(
						LOCTEXT("ShowVariableIDs", "Show variable IDs"),
						TEXT("GameplayCameras.Debug.BlendStack.ShowVariableIDs"))
			]
	];
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

