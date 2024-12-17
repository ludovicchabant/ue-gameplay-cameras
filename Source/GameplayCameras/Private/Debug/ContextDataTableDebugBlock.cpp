// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/ContextDataTableDebugBlock.h"

#include "Core/CameraContextDataTable.h"
#include "Debug/CameraDebugColors.h"
#include "Debug/CameraDebugRenderer.h"
#include "Debug/DebugTextRenderer.h"
#include "HAL/IConsoleManager.h"
#include "UObject/PropertyPortFlags.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

UE_DEFINE_CAMERA_DEBUG_BLOCK(FContextDataTableDebugBlock)

FContextDataTableDebugBlock::FContextDataTableDebugBlock()
{
}

FContextDataTableDebugBlock::FContextDataTableDebugBlock(const FCameraContextDataTable& InContextDataTable)
{
	Initialize(InContextDataTable);
}

void FContextDataTableDebugBlock::Initialize(const FCameraContextDataTable& InContextDataTable)
{
	for (const FCameraContextDataTable::FEntry& Entry : InContextDataTable.Entries)
	{
		const uint8* EntryData = InContextDataTable.Memory + Entry.Offset;

		FName EntryTypeName;
		if (Entry.TypeObject)
		{
			EntryTypeName = Entry.TypeObject->GetFName();
		}

		FString EntryValueStr;
		switch (Entry.Type)
		{
			case ECameraContextDataType::Name:
				EntryValueStr = reinterpret_cast<const FName*>(EntryData)->ToString();
				break;
			case ECameraContextDataType::String:
				EntryValueStr = *reinterpret_cast<const FString*>(EntryData);
				break;
			case ECameraContextDataType::Enum:
				{
					const UEnum* EnumType = CastChecked<const UEnum>(Entry.TypeObject);
					EntryValueStr = EnumType->GetNameStringByValue((int64)*reinterpret_cast<const uint8*>(EntryData));
				}
				break;
			case ECameraContextDataType::Struct:
				{
					const UScriptStruct* StructType = CastChecked<const UScriptStruct>(Entry.TypeObject);
					const int32 ExportFlags = PPF_Delimited | PPF_IncludeTransient | PPF_ExternalEditor;
					StructType->ExportText(EntryValueStr, EntryData, nullptr, nullptr, ExportFlags, nullptr);
				}
				break;
			case ECameraContextDataType::Object:
				EntryValueStr = reinterpret_cast<const TObjectPtr<UObject>*>(EntryData)->GetPathName();
				break;
			case ECameraContextDataType::Class:
				EntryValueStr = reinterpret_cast<const TObjectPtr<UClass>*>(EntryData)->GetPathName();
				break;
			default:
				ensure(false);
				break;
		}

		FEntryDebugInfo EntryDebugInfo{ Entry.ID.DataName, EntryTypeName, EntryValueStr};
		EntryDebugInfo.bWritten = EnumHasAnyFlags(Entry.Flags, FCameraContextDataTable::EEntryFlags::Written);
		EntryDebugInfo.bWrittenThisFrame = EnumHasAnyFlags(Entry.Flags, FCameraContextDataTable::EEntryFlags::WrittenThisFrame);
		Entries.Add(EntryDebugInfo);
	}

	Entries.StableSort([](const FEntryDebugInfo& A, const FEntryDebugInfo& B) -> bool
			{
				return A.Name.Compare(B.Name) < 0;
			});
}

void FContextDataTableDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	const FCameraDebugColors& Colors = FCameraDebugColors::Get();

	for (const FEntryDebugInfo& Entry : Entries)
	{
		Renderer.AddText(TEXT("%s [%s] "), *Entry.Name.ToString(), *Entry.TypeName.ToString());
		if (!Entry.bWritten)
		{
			Renderer.AddText("{cam_warning}[Uninitialized]");
		}
		else if (Entry.bWrittenThisFrame)
		{
			Renderer.AddText(TEXT(" {cam_passive}[WrittenThisFrame]"));
		}
		Renderer.NewLine();

		Renderer.AddIndent();
		{
			Renderer.AddText(Entry.Value);
		}
		Renderer.RemoveIndent();
		Renderer.SetTextColor(Colors.Default);
	}
}

void FContextDataTableDebugBlock::OnSerialize(FArchive& Ar)
{
	Ar << Entries;
}

FArchive& operator<< (FArchive& Ar, FContextDataTableDebugBlock::FEntryDebugInfo& EntryDebugInfo)
{
	Ar << EntryDebugInfo.Name;
	Ar << EntryDebugInfo.TypeName;
	Ar << EntryDebugInfo.Value;
	Ar << EntryDebugInfo.bWritten;
	Ar << EntryDebugInfo.bWrittenThisFrame;
	return Ar;
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

