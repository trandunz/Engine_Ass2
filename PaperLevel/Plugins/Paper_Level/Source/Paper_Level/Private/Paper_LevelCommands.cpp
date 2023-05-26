// Copyright Epic Games, Inc. All Rights Reserved.

#include "Paper_LevelCommands.h"

#define LOCTEXT_NAMESPACE "FPaper_LevelModule"

void FPaper_LevelCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Paper_Level", "Start Paper Level", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
