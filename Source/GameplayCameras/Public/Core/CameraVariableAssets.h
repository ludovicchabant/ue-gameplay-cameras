// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraVariableTableFwd.h"
#include "CoreTypes.h"
#include "Engine/DataAsset.h"
#include "Math/MathFwd.h"

#include "CameraVariableAssets.generated.h"

/**
 * The base asset class for all camera variables.
 */
UCLASS(Abstract)
class GAMEPLAYCAMERAS_API UCameraVariableAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UCameraVariableAsset(const FObjectInitializer& ObjectInit);

	uint32 GetVariableId() const { return VariableId; }

	virtual ECameraVariableType GetVariableType() const PURE_VIRTUAL(UCameraVariableAsset::GetVariableType, return ECameraVariableType::Boolean;);
	virtual const uint8* GetDefaultValuePtr() const PURE_VIRTUAL(UCameraVariableAsset::GetDefaultValuePtr, return nullptr;);

public:

	// UObject interface
	virtual void Serialize(FArchive& Ar) override;

public:

	/** Whether this variable auto-resets to its default value every frame. */
	UPROPERTY(EditAnywhere, Category=Camera)
	bool bAutoReset = false;

private:

	UPROPERTY()
	uint32 VariableId = 0;

	UPROPERTY()
	bool bIsLocal = false;
};

/** Boolean camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UBooleanCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Boolean; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&bDefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	bool bDefaultValue = false;
};

/** Integer camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UInteger32CameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Integer32; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	int32 DefaultValue = 0;
};

/** Float camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UFloatCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Float; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	float DefaultValue = 0.f;
};

/** Double camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UDoubleCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Double; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	double DefaultValue = 0.0;
};

/** Vector2f camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UVector2fCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Vector2f; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FVector2f DefaultValue;
};

/** Vector2d camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UVector2dCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Vector2d; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FVector2D DefaultValue;
};

/** Vector3f camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UVector3fCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Vector3f; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FVector3f DefaultValue;
};

/** Vector3d camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UVector3dCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Vector3d; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FVector3d DefaultValue;
};

/** Vector4f camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UVector4fCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Vector4f; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FVector4f DefaultValue;
};

/** Vector4d camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UVector4dCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Vector4d; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FVector4d DefaultValue;
};

/** Rotator3f camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API URotator3fCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Rotator3f; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FRotator3f DefaultValue;
};

/** Rotator3d camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API URotator3dCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Rotator3d; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FRotator3d DefaultValue;
};

/** Transform3f camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UTransform3fCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Transform3f; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FTransform3f DefaultValue;
};

/** Transform3d camera variable. */
UCLASS()
class GAMEPLAYCAMERAS_API UTransform3dCameraVariable : public UCameraVariableAsset
{
	GENERATED_BODY()

public:

	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Transform3d; }
	virtual const uint8* GetDefaultValuePtr() const override { return reinterpret_cast<const uint8*>(&DefaultValue); }

public:

	/** The default value of this variable. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FTransform3d DefaultValue;
};

