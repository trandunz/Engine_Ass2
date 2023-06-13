// Fill out your copyright notice in the Description page of Project Settings.


#include "SPaperLevelMenu.h"

#include "LineTypes.h"
#include "SlateMaterialBrush.h"
#include "AssetRegistry/AssetRegistryModule.h"
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
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "PackageTools.h"
#include "Engine/Texture2D.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
#include "Engine/World.h"
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "EditorAssetLibrary.h"
#include "FileHelpers.h"
#include "PackageTools.h"
#include "Engine/Level.h"
#include "GameFramework/WorldSettings.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Engine/LevelStreaming.h"
#include "EngineUtils.h"
#include "PackageTools.h"
#include <iostream>
#include <fstream>
#include "Engine/AssetManager.h"
#include "Engine/ObjectLibrary.h"
#include "Containers/UnrealString.h"
#include "Misc/OutputDeviceNull.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPaperLevelMenu::Construct(const FArguments& InArgs)
{
	// Misc
	SetCanTick(true);

	//
	// Create Menu's
	//
	auto parent = SNew(SVerticalBox);
	ParentBox = &parent.Get();
	
	InitPaperLevelMenu();

	TSharedRef<SScrollBox> scrollBox = SNew(SScrollBox);
	scrollBox->AddSlot()
	.AttachWidget(parent);

	// Setup Callbacks
	parent->SetOnMouseButtonUp(FPointerEventHandler::CreateSP(this, &SPaperLevelMenu::OnImageMouseButtonUp));
	
	ChildSlot
	[
		scrollBox
	];
	//
	//

	
	CustomErrorDevice = new FCustomOutputDeviceError{};
}

void SPaperLevelMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	HandleTexturePainting();
}

void SPaperLevelMenu::InitPaperLevelMenu()
{
	// Add menu to the parent
	ParentBox->AddSlot()
	.AutoHeight()
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(Statics::GetInnerBackgroundColor())
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			InitPaperLevelMenuContents()
		]
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
						EditTexel( MapImageTexture,
							{FMath::RoundToInt(localMousePosition.X),FMath::RoundToInt(localMousePosition.Y)},
							mipData,
							{1,0,0,0},
							TEXTUREACTION::UPDATE,
							3);
						UnlockMipsBulkData_Safe(MapImageTexture);

						///UE_LOG(LogTemp, Warning, TEXT("Texture Colour Updated") );
					}
				}
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

	ImprintCurrentMapOntoCopy();

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
	unsigned short skipTarget{3};
	int skipCounter{};

	if (SymbolTextures.Num() >= 3)
	{
		SymbolsGrid.Add({0,0}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnSpawnSymbolClicked, SymbolTextures[0]));
		SymbolsGrid.Add({1,0}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnSellBinSymbolClicked, SymbolTextures[1]));
		SymbolsGrid.Add({2,0}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnGrowPlotSymbolClicked, SymbolTextures[2]));
	}
	else
	{
		SymbolsGrid.Add({0,0}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnSpawnSymbolClicked));
		SymbolsGrid.Add({1,0}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnSellBinSymbolClicked));
		SymbolsGrid.Add({2,0}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnGrowPlotSymbolClicked));
	}

	for(unsigned y = 0; y < 6; y++)
	{
		for(unsigned x = 0; x < 6; x++)
		{
			if (skipCounter < skipTarget)
			{
				skipCounter++;
			}
			else
			{
				if (SymbolTextures.Num() >= 4)
				{
					SymbolsGrid.Add({x,y}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnSpawnSymbolClicked, SymbolTextures[3]));
				}
				else
				{
					SymbolsGrid.Add({x,y}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnSpawnSymbolClicked));
				}
			}
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

void SPaperLevelMenu::OnSpawnSymbolClicked()
{
}

void SPaperLevelMenu::OnSellBinSymbolClicked()
{
}

void SPaperLevelMenu::OnGrowPlotSymbolClicked()
{
}

void SPaperLevelMenu::OnCreateLevelClicked()
{
	CreateAndAddLevelAsset();
}

void SPaperLevelMenu::OnClearClicked()
{
	//LastAction = &SPaperLevelMenu::OnClearClicked;
	ImprintCurrentMapOntoCopy();
	
	auto mipData = static_cast<RGBA*>(MapImageTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	EditTexel(MapImageTexture, {250, 250}, mipData, {1,1,1,1}, TEXTUREACTION::UPDATE, 250);
	UnlockMipsBulkData_Safe(MapImageTexture);
}

void SPaperLevelMenu::OnEraserClicked()
{
	//LastAction = &SPaperLevelMenu::OnEraserClicked;
}

void SPaperLevelMenu::OnUndoClicked()
{
	ImprintCopyMapOntoCurrent();
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

	//
	// Setup duplicate for undo tool
	//
	LastMapImageTexture = UTexture2D::CreateTransient(500.0f, 500.0f, PF_A32B32G32R32F);
	// Set the texture settings and properties
	LastMapImageTexture->CompressionSettings = TC_Default;
	LastMapImageTexture->MipGenSettings = TMGS_SimpleAverage;
	LastMapImageTexture->AddToRoot();
	if (LastMapImageTexture->GetPlatformData()->Mips.Num() == 0)
	{
		LastMapImageTexture->GetPlatformData()->Mips.Add(new FTexture2DMipMap{LastMapImageTexture->GetPlatformData()->SizeX, LastMapImageTexture->GetPlatformData()->SizeY});
	}
	//
	//

	//MapImageTexture->CreateResource();
	LastMapImageTexture->UpdateResource();

	FVector2D imageSize(500.0f, 500.0f);
	MapImageBrush = new FSlateImageBrush(MapImageTexture, ImageSize);

	// Set the texture to white
	auto mipData = static_cast<RGBA*>(MapImageTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	for(int y = 0; y < 500; y++)
	{
		for(int x = 0; x < 500; x++)
		{
			if (y == 499 && x == 499)
				EditTexel(MapImageTexture, {x,y}, mipData, {1,1,1,1}, TEXTUREACTION::UPDATE);
			else
				EditTexel(MapImageTexture, {x,y}, mipData, {1,1,1,1});
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
		//UE_LOG(LogTemp, Warning, TEXT("Mosue Pressed On Texture") );
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply SPaperLevelMenu::OnImageMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		IsTrackingMousePos = false;
		//UE_LOG(LogTemp, Warning, TEXT("Mouse Released") );
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

void SPaperLevelMenu::EditTexel(UTexture2D* _texture, FIntVector2 _texel, RGBA* _mipData, RGBA _newColor, TEXTUREACTION _postUpdateAction, int _radius)
{
	int32 index{};
	for(int32 i = FMath::Clamp<int32>(_texel.Y - _radius, 0, 500); i < FMath::Clamp<int32>(_texel.Y + _radius, 0, 500); i++)
	{
		for(int32 j = FMath::Clamp<int32>(_texel.X - _radius, 0, 500);j < FMath::Clamp<int32>(_texel.X + _radius, 0, 500); j++)
		{
			index = i * 500 + j;

			//UE_LOG(LogTemp, Warning, TEXT("Index Clicked: %s"), *FString::FromInt(index) );
			
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
			_texture->GetPlatformData()->Mips[0].BulkData.Unlock();

			// Set the modified data to the texture
			_texture->UpdateResource();
				
			MapImageBrush->SetResourceObject(_texture);
			
			break;
		}
	default:
		break;
	}
}

void SPaperLevelMenu::UnlockMipsBulkData_Safe(UTexture2D* _texture)
{
	if (_texture->GetPlatformData()->Mips[0].BulkData.IsLocked())
	{
		_texture->GetPlatformData()->Mips[0].BulkData.Unlock();
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
		// Create all the textures
		CreateSymbolsTextures();
		
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
				Statics::CreateButton<SPaperLevelMenu>(this, "Undo", &SPaperLevelMenu::OnUndoClicked)
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
			/*symblsVertBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "New Symbol", &SPaperLevelMenu::OnRedoClicked)
			];*/
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

		if (SymbolTextures.Num() >= 7)
		{
			auto horizonBox = SNew(SHorizontalBox);
			horizonBox->AddSlot()
			.HAlign(HAlign_Center)
			.FillWidth(1)
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnEraserClicked, SymbolTextures[4]) // Circle
			];
			horizonBox->AddSlot()
			.HAlign(HAlign_Center)
			.FillWidth(1)
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnUndoClicked, SymbolTextures[5]) // Square
			];
			horizonBox->AddSlot()
			.HAlign(HAlign_Center)
			.FillWidth(1)
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnRedoClicked, SymbolTextures[6]) // Triangle
			];
		
			WorldBoundVertBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				horizonBox
			];
		}
		
		auto horizonBoxControls = SNew(SHorizontalBox);
		horizonBoxControls->AddSlot()
		.HAlign(HAlign_Center)
		.FillWidth(1)
		[
			Statics::CreateButton<SPaperLevelMenu>(this, "Undo", &SPaperLevelMenu::OnUndoClicked)
		];
		horizonBoxControls->AddSlot()
		.HAlign(HAlign_Center)
		.FillWidth(1)
		[
			Statics::CreateButton<SPaperLevelMenu>(this, "Clear", &SPaperLevelMenu::OnClearClicked)
		];
		
		WorldBoundVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			horizonBoxControls
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
			Statics::CreateButton<SPaperLevelMenu>(this, "Create", &SPaperLevelMenu::OnCreateLevelClicked)
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

void SPaperLevelMenu::CreateSymbolsTextures()
{
	for(int i = 0; i < 7; i++)
	{
		auto pluginDir = IPluginManager::Get().FindPlugin("Paper_Level")->GetBaseDir() / TEXT("Resources");
		
		FString texturePath;
		FString fileName;
		switch(i)
		{
		case 0:
			{
				fileName = "SpawnIcon";
				texturePath = FPaths::Combine(*pluginDir, TEXT("SpawnIcon.png"));
				break;
			}
		case 1:
			{
				fileName = "SpawnIcon";
				texturePath = FPaths::Combine(*pluginDir, TEXT("SellBinIcon.png"));
				break;
			}
		case 2:
			{
				fileName = "SpawnIcon";
				texturePath = FPaths::Combine(*pluginDir, TEXT("PlotIcon.png"));
				break;
			}
		case 3:
			{
				fileName = "SpawnIcon";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Icon128.png"));
				break;
			}
		case 4:
			{
				fileName = "SpawnIcon";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Circle.png"));
				break;
			}
		case 5:
			{
				fileName = "SpawnIcon";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Square.png"));
				break;
			}
		case 6:
			{
				fileName = "SpawnIcon";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Triangle.png"));
				break;
			}
		default:
			{
				fileName = "SpawnIcon";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Icon128.png"));
			}
			break;
		}
		
		TArray<uint8> FileData;
		if (!FFileHelper::LoadFileToArray(FileData, *texturePath))
		{
			// Failed to load the image file
			return;
		}
		
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		if (!ImageWrapper.IsValid())
		{
			// Failed to create the image wrapper
			return;
		}
		SymbolImageWrappers.Add(ImageWrapper);

		if (!ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num()))
		{
			// Failed to set the compressed data
			return;
		}

		TArray<uint8> RawData;
		if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
		{
			// Failed to get the raw image data
			return;
		}

		auto texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
		texture->AddToRoot();
		SymbolTextures.Add(texture);

		FTexture2DMipMap& Mip = SymbolTextures[SymbolTextures.Num() - 1]->GetPlatformData()->Mips[0];
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(Data, RawData.GetData(), RawData.Num());
		Mip.BulkData.Unlock();

		SymbolTextures[SymbolTextures.Num() - 1]->UpdateResource();
	}
}

void SPaperLevelMenu::ImprintCurrentMapOntoCopy()
{
	FTexture2DMipMap& Mip = MapImageTexture->GetPlatformData()->Mips[0];
	FTexture2DMipMap& Mip2 = LastMapImageTexture->GetPlatformData()->Mips[0];
	if (!Mip.BulkData.IsLocked() && !Mip2.BulkData.IsLocked())
	{
		auto Data = static_cast<RGBA*>(Mip.BulkData.Lock(LOCK_READ_WRITE));
		auto Data2 = static_cast<RGBA*>(Mip2.BulkData.Lock(LOCK_READ_WRITE));
		
		for(int i = 0; i < 500*500; i++)
		{
			Data2[i] = Data[i];
		}
	
		Mip.BulkData.Unlock();
		Mip2.BulkData.Unlock();

		MapImageTexture->UpdateResource();
		LastMapImageTexture->UpdateResource();

		MapImageBrush->SetResourceObject(MapImageTexture);
	}
	UE_LOG(LogTemp, Warning, TEXT("Imprint Current Map Onto Copy") );
}

void SPaperLevelMenu::ImprintCopyMapOntoCurrent()
{
	FTexture2DMipMap& Mip = MapImageTexture->GetPlatformData()->Mips[0];
	FTexture2DMipMap& Mip2 = LastMapImageTexture->GetPlatformData()->Mips[0];
	if (!Mip.BulkData.IsLocked() && !Mip2.BulkData.IsLocked())
	{
		auto Data = static_cast<RGBA*>(Mip.BulkData.Lock(LOCK_READ_WRITE));
		auto Data2 = static_cast<RGBA*>(Mip2.BulkData.Lock(LOCK_READ_WRITE));

		for(int i = 0; i < 500*500; i++)
		{
			Data[i] = Data2[i];
		}
	
		Mip.BulkData.Unlock();
		Mip2.BulkData.Unlock();

		MapImageTexture->UpdateResource();
		LastMapImageTexture->UpdateResource();

		MapImageBrush->SetResourceObject(MapImageTexture);
	}
	UE_LOG(LogTemp, Warning, TEXT("Imprint Copy Map Onto Current") );
}

void SPaperLevelMenu::CreateAndAddLevelAsset()
{
	//auto pluginDir = IPluginManager::Get().FindPlugin("Paper_Level")->GetBaseDir() / TEXT("Resources");
	Statics::CreateObject("/GAME/SomeLevel", ToBeMapName);
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
