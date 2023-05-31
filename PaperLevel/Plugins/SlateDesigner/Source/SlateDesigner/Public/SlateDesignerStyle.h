// Bachelor of Software Engineering 
// Media Design School 
// Auckland 
// New Zealand 
// (c) Media Design School
// File Name : SlateDesignerStyle.h 
// Description : SlateDesignerStyle Header File
// Author : William Inman
// Mail : William.inman@mds.ac.nz

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FSlateDesignerStyle
{
public:

	static void Initialize();

	static void Shutdown();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();

	/** @return The Slate style set for the Shooter game */
	static const ISlateStyle& Get();

	static FName GetStyleSetName();

private:

	static TSharedRef< class FSlateStyleSet > Create();

private:

	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};