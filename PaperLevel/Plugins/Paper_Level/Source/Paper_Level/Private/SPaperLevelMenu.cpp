// Fill out your copyright notice in the Description page of Project Settings.


#include "SPaperLevelMenu.h"
#include "SlateDesigner/Public/Statics.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPaperLevelMenu::Construct(const FArguments& InArgs)
{
	// create a new vertical box to hold everything
	TSharedRef<SVerticalBox> newParent = SNew(SVerticalBox);
	// create a vertical box for storing the names of all the menus
	TSharedRef<SVerticalBox> newCurrentlyUsedMenus = SNew(SVerticalBox);
	// Create a scroll box to hold the newParent vertical box
	TSharedRef<SScrollBox> scrollBox = SNew(SScrollBox);

	ParentBox = &newParent.Get();
	
	InitSlateDesignerMenu();
	
	// add the parent vertical box too the scroll box
	scrollBox->AddSlot()
	.AttachWidget(newParent);
	
	// Assign Finished Parent (the scroll box)
	ChildSlot
	[
		scrollBox
	];
}

void SPaperLevelMenu::InitSlateDesignerMenu()
{
	auto newMenu = SNew(SBorder)
		.Padding(20)
		.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(FLinearColor(0.0f,0.0f,0.0f))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(Statics::GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				Statics::CreateNewTitle("Paper level")
			]
		];

	// Add menu to the parent
	ParentBox->AddSlot()
	.AutoHeight()
	[
		newMenu
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
