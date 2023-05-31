// Bachelor of Software Engineering 
// Media Design School 
// Auckland 
// New Zealand 
// (c) Media Design School
// File Name : SlateDesignerCommands.cpp 
// Description : SlateDesignerCommands Implementation File
// Author : William Inman
// Mail : William.inman@mds.ac.nz

#include "SlateDesignerCommands.h"

#define LOCTEXT_NAMESPACE "FSlateDesignerModule"

void FSlateDesignerCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "SlateDesigner", "Execute SlateDesigner action", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(OpenPluginWindow, "SlateDesigner", "Bring up SlateDesigner window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
