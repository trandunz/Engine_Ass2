// Bachelor of Software Engineering 
// Media Design School 
// Auckland 
// New Zealand 
// (c) Media Design School
// File Name : SlateDesignerCommands.h 
// Description : SlateDesignerCommands Header File
// Author : William Inman
// Mail : William.inman@mds.ac.nz

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SlateDesignerStyle.h"

class FSlateDesignerCommands : public TCommands<FSlateDesignerCommands>
{
public:

	FSlateDesignerCommands()
		: TCommands<FSlateDesignerCommands>(TEXT("SlateDesigner"), NSLOCTEXT("Contexts", "SlateDesigner", "SlateDesigner Plugin"), NAME_None, FSlateDesignerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};
