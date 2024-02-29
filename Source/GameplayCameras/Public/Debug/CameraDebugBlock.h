// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "CoreTypes.h"
#include "GameplayCameras.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

class FArchive;

namespace UE::Cameras
{

class FCameraDebugBlock;
class FCameraDebugRenderer;

/**
 * Parameter structure for debug block drawing.
 */
struct FCameraDebugBlockDrawParams
{
	float DeltaTime = 0.f;
};

/**
 * Metadata for a debug block's member field.
 * This is mostly used for auto-serialization of debugging info.
 */
struct FCameraDebugBlockField
{
	virtual ~FCameraDebugBlockField() {}
	virtual void SerializeField(FCameraDebugBlock* This, FArchive& Ar) = 0;

	FName FieldName;
	uint16 FieldIndex = 0;
	uint16 FieldOffset = 0;
};

/**
 * Templated version of the debug block field.
 */
template<typename FieldType>
struct TCameraDebugBlockField : FCameraDebugBlockField
{
	TCameraDebugBlockField(const FName& InName, uint16 InOffset) 
	{
		FieldName = InName;
		FieldOffset = InOffset;
	}

	virtual void SerializeField(FCameraDebugBlock* This, FArchive& Ar) override
	{
		FieldType* Value = reinterpret_cast<FieldType*>(reinterpret_cast<uint8*>(This) + FieldOffset);
		Ar << (*Value);
	}
};

enum class EDebugDrawResult
{
	Default = 0,
	SkipChildren = 1 << 0
};

ENUM_CLASS_FLAGS(EDebugDrawResult);

/**
 * Debug drawing structure responsible for displaying information about some aspect
 * of the camera system evaluation. For instance, there is generally one debug block
 * per camera node.
 */
class FCameraDebugBlock
{
public:

	virtual ~FCameraDebugBlock() {}

	/** Adds a child block to this block. */
	void AddChild(FCameraDebugBlock* InChild);

	/** Gets the children of this block. */
	TArrayView<FCameraDebugBlock*> GetChildren() { return Children; }

	/** Called to let this block display its information on screen. */
	void DebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer);

	/** Serializes this debug block into a buffer, for recording/replaying purposes. */
	void Serialize(FArchive& Ar);

protected:

	virtual EDebugDrawResult OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer);
	virtual void OnPostDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) {}

	virtual void OnSerialize(FArchive& Ar) {}

protected:

	template<typename FieldType>
	static TCameraDebugBlockField<FieldType> CreateField(const FName& FieldName, uint16 FieldOffset)
	{
		return TCameraDebugBlockField<FieldType>{ FieldName, FieldOffset };
	}

	static int32 RegisterField(FCameraDebugBlockField* InField)
	{
		StaticFields.Add(InField);
		return StaticFields.Num() - 1;
	}

private:

	static TArray<FCameraDebugBlockField*> StaticFields;

	using FChildrenArray = TArray<FCameraDebugBlock*>;
	FChildrenArray Children;
};

}  // namespace UE::Cameras

// Macros for defining debug blocks for a camera node evaluator.
//
#define UE_DEFINE_CAMERA_DEBUG_BLOCK_START(ClassName)\
	class ClassName : public ::UE::Cameras::FCameraDebugBlock\
	{\
	private:\
		using Super = ::UE::Cameras::FCameraDebugBlock;\
		using ThisClassName = ClassName;

#define UE_DEFINE_CAMERA_DEBUG_BLOCK_FIELD(FieldType, FieldName)\
	public:\
		FieldType FieldName;\
	private:\
		inline static PTRINT Get##FieldName##Offset() { return (PTRINT)(&(((ThisClassName*)0)->FieldName)); }\
		inline static TCameraDebugBlockField<FieldType>* Get##FieldName##Field() {\
			using FField = TCameraDebugBlockField<FieldType>;\
			static FField StaticField = CreateField<FieldType>(TEXT(#FieldName), Get##FieldName##Offset());\
			return &StaticField; }\
		inline static const int32 FieldName##FieldIndex = RegisterField(Get##FieldName##Field());

#define UE_DEFINE_CAMERA_DEBUG_BLOCK_END()\
		virtual EDebugDrawResult OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override;\
	};

#else

#define UE_DEFINE_CAMERA_DEBUG_BLOCK_START(ClassName)
#define UE_DEFINE_CAMERA_DEBUG_BLOCK_FIELD(FieldType, FieldName)
#define UE_DEFINE_CAMERA_DEBUG_BLOCK_END()

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

