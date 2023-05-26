// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Paper_LevelStyle.h"

class FPaper_LevelCommands : public TCommands<FPaper_LevelCommands>
{
public:

	FPaper_LevelCommands()
		: TCommands<FPaper_LevelCommands>(TEXT("Paper_Level"), NSLOCTEXT("Contexts", "Paper_Level", "Paper_Level Plugin"), NAME_None, FPaper_LevelStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};