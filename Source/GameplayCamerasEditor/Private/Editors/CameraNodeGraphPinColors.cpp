// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraNodeGraphPinColors.h"

#include "Core/CameraContextDataTableFwd.h"
#include "Core/CameraParameters.h"
#include "Core/CameraVariableReferences.h"
#include "Core/CameraVariableTableFwd.h"
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
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Boolean), Settings->BooleanPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Integer32), Settings->IntPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Float), Settings->FloatPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Double), Settings->DoublePinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Vector2f), Settings->VectorPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Vector2d), Settings->VectorPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Vector3f), Settings->VectorPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Vector3d), Settings->VectorPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Vector4f), Settings->VectorPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Vector4d), Settings->VectorPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Rotator3f), Settings->RotatorPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Rotator3d), Settings->RotatorPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Transform3f), Settings->TransformPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::Transform3d), Settings->TransformPinTypeColor);
	VariablePinColors.Add(VariableTypeEnum->GetNameByValue((int64)ECameraVariableType::BlendableStruct), Settings->StructPinTypeColor);

	const UEnum* DataTypeEnum = StaticEnum<ECameraContextDataType>();
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::Name), Settings->NamePinTypeColor);
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::String), Settings->StringPinTypeColor);
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::Enum), Settings->Int64PinTypeColor);
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::Struct), Settings->StructPinTypeColor);
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::Object), Settings->ObjectPinTypeColor);
	DataPinColors.Add(DataTypeEnum->GetNameByValue((int64)ECameraContextDataType::Class), Settings->ClassPinTypeColor);
}

FLinearColor FCameraNodeGraphPinColors::GetPinColor(const FName& VariableTypeName) const
{
	return VariablePinColors.FindRef(VariableTypeName, DefaultPinColor);
}

FLinearColor FCameraNodeGraphPinColors::GetContextDataPinColor(const FName& DataTypeName) const
{
	return DataPinColors.FindRef(DataTypeName, DefaultPinColor);
}

}  // namespace UE::Cameras

