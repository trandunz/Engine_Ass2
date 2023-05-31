// Bachelor of Software Engineering 
// Media Design School 
// Auckland 
// New Zealand 
// (c) Media Design School
// File Name : SlateDesignerStyle.cpp 
// Description : SlateDesignerStyle Implementation File
// Author : William Inman
// Mail : William.inman@mds.ac.nz

#include "SlateDesignerStyle.h"
#include "SlateDesigner.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FSlateDesignerStyle::StyleInstance = nullptr;

void FSlateDesignerStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FSlateDesignerStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FSlateDesignerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SlateDesignerStyle"));
	return StyleSetName;
}
 
const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FSlateDesignerStyle::Create()
{
	// make a new style with the root search directory for images being the plugin/Resources
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("SlateDesignerStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("SlateDesigner")->GetBaseDir() / TEXT("Resources"));

	// Set the icon of the action to be SlateDesigner.png (https://ih1.redbubble.net/image.4793924682.5263/st,small,507x507-pad,600x600,f8f8f8.jpg)
	Style->Set("SlateDesigner.PluginAction", new IMAGE_BRUSH(TEXT("SlateDesigner"), Icon20x20));
	return Style;
}

void FSlateDesignerStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FSlateDesignerStyle::Get()
{
	return *StyleInstance;
}
