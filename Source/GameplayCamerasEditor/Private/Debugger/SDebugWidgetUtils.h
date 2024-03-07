// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "Templates/SharedPointerFwd.h"

class IConsoleVariable;
class SCheckBox;
class SEditableTextBox;

namespace UE::Cameras
{

class SDebugWidgetUtils
{
public:

	static TSharedRef<SCheckBox> CreateConsoleVariableCheckBox(const FText& Text, const FString& ConsoleVariableName);
	static TSharedRef<SEditableTextBox> CreateConsoleVariableTextBox(const FString& ConsoleVariableName);

private:

	static IConsoleVariable* GetConsoleVariable(const FString& ConsoleVariableName);

private:

	static TMap<FString, IConsoleVariable*> CachedConsoleVariables;
};

}  // namespace UE::Cameras

