// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debugger/SCameraDirectorTreeDebugPanel.h"

#include "Debugger/SDebugWidgetUtils.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "SCameraDirectorTreeDebugPanel"

namespace UE::Cameras
{

void SCameraDirectorTreeDebugPanel::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SDebugWidgetUtils::CreateConsoleVariableCheckBox(
						LOCTEXT("ShowUnchanged", "Show unchanged properties"),
						TEXT("GameplayCameras.Debug.ContextInitialResult.ShowUnchanged"))
			]
	];
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

