// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"

#include "CameraVariableFactories.generated.h"

class UCameraVariableAsset;

/**
 * Implements a factory for camera variable assets.
 */
UCLASS(Abstract, hidecategories=Object)
class UCameraVariableAssetFactory : public UFactory
{
	GENERATED_BODY()

public:

	UCameraVariableAssetFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual bool ShouldShowInNewMenu() const override;
};

UCLASS()
class UBooleanCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UBooleanCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UInteger32CameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UInteger32CameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UFloatCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UFloatCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UDoubleCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UDoubleCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UVector2fCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UVector2fCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UVector2dCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UVector2dCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UVector3fCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UVector3fCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UVector3dCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UVector3dCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UVector4fCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UVector4fCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UVector4dCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UVector4dCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class URotator3fCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	URotator3fCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class URotator3dCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	URotator3dCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UTransform3fCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UTransform3fCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UTransform3dCameraVariableFactory : public UCameraVariableAssetFactory
{
	GENERATED_BODY()

	UTransform3dCameraVariableFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

