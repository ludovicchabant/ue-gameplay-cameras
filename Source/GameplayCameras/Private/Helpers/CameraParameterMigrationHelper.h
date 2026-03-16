// Copyright Epic Games, Inc. All Rights Reserved.

class UBaseCameraObject;
struct FInstancedOverridablePropertyBag;
struct FInstancedPropertyBag;

namespace UE::Cameras
{

/**
 * Helper class for helping migrate camera rig parameter bags without losing override values when a parameter is
 * changed from single-precision to double-precision.
 */
struct FCameraParameterMigrationHelper
{
	static void MigrateToNewBagInstanceWithOverrides(FInstancedOverridablePropertyBag& Parameters, const FInstancedPropertyBag& NewBagInstance);
};

}  // namespace UE::Cameras::Internal

