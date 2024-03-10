// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debugger/SDebugWidgetUtils.h"

#include "HAL/IConsoleManager.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"

namespace UE::Cameras
{

TMap<FString, IConsoleVariable*> SDebugWidgetUtils::CachedConsoleVariables;

TSharedRef<SCheckBox> SDebugWidgetUtils::CreateConsoleVariableCheckBox(const FText& Text, const FString& ConsoleVariableName)
{
	return SNew(SCheckBox)
		.Padding(4.f)
		.IsChecked_Lambda([ConsoleVariableName]() 
				{
					if (IConsoleVariable* ConsoleVariable = SDebugWidgetUtils::GetConsoleVariable(ConsoleVariableName))
					{
						return ConsoleVariable->GetBool() ?
							ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					}
					return ECheckBoxState::Undetermined;
				})
		.OnCheckStateChanged_Lambda([ConsoleVariableName](ECheckBoxState NewState)
				{
					if (IConsoleVariable* ConsoleVariable = SDebugWidgetUtils::GetConsoleVariable(ConsoleVariableName))
					{
						ConsoleVariable->Set(NewState == ECheckBoxState::Checked);
					}
				})
		[
			SNew(STextBlock)
				.Text(Text)
		];
}

TSharedRef<SEditableTextBox> SDebugWidgetUtils::CreateConsoleVariableTextBox(const FString& ConsoleVariableName)
{
	return SNew(SEditableTextBox)
		.Padding(4.f)
		.Text_Lambda([ConsoleVariableName]()
				{
					if (IConsoleVariable* ConsoleVariable = SDebugWidgetUtils::GetConsoleVariable(ConsoleVariableName))
					{
						return FText::FromString(ConsoleVariable->GetString());
					}
					return FText();
				})
		.OnTextChanged_Lambda([ConsoleVariableName](const FText& NewText)
				{
					if (IConsoleVariable* ConsoleVariable = SDebugWidgetUtils::GetConsoleVariable(ConsoleVariableName))
					{
						ConsoleVariable->Set(*NewText.ToString());
					}
				});
}

IConsoleVariable* SDebugWidgetUtils::GetConsoleVariable(const FString& ConsoleVariableName)
{
	if (IConsoleVariable** ConsoleVariablePtr = CachedConsoleVariables.Find(ConsoleVariableName))
	{
		return (*ConsoleVariablePtr);
	}

	IConsoleVariable* ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*ConsoleVariableName);
	if (ensureMsgf(ConsoleVariable, TEXT("No such console variable: %s"), *ConsoleVariableName))
	{
		CachedConsoleVariables.Add(ConsoleVariableName, ConsoleVariable);
		return ConsoleVariable;
	}

	return nullptr;
}

}  // namespace UE::Cameras

