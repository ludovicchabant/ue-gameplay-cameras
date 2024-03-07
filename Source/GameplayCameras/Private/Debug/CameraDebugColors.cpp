// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/CameraDebugColors.h"

#include "Misc/Optional.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

TMap<FString, FColor> FCameraDebugColors::ColorMap;

const FCameraDebugColors& FCameraDebugColors::Get()
{
	static FCameraDebugColors StaticInstance;
	return StaticInstance;
}

void FCameraDebugColors::UpdateColorMap(const FCameraDebugColors& Instance)
{
	ColorMap.Reset();
	ColorMap.Add(TEXT("cam_title"), Instance.Title);
	ColorMap.Add(TEXT("cam_default"), Instance.Default);
	ColorMap.Add(TEXT("cam_passive"), Instance.Passive);
	ColorMap.Add(TEXT("cam_verypassive"), Instance.VeryPassive);
	ColorMap.Add(TEXT("cam_highlighted"), Instance.Hightlighted);
	ColorMap.Add(TEXT("cam_notice"), Instance.Notice);
	ColorMap.Add(TEXT("cam_notice2"), Instance.Notice2);
	ColorMap.Add(TEXT("cam_good"), Instance.Good);
	ColorMap.Add(TEXT("cam_warning"), Instance.Warning);
	ColorMap.Add(TEXT("cam_error"), Instance.Error);
	ColorMap.Add(TEXT("cam_background"), Instance.Background);
}

TOptional<FColor> FCameraDebugColors::GetFColorByName(const FString& InColorName)
{
	if (ColorMap.IsEmpty())
	{
		UpdateColorMap(FCameraDebugColors::Get());
	}
	if (const FColor* Color = ColorMap.Find(InColorName))
	{
		return TOptional<FColor>(*Color);
	}
	return TOptional<FColor>();
}

FCameraDebugColors::FCameraDebugColors()
{
	// Default to colors inspired by the Solarized palette.
	// Other palettes could be implemented.
	//
	//    SOLARIZED HEX     RGB        
    //    --------- ------- -----------
    //    base03    #002b36   0  43  54
    //    base02    #073642   7  54  66
    //    base01    #586e75  88 110 117
    //    base00    #657b83 101 123 131
    //    base0     #839496 131 148 150
    //    base1     #93a1a1 147 161 161
    //    base2     #eee8d5 238 232 213
    //    base3     #fdf6e3 253 246 227
    //    yellow    #b58900 181 137   0
    //    orange    #cb4b16 203  75  22
    //    red       #dc322f 220  50  47
    //    magenta   #d33682 211  54 130
    //    violet    #6c71c4 108 113 196
    //    blue      #268bd2  38 139 210
    //    cyan      #2aa198  42 161 152
    //    green     #859900 133 153   0
	//
	Title = FColor(38, 139, 210); // blue
	Default = FColor(238, 232, 213); // base2
	Passive = FColor(147, 161, 161); // base1
	VeryPassive = FColor(101, 123, 131); // base00
	Hightlighted = FColor(253, 246, 227); // base3
	Notice = FColor(42, 161, 152); // cyan
	Notice2 = FColor(211, 54, 130); // magenta
	Good = FColor(133, 153, 0); // green
	Warning = FColor(181, 137, 0); // yellow
	Error = FColor(220, 50, 47); // red
	Background = FColor(7, 54, 66); // base02
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

