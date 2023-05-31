// Bachelor of Software Engineering 
// Media Design School 
// Auckland 
// New Zealand 
// (c) Media Design School
// File Name : SlateDesigner.h 
// Description : SlateDesigner Header File
// Author : William Inman
// Mail : William.inman@mds.ac.nz

#pragma once
#include "CoreMinimal.h"

class FSlateDesignerModule : public IModuleInterface
{
public:
	/**
	 * @brief Called when the plugin starts up
	 */
	virtual void StartupModule() override;
	/**
	 * @brief  Called when the plugin shuts down
	 */
	virtual void ShutdownModule() override;

private:
	/**
	 * @brief Startup callback to add the toolbar menu entry for the button
	 */
	void RegisterMenus();
	/**
	 * @brief Opens a new window. Callback for when the plugin button is clicked. 
	 */
	void PluginButtonClicked();

	/**
	 * @brief Creates a new SPluginDesigner menu and adds it too the window .Called when the plugin command 'TryEnvokeTab' is called.
	 * @param SpawnTabArgs 
	 * @return 
	 */
	TSharedRef<class SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& _spawnTabArgs);
	
	TSharedPtr<class FUICommandList> PluginCommands;
};
