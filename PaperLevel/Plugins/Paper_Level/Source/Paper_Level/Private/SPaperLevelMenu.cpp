// Fill out your copyright notice in the Description page of Project Settings.


#include "SPaperLevelMenu.h"

#include "LineTypes.h"
#include "SlateMaterialBrush.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Brushes/SlateImageBrush.h"
#include "SlateDesigner/Public/Statics.h"
#include "Styling/SlateStyleMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "PixelFormat.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureDefines.h"

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

	ParentBox->SetOnMouseButtonUp(FPointerEventHandler::CreateSP(this, &SPaperLevelMenu::OnImageMouseButtonUp));
	
	// Assign Finished Parent (the scroll box)
	ChildSlot
	[
		scrollBox
	];
}

void SPaperLevelMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	HandleTexturePainting();
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
	.HAlign(HAlign_Left)
	[
		CreateMapImage()
	];
	
	// Add the image widget to the widget tree
	//vertBox->AddSlot()
	//.HAlign(HAlign_Center)
	//.AutoHeight()
	//[
	//	CreateMapImage()
	//];

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
	.HAlign(HAlign_Right)
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

void SPaperLevelMenu::HandleTexturePainting()
{
	auto mousePos = FSlateApplication::Get().GetCursorPos();

	if (TextureGeo && IsTrackingMousePos)
	{
		auto localMousePosition = TextureGeo->AbsoluteToLocal(mousePos);
		
		if (MapImageTexture && MapImageTexture->GetPlatformData())
		{
			if (!MapImageTexture->GetPlatformData()->Mips[0].BulkData.IsLocked())
			{
				auto mipData = static_cast<RGBA*>(MapImageTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
				
				if (FMath::RoundToInt(localMousePosition.X) >= 0 && FMath::RoundToInt(localMousePosition.X) < 500)
				{
					if (FMath::RoundToInt(localMousePosition.Y) >= 0 && FMath::RoundToInt(localMousePosition.Y) < 500)
					{
						int32 index = FMath::RoundToInt(localMousePosition.Y) * 500 + FMath::RoundToInt(localMousePosition.X);

						mipData[index].A = 1;
						mipData[index].G = 0;
						mipData[index].B = 0;
						mipData[index].R = 0;
					}
				}

				MapImageTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

				// Set the modified data to the texture
				MapImageTexture->UpdateResource();
				
				MapImageBrush->SetResourceObject(MapImageTexture);
				
				UE_LOG(LogTemp, Warning, TEXT("Texture Colour Updated") );
			}
		}
	}
}

TSharedRef<SBorder> SPaperLevelMenu::CreateMapImage()
{
	const FName ImageName("PaperLevel");
	const FVector2D ImageSize(500.0f, 500.0f);
	const FName StyleSetName("Paper_LevelStyle");

	MapImageBrush = CreateImageBrushFromStyleSet(ImageName, ImageSize, StyleSetName);

	auto image = 	SNew(SImage)
			.Image(MapImageBrush)
			.OnMouseButtonDown(this, &SPaperLevelMenu::OnImageMouseButtonDown)
			.ColorAndOpacity(FLinearColor::White);

	MapImageToPaint = &image.Get();

	TextureGeo = &MapImageToPaint->GetTickSpaceGeometry();
	
	// wrap image in a border
	return SNew(SBorder)
	.Padding(1)
	.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
	.BorderBackgroundColor(Statics::GetInnerBackgroundColor())
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	[
		MapImageToPaint->AsShared()
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

FSlateImageBrush* SPaperLevelMenu::CreateImageBrushFromStyleSet(FName ImageName, const FVector2D& ImageSize,
	const FName& StyleSetName)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().SearchAllAssets(true);
	
	MapImageTexture = UTexture2D::CreateTransient(500.0f, 500.0f, PF_A32B32G32R32F);

	// Set the texture settings and properties
	MapImageTexture->CompressionSettings = TC_Default;
	MapImageTexture->MipGenSettings = TMGS_SimpleAverage;
	MapImageTexture->AddToRoot();
	
	if (MapImageTexture->GetPlatformData()->Mips.Num() == 0)
	{
		MapImageTexture->GetPlatformData()->Mips.Add(new FTexture2DMipMap{MapImageTexture->GetPlatformData()->SizeX, MapImageTexture->GetPlatformData()->SizeY});
	}

	//MapImageTexture->CreateResource();
	MapImageTexture->UpdateResource();

	FVector2D imageSize(500.0f, 500.0f);
	MapImageBrush = new FSlateImageBrush(MapImageTexture, ImageSize);

	// Set the teexture to white
	auto mipData = static_cast<RGBA*>(MapImageTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	for(unsigned y = 0; y < 500; y++)
	{
		for(unsigned x = 0; x < 500; x++)
		{
			int32 index = y * 500 + x;

			mipData[index].A = 1;
			mipData[index].G = 1;
			mipData[index].B = 1;
			mipData[index].R = 1;
		}
	}
	
	MapImageTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

	// Set the modified data to the texture
	MapImageTexture->UpdateResource();
				
	MapImageBrush->SetResourceObject(MapImageTexture);

	// return image brush
	return new FSlateImageBrush(
			   MapImageBrush->GetResourceName(),
			   imageSize
		   );
}

FReply SPaperLevelMenu::OnImageMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		IsTrackingMousePos = true;
		UE_LOG(LogTemp, Warning, TEXT("Mosue Pressed On Texture") );
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply SPaperLevelMenu::OnImageMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		IsTrackingMousePos = false;
		UE_LOG(LogTemp, Warning, TEXT("Mouse Released") );
		return FReply::Handled();
	}

	
	
	return FReply::Unhandled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
