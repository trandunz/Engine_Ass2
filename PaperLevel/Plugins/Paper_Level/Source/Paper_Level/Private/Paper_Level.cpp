// Copyright Epic Games, Inc. All Rights Reserved.

#include "Paper_Level.h"
#include "Paper_LevelStyle.h"
#include "Paper_LevelCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName Paper_LevelTabName("Paper_Level");

#define LOCTEXT_NAMESPACE "FPaper_LevelModule"

void FPaper_LevelModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FPaper_LevelStyle::Initialize();
	FPaper_LevelStyle::ReloadTextures();

	FPaper_LevelCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FPaper_LevelCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FPaper_LevelModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPaper_LevelModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(Paper_LevelTabName, FOnSpawnTab::CreateRaw(this, &FPaper_LevelModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FPaper_LevelTabTitle", "Paper_Level"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FPaper_LevelModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FPaper_LevelStyle::Shutdown();

	FPaper_LevelCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(Paper_LevelTabName);
}

TSharedRef<SDockTab> FPaper_LevelModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Paper Level"))
			]
		];
}

void FPaper_LevelModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(Paper_LevelTabName);
}

void FPaper_LevelModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FPaper_LevelCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FPaper_LevelCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPaper_LevelModule, Paper_Level)