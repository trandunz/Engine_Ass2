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
		.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(Statics::GetInnerBackgroundColor())
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
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
	.Padding(10.0f, 10.0f)
	.HAlign(HAlign_Left)
	[
		CreateMapImage()
	];
	
	// Add Controls To Grid Splitter (next to map)
	gridAndControlsSplitter->AddSlot()
	.AutoWidth()
	.Padding(10.0f, 10.0f)
	.HAlign(HAlign_Right)
	.VAlign(VAlign_Center)
	[
		CreateControlsVertBox()
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
		.Padding(10.0f)
		.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(Statics::GetInnerBackgroundColor())
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
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
						EditTexel(
							{FMath::RoundToInt(localMousePosition.X),FMath::RoundToInt(localMousePosition.Y)},
							mipData,
							{1,0,0,0},
							TEXTUREACTION::UPDATE,
							3);
					}
				}
				
				UE_LOG(LogTemp, Warning, TEXT("Texture Colour Updated") );

				UnlockMipsBulkData_Safe();
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
		.Padding(10)
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

void SPaperLevelMenu::OnClearClicked()
{
	//LastAction = &SPaperLevelMenu::OnClearClicked;
	auto mipData = static_cast<RGBA*>(MapImageTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	EditTexel({250, 250}, mipData, {1,1,1,1}, TEXTUREACTION::UPDATE, 250);
	UnlockMipsBulkData_Safe();
}

void SPaperLevelMenu::OnEraserClicked()
{
	//LastAction = &SPaperLevelMenu::OnEraserClicked;
}

void SPaperLevelMenu::OnUndoClicked()
{
	//LastAction();
}

void SPaperLevelMenu::OnRedoClicked()
{
	//LastAction = &SPaperLevelMenu::OnRedoClicked;
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

	// Set the texture to white
	auto mipData = static_cast<RGBA*>(MapImageTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	for(int y = 0; y < 500; y++)
	{
		for(int x = 0; x < 500; x++)
		{
			if (y == 499 && x == 499)
				EditTexel({x,y}, mipData, {1,1,1,1}, TEXTUREACTION::UPDATE);
			else
				EditTexel({x,y}, mipData, {1,1,1,1});
		}
	}

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

void SPaperLevelMenu::EditTexel(FIntVector2 _texel, RGBA* _mipData, RGBA _newColor, TEXTUREACTION _postUpdateAction, int _radius)
{
	int32 index{};
	for(int32 i = FMath::Clamp<int32>(_texel.Y - _radius, 0, 500); i < FMath::Clamp<int32>(_texel.Y + _radius, 0, 500); i++)
	{
		for(int32 j = FMath::Clamp<int32>(_texel.X - _radius, 0, 500);j < FMath::Clamp<int32>(_texel.X + _radius, 0, 500); j++)
		{
			index = i * 500 + j;

			UE_LOG(LogTemp, Warning, TEXT("Index Clicked: %s"), *FString::FromInt(index) );
			
			_mipData[index].A = _newColor.A;
			_mipData[index].G = _newColor.G;
			_mipData[index].B = _newColor.B;
			_mipData[index].R = _newColor.R;
		}
	}
	
	switch (_postUpdateAction)
	{
	case TEXTUREACTION::UPDATE:
		{
			MapImageTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

			// Set the modified data to the texture
			MapImageTexture->UpdateResource();
				
			MapImageBrush->SetResourceObject(MapImageTexture);
			
			break;
		}
	default:
		break;
	}
}

void SPaperLevelMenu::UnlockMipsBulkData_Safe()
{
	if (MapImageTexture->GetPlatformData()->Mips[0].BulkData.IsLocked())
	{
		MapImageTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	}
}

TSharedRef<SVerticalBox> SPaperLevelMenu::CreateControlsVertBox()
{
	auto controlsVertBox = SNew(SVerticalBox);
	{
		controlsVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::WrapWidgetInBorder(CreateSymblsVertBox())
		];
		
		controlsVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::WrapWidgetInBorder(CreateWrldBndsVertBox())
		];

		controlsVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::WrapWidgetInBorder(CreateGnrlVertBox())
		];
	}

	return controlsVertBox;
}

TSharedRef<SVerticalBox> SPaperLevelMenu::CreateSymblsVertBox()
{
	auto symblsVertBox = SNew(SVerticalBox);
	{
		// Create all the buttons
		CreateSymbolsGrid();

		// Add Symbol Constols Title
		symblsVertBox->AddSlot()
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
		
			symblsVertBox->AddSlot()
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
			horizonBox->AddSlot()
			.HAlign(HAlign_Center)
			.FillWidth(1)
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "Clear", &SPaperLevelMenu::OnClearClicked)
			];

			symblsVertBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				horizonBox
			];
			symblsVertBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "New Symbol", &SPaperLevelMenu::OnRedoClicked)
			];
		}
	}

	return symblsVertBox;
}

TSharedRef<SVerticalBox> SPaperLevelMenu::CreateWrldBndsVertBox()
{
	auto WorldBoundVertBox = SNew(SVerticalBox);
	{
		WorldBoundVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::CreateNewTitle("World Boundary")
		];
		
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
	
		WorldBoundVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			horizonBox
		];
		WorldBoundVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::CreateButton<SPaperLevelMenu>(this, "Apply", &SPaperLevelMenu::OnRedoClicked)
		];
	}

	return WorldBoundVertBox;
}

TSharedRef<SVerticalBox> SPaperLevelMenu::CreateGnrlVertBox()
{
	auto GeneralBoundVertBox = SNew(SVerticalBox);
	{
		GeneralBoundVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::CreateNewTitle("General")
		];
		
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
			
		GeneralBoundVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::CreateInputField<SPaperLevelMenu>(this, "Map Title...", &SPaperLevelMenu::OnMapNameChanged)
		];
		GeneralBoundVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			horizonBox
		];
	}

	return GeneralBoundVertBox;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
