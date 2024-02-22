// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AssetDefinitionDefault.h"

#include "AssetDefinition_CameraVariableAssets.generated.h"

UCLASS(Abstract, MinimalAPI)
class UAssetDefinition_CameraVariableAsset : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:

	// UAssetDefinition interface
	virtual FLinearColor GetAssetColor() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
};

UCLASS()
class UAssetDefinition_BooleanCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Integer32CameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_FloatCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_DoubleCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Vector2fCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Vector2dCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Vector3fCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Vector3dCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Vector4fCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Vector4dCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Rotator3fCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Rotator3dCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Transform3fCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

UCLASS()
class UAssetDefinition_Transform3dCameraVariable : public UAssetDefinition_CameraVariableAsset
{
	GENERATED_BODY()

	// UAssetDefinition interface
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};

