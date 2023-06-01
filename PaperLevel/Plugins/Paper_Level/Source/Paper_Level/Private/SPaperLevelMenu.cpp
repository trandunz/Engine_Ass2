// Fill out your copyright notice in the Description page of Project Settings.


#include "SPaperLevelMenu.h"
#include "SlateDesigner/Public/Statics.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPaperLevelMenu::Construct(const FArguments& InArgs)
{
	SetCanTick(true);
	
	// create a new vertical box to hold everything
	TSharedRef<SVerticalBox> newParent = SNew(SVerticalBox);
	// create a vertical box for storing the names of all the menus
	TSharedRef<SVerticalBox> newCurrentlyUsedMenus = SNew(SVerticalBox);
	// Create a scroll box to hold the newParent vertical box
	TSharedRef<SScrollBox> scrollBox = SNew(SScrollBox);

	// assign parent box
	ParentBox = &newParent.Get();
	
	InitPaperLevelMenu();
	
	// add the parent vertical box too the scroll box
	scrollBox->AddSlot()
	.AttachWidget(newParent);
	
	// Assign Finished Parent (the scroll box)
	ChildSlot
	[
		scrollBox
	];
}

void SPaperLevelMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	auto mousePos = FSlateApplication::Get().GetCursorPos();
	UE_LOG(LogTemp, Warning, TEXT("Mouse X: %s | Mouse Y: %s"), *FString::FromInt(mousePos.X), *FString::FromInt(mousePos.Y) );
}

void SPaperLevelMenu::InitPaperLevelMenu()
{
	auto newMenu = SNew(SBorder)
		.Padding(20)
		.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(FLinearColor(0.0f,0.0f,0.0f))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			InitPaperLevelMenuContents()
		];

	// Add menu to the parent
	ParentBox->AddSlot()
	.AutoHeight()
	[
		newMenu
	];
}

TSharedRef<SBorder> SPaperLevelMenu::InitPaperLevelMenuContents()
{
	// Grand Vert Box
	auto outerVertBox = SNew(SVerticalBox);

	// Create VertBox for Grid
	auto vertBox = SNew(SVerticalBox);

	// Create gridAndControlsSplitter for Grid and controls
	auto gridAndControlsSplitter = SNew(SHorizontalBox);
	gridAndControlsSplitter->AddSlot()
	.AutoWidth()
	[
		vertBox
	];

	// Create all the buttons
	CreatePaperMapGrid();
	
	for(unsigned y = 0; y < 10; y++ )
	{
		auto horizonBox = SNew(SHorizontalBox);
		for(unsigned x = 0; x < 10; x++ )
		{
			horizonBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoWidth()
			[
				WorldGrid[{x,y}]
			];
		}
		
		vertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			horizonBox
		];
	}

	// Create VertBox for Controls
	auto controlsVertBox = SNew(SVerticalBox);
	
	{
		// Create all the buttons
		CreateSymbolsGrid();

		//auto objectControlsBorder = SNew(SBorder);
	
		// Add Symbol Constols Title
		controlsVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::CreateNewTitle("Objects")
		];
	
		for(unsigned y = 0; y < 6; y++ )
		{
			auto horizonBox = SNew(SHorizontalBox);
			for(unsigned x = 0; x < 6; x++ )
			{
				horizonBox->AddSlot()
				.HAlign(HAlign_Center)
				.AutoWidth()
				[
					SymbolsGrid[{x,y}]
				];
			}
		
			controlsVertBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				horizonBox
			];
		}

		{
			auto horizonBox = SNew(SHorizontalBox);
			horizonBox->AddSlot()
			.HAlign(HAlign_Center)
			.FillWidth(1)
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "Eraser", &SPaperLevelMenu::OnEraserClicked)
			];
			horizonBox->AddSlot()
			.HAlign(HAlign_Center)
			.FillWidth(1)
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "Undo", &SPaperLevelMenu::OnUndoClicked)
			];
			horizonBox->AddSlot()
			.HAlign(HAlign_Center)
			.FillWidth(1)
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "Redo", &SPaperLevelMenu::OnRedoClicked)
			];

			controlsVertBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				horizonBox
			];
			controlsVertBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "New Symbol", &SPaperLevelMenu::OnRedoClicked)
			];
		}
	}
	
	controlsVertBox->AddSlot()
	.HAlign(HAlign_Center)
	.AutoHeight()
	[
		Statics::CreateNewTitle("World Boundary")
	];

	{
		auto horizonBox = SNew(SHorizontalBox);
		horizonBox->AddSlot()
		.HAlign(HAlign_Center)
		.FillWidth(1)
		[
			Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnEraserClicked)
		];
		horizonBox->AddSlot()
		.HAlign(HAlign_Center)
		.FillWidth(1)
		[
			Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnUndoClicked)
		];
		horizonBox->AddSlot()
		.HAlign(HAlign_Center)
		.FillWidth(1)
		[
			Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnRedoClicked)
		];

		controlsVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			horizonBox
		];
		controlsVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::CreateButton<SPaperLevelMenu>(this, "Apply", &SPaperLevelMenu::OnRedoClicked)
		];
	}
	
	controlsVertBox->AddSlot()
	.HAlign(HAlign_Center)
	.AutoHeight()
	[
		Statics::CreateNewTitle("General")
	];

	{
		auto horizonBox = SNew(SHorizontalBox);
		horizonBox->AddSlot()
		.HAlign(HAlign_Center)
		.FillWidth(1)
		[
			Statics::CreateButton<SPaperLevelMenu>(this, "Advanced", &SPaperLevelMenu::OnEraserClicked)
		];
		horizonBox->AddSlot()
		.HAlign(HAlign_Center)
		.FillWidth(1)
		[
			Statics::CreateButton<SPaperLevelMenu>(this, "Create", &SPaperLevelMenu::OnUndoClicked)
		];
		
		controlsVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::CreateInputField<SPaperLevelMenu>(this, "Map Title...", &SPaperLevelMenu::OnMapNameChanged)
		];
		controlsVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			horizonBox
		];
	}
	
	// Add Controls To Grid Splitter (next to map)
	gridAndControlsSplitter->AddSlot()
	.AutoWidth()
	.Padding(10.0f, 0)
	.VAlign(VAlign_Center)
	[
		controlsVertBox
	];

	// Add gridAndControlsSplitter To Grand Vert Box
	outerVertBox->AddSlot()
	.HAlign(HAlign_Center)
	.AutoHeight()
	[
		gridAndControlsSplitter
	];

	// Add Grand Vert Box To Grand Border
	return SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(Statics::GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				outerVertBox
			];
}

void SPaperLevelMenu::CreatePaperMapGrid()
{
	for(unsigned y = 0; y < 10; y++)
	{
		for(unsigned x = 0; x < 10; x++)
		{
			WorldGrid.Add({x,y}, Statics::CreateButton<SPaperLevelMenu>(this, FString::FromInt(x) + ":" + FString::FromInt(y), &SPaperLevelMenu::OnButtonClicked));
		}
	}
}

void SPaperLevelMenu::CreateSymbolsGrid()
{
	for(unsigned y = 0; y < 6; y++)
	{
		for(unsigned x = 0; x < 6; x++)
		{
			SymbolsGrid.Add({x,y}, Statics::CreateButton<SPaperLevelMenu>(this, FString::FromInt(x) + ":" + FString::FromInt(y), &SPaperLevelMenu::OnButtonClicked));
		}
	}
}

void SPaperLevelMenu::OnButtonClicked()
{
}

void SPaperLevelMenu::OnMapNameChanged(const FText& _newText)
{
	ToBeMapName = _newText.ToString();
}

void SPaperLevelMenu::OnEraserClicked()
{
}

void SPaperLevelMenu::OnUndoClicked()
{
}

void SPaperLevelMenu::OnRedoClicked()
{
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
