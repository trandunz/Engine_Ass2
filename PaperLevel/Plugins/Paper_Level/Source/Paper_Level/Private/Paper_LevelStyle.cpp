// Copyright Epic Games, Inc. All Rights Reserved.

#include "Paper_LevelStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FPaper_LevelStyle::StyleInstance = nullptr;

void FPaper_LevelStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FPaper_LevelStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FPaper_LevelStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("Paper_LevelStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FPaper_LevelStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("Paper_LevelStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("Paper_Level")->GetBaseDir() / TEXT("Resources"));
	Style->Set("Paper_Level.PluginAction", new IMAGE_BRUSH(TEXT("Icon128"), Icon20x20));
	
	return Style;
}

void FPaper_LevelStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FPaper_LevelStyle::Get()
{
	return *StyleInstance;
}
