// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraNodeGraphPinColors.h"

#include "Core/CameraContextDataTableFwd.h"
#include "Core/CameraVariableTableFwd.h"
#include "Editors/CameraObjectGraphSchemaBase.h"
#include "GraphEditorSettings.h"

namespace UE::Cameras
{

void FCameraNodeGraphPinColors::Initialize()
{
	const UGraphEditorSettings* Settings = GetDefault<UGraphEditorSettings>();

	DefaultPinColor = Settings->DefaultPinTypeColor;

	VariablePinColors.Reset();
	DataPinColors.Reset();

	const UEnum* VariableTypeEnum = StaticEnum<ECameraVariableType>();
	VariablePinColors.Add(UCameraObjectGraphSchemaBase::PSC_Boolean, Settings->BooleanPinTypeColor);
	VariablePinColors.Add(UCameraObjectGraphSchemaBase::PSC_Integer, Settings->IntPinTypeColor);
	VariablePinColors.Add(UCameraObjectGraphSchemaBase::PSC_Real, Settings->RealPinTypeColor);
	VariablePinColors.Add(UCameraObjectGraphSchemaBase::PSC_Vector2, Settings->StructPinTypeColor);
	VariablePinColors.Add(UCameraObjectGraphSchemaBase::PSC_Vector3, Settings->VectorPinTypeColor);
	VariablePinColors.Add(UCameraObjectGraphSchemaBase::PSC_Vector4, Settings->StructPinTypeColor);
	VariablePinColors.Add(UCameraObjectGraphSchemaBase::PSC_Rotator, Settings->RotatorPinTypeColor);
	VariablePinColors.Add(UCameraObjectGraphSchemaBase::PSC_Transform, Settings->TransformPinTypeColor);
	VariablePinColors.Add(UCameraObjectGraphSchemaBase::PSC_BlendableStruct, Settings->StructPinTypeColor);

	const UEnum* DataTypeEnum = StaticEnum<ECameraContextDataType>();
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::Name), Settings->NamePinTypeColor);
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::String), Settings->StringPinTypeColor);
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::Enum), Settings->Int64PinTypeColor);
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::Struct), Settings->StructPinTypeColor);
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::Object), Settings->ObjectPinTypeColor);
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::Class), Settings->ClassPinTypeColor);
}

FLinearColor FCameraNodeGraphPinColors::GetVariablePinColor(const FName& VariableTypeName) const
{
	return VariablePinColors.FindRef(VariableTypeName, DefaultPinColor);
}

FLinearColor FCameraNodeGraphPinColors::GetContextDataPinColor(const FName& DataTypeName) const
{
	return DataPinColors.FindRef(DataTypeName, DefaultPinColor);
}

}  // namespace UE::Cameras

