// Bachelor of Software Engineering 
// Media Design School 
// Auckland 
// New Zealand 
// (c) Media Design School
// File Name : SlateDesigner.cpp 
// Description : SlateDesigner Implementation File
// Author : William Inman
// Mail : William.inman@mds.ac.nz

#include "SlateDesigner.h"

#include "EditorStyleSet.h"
#include "ModuleDescriptor.h"
#include "SlateDesignerStyle.h"
#include "SlateDesignerCommands.h"
#include "ToolMenus.h"
#include "PluginDescriptor.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "GameProjectGenerationModule.h"
#include "ProjectDescriptor.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IPluginManager.h"
#include "SlateDesigner/Menu/SPluginDesignerMenu.h"
#include "Styling/SlateStyleMacros.h"

struct FPluginTemplateDescription;
static const FName SlateDesignerTabName("Slate Designer");

#define LOCTEXT_NAMESPACE "FSlateDesignerModule"

void FSlateDesignerModule::StartupModule()
{
	// Init the style
	FSlateDesignerStyle::Initialize();
	// Init the textures
	FSlateDesignerStyle::ReloadTextures();
	// Register the commands class
	FSlateDesignerCommands::Register();
	// make a new command pool
	PluginCommands = MakeShareable(new FUICommandList);

	// Map 'pluginButtonClicked' function to the action / command of clicking the plugin button in toolbar
	PluginCommands->MapAction(
		FSlateDesignerCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSlateDesignerModule::PluginButtonClicked),
		FCanExecuteAction());

	// Register the startup callback 'RegisterMenus'
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSlateDesignerModule::RegisterMenus));

	// Register a new tab spawner with the ID 'SlateDesignerTabName'. Will call 'onSpawnPluginTab' when 'FGlobalTabmanager::Get()->TryInvokeTab' is invoked
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SlateDesignerTabName, FOnSpawnTab::CreateRaw(this, &FSlateDesignerModule::OnSpawnPluginTab))
	.SetDisplayName(LOCTEXT("SlateDesignerTitle", "Slate Designer"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FSlateDesignerModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FSlateDesignerStyle::Shutdown();
	FSlateDesignerCommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SlateDesignerTabName);
}

void FSlateDesignerModule::RegisterMenus()
{
	// Not really sure, I think it has to do with the non-local nature of slate instantiation
	FToolMenuOwnerScoped ownerScoped(this);

	// Add a menu entry with the command pool we specified before to 'Tools -> Programming -> Slate Designer'
	if (UToolMenu* menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools"))
	{
		FToolMenuSection& section = menu->FindOrAddSection("Programming");
		section.AddMenuEntryWithCommandList(FSlateDesignerCommands::Get().PluginAction, PluginCommands);
	}
}
void FSlateDesignerModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(SlateDesignerTabName);
}

TSharedRef<SDockTab> FSlateDesignerModule::OnSpawnPluginTab(const FSpawnTabArgs& _spawnTabArgs)
{
	// Create a new Nomad DockTab containing our SPluginDesignerMenu
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SPluginDesignerMenu)
		];
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSlateDesignerModule, SlateDesigner)