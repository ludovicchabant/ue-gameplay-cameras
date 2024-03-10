// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/VariableTableDebugBlock.h"

#include "Core/CameraVariableTable.h"
#include "Debug/CameraDebugColors.h"
#include "Debug/CameraDebugRenderer.h"
#include "Debug/DebugTextRenderer.h"
#include "HAL/IConsoleManager.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

UE_DEFINE_CAMERA_DEBUG_BLOCK(FVariableTableDebugBlock)

FVariableTableDebugBlock::FVariableTableDebugBlock()
{
}

FVariableTableDebugBlock::FVariableTableDebugBlock(const FCameraVariableTable& InVariableTable)
{
	Initialize(InVariableTable);
}

void FVariableTableDebugBlock::Initialize(const FCameraVariableTable& InVariableTable)
{
	for (const TPair<uint32, FCameraVariableTable::FEntry>& Pair : InVariableTable.Entries)
	{
		const int32 EntryId = Pair.Key;
		const FCameraVariableTable::FEntry& Entry = Pair.Value;

		FString EntryName;
#if WITH_EDITORONLY_DATA
		EntryName = Entry.DebugName;
#endif

		FString EntryValueStr;
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
			case ECameraVariableType::ValueName:\
				if (EnumHasAnyFlags(Entry.Flags, FCameraVariableTable::EEntryFlags::Written))\
				{\
					const ValueType EntryValue = InVariableTable.GetValue<ValueType>(EntryId);\
					EntryValueStr = ToDebugString(EntryValue);\
				}\
				break;
		switch (Entry.Type)
		{
			UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
		}
#undef UE_CAMERA_VARIABLE_FOR_TYPE

		FEntryDebugInfo EntryDebugInfo{ EntryId, EntryName, EntryValueStr };
		EntryDebugInfo.bWritten = EnumHasAnyFlags(Entry.Flags, FCameraVariableTable::EEntryFlags::Written);
		EntryDebugInfo.bWrittenThisFrame = EnumHasAnyFlags(Entry.Flags, FCameraVariableTable::EEntryFlags::WrittenThisFrame);
		Entries.Add(EntryDebugInfo);
	}
}

void FVariableTableDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
#if WITH_EDITORONLY_DATA
	bool bShowVariableIds = false;
	if (!ShowVariableIdsCVarName.IsEmpty())
	{
		IConsoleVariable* ShowVariableIdsCVar = IConsoleManager::Get().FindConsoleVariable(*ShowVariableIdsCVarName, false);
		if (ensureMsgf(ShowVariableIdsCVar, TEXT("No such console variable: %s"), *ShowVariableIdsCVarName))
		{
			bShowVariableIds = ShowVariableIdsCVar->GetBool();
		}
	}
#endif

	const FCameraDebugColors& Colors = FCameraDebugColors::Get();

	for (const FEntryDebugInfo& Entry : Entries)
	{
#if WITH_EDITORONLY_DATA
		if (bShowVariableIds)
		{
			Renderer.AddText(TEXT("{cam_passive}[%d]{cam_default} "), Entry.Id);
		}
		if (!Entry.Name.IsEmpty())
		{
			Renderer.AddText(TEXT("%s : "), *Entry.Name);
		}
		else
		{
			Renderer.AddText(TEXT("<no name data> : "), Entry.Id);
		}
#else
		Renderer.AddText(TEXT("[%d] <no name data> : "), Entry.Id);
#endif

		if (Entry.bWritten)
		{
			Renderer.AddText(Entry.Value);
			if (Entry.bWrittenThisFrame)
			{
				Renderer.AddText(TEXT(" {cam_passive}[WrittenThisFrame]"));
			}
		}
		else
		{
			Renderer.AddText("{cam_warning}[Uninitialized]");
		}

		Renderer.NewLine();
		Renderer.SetTextColor(Colors.Default);
	}
}

void FVariableTableDebugBlock::OnSerialize(FArchive& Ar)
{
	Ar << Entries;
	Ar << ShowVariableIdsCVarName;
}

FArchive& operator<< (FArchive& Ar, FVariableTableDebugBlock::FEntryDebugInfo& EntryDebugInfo)
{
	Ar << EntryDebugInfo.Id;
	Ar << EntryDebugInfo.Name;
	Ar << EntryDebugInfo.Value;
	Ar << EntryDebugInfo.bWritten;
	Ar << EntryDebugInfo.bWrittenThisFrame;
	return Ar;
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

