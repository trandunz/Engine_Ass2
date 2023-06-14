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

	auto grandOverlay = SNew(SOverlay);
	GrandOverlay = &grandOverlay.Get();
	auto parent = SNew(SVerticalBox);
	ParentBox = &parent.Get();
	
	InitPaperLevelMenu();
	
	TSharedRef<SScrollBox> scrollBox = SNew(SScrollBox);
	scrollBox->AddSlot()
	.AttachWidget(parent);
	
	grandOverlay->AddSlot()
	.AttachWidget(scrollBox);

	// Setup Callbacks
	grandOverlay->SetOnMouseButtonUp(FPointerEventHandler::CreateSP(this, &SPaperLevelMenu::OnImageMouseButtonUp));
	grandOverlay->SetOnMouseButtonDown(FPointerEventHandler::CreateSP(this, &SPaperLevelMenu::OnImageMouseButtonDown));
	parent->SetOnMouseButtonUp(FPointerEventHandler::CreateSP(this, &SPaperLevelMenu::OnImageMouseButtonUp));
	parent->SetOnMouseButtonDown(FPointerEventHandler::CreateSP(this, &SPaperLevelMenu::OnImageMouseButtonDown));

	auto image = CreatePreviewSymbolImage();
	PreviewSymbolImage = &image.Get();
	GrandOverlay->AddSlot()
	.HAlign(HAlign_Left)
	.VAlign(VAlign_Top)
	[
		image
	];
	
	ChildSlot
	[
		grandOverlay
	];
	//
	//

	
	CustomErrorDevice = new FCustomOutputDeviceError{};

	TSharedPtr<SWindow> OwningWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
	if (OwningWindow.IsValid())
	{
		OwningWindow->SetOnWindowClosed(FOnWindowClosed::CreateSP(this, &SPaperLevelMenu::OnWindowClosed));
	}

}

void SPaperLevelMenu::OnWindowClosed(const TSharedRef<SWindow>& WindowBeingClosed)
{
	if (WindowBeingClosed == FSlateApplication::Get().GetActiveTopLevelWindow())
	{
		if (MapImageTexture && LastMapImageTexture)
		{
			UnlockMipsBulkData_Safe(MapImageTexture);
			MapImageTexture->GetResource()->ReleaseResource();
			MapImageTexture->RemoveFromRoot();
			MapImageTexture->MarkAsGarbage();
			UnlockMipsBulkData_Safe(LastMapImageTexture);
			LastMapImageTexture->GetResource()->ReleaseResource();
			LastMapImageTexture->RemoveFromRoot();
			LastMapImageTexture->MarkAsGarbage();
		}

		for(auto texture : SymbolTextures)
		{
			if (texture)
			{
				UnlockMipsBulkData_Safe(texture);
				texture->GetResource()->ReleaseResource();
				texture->RemoveFromRoot();
				texture->MarkAsGarbage();
			}
		}

		if (PreviewSymbolTexture)
		{
			UnlockMipsBulkData_Safe(PreviewSymbolTexture);
			PreviewSymbolTexture->GetResource()->ReleaseResource();
			PreviewSymbolTexture->RemoveFromRoot();
			PreviewSymbolTexture->MarkAsGarbage();
		}
	}
}


void SPaperLevelMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	HandleTexturePainting();
	SetPreviewSymbolImageVisiblity();
	//UnlockMipsBulkData_Safe(MapImageTexture);
	//UnlockMipsBulkData_Safe(LastMapImageTexture);
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

	if (IsStamping)
	{
		HandleSymbolStamping();
		return;
	}
	
	if (TextureGeo && IsTrackingMousePos)
	{
		auto localMousePosition = TextureGeo->AbsoluteToLocal(mousePos);
		
		if (MapImageTexture && MapImageTexture->GetPlatformData())
		{
			if (!MapImageTexture->GetPlatformData()->Mips[0].BulkData.IsLocked())
			{
				if (auto mipData = static_cast<RGBA*>(MapImageTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE)))
				{
					if (FMath::RoundToInt(localMousePosition.X) >= 0 && FMath::RoundToInt(localMousePosition.X) < 500)
					{
						if (FMath::RoundToInt(localMousePosition.Y) >= 0 && FMath::RoundToInt(localMousePosition.Y) < 500)
						{
							EditTexel( MapImageTexture,
								{FMath::RoundToInt(localMousePosition.X),FMath::RoundToInt(localMousePosition.Y)},
								mipData,
								{0,0,0,1},
								MapImageBrush,
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
}

void SPaperLevelMenu::HandleSymbolStamping()
{
	// Get the current mouse position in screen coordinates
	FVector2D mousePosition = FSlateApplication::Get().GetCursorPos();

	const FGeometry& parentGeometry = GrandOverlay->GetTickSpaceGeometry();

	// Convert the mouse position to the parent widget's local space
	FVector2D localMousePosition = parentGeometry.AbsoluteToLocal(mousePosition);

	FVector2D offset = FVector2D((PreviewSymbolTexture->GetResource()->GetSizeX() / 2), (PreviewSymbolTexture->GetResource()->GetSizeY() / 2));
	localMousePosition -= offset;
	FSlateRenderTransform renderTransform;
	renderTransform.SetTranslation(localMousePosition);
	PreviewSymbolImage->SetRenderTransform(renderTransform);
}

TSharedRef<SBorder> SPaperLevelMenu::CreateMapImage()
{
	const FName ImageName("PaperLevel");
	const FVector2D ImageSize(500.0f, 500.0f);
	const FName StyleSetName("Paper_LevelStyle");

	MapImageTexture = UTexture2D::CreateTransient(ImageSize.X, ImageSize.Y, PF_B8G8R8A8);
	MapImageBrush = CreateImageBrushFromStyleSet(ImageName, ImageSize, StyleSetName, MapImageTexture, MapImageBrush);

	LastMapImageTexture = UTexture2D::CreateTransient(500.0f, 500.0f, PF_B8G8R8A8);
	CreateDuplicateMapTextureForUndo(ImageSize);
	LastMapImageTexture->UpdateResource();
	
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

TSharedRef<SImage>  SPaperLevelMenu::CreatePreviewSymbolImage()
{
	const FName ImageName("PreviewSymbol");
	const FVector2D ImageSize(PreviewSymbolTextureSize.X, PreviewSymbolTextureSize.Y);
	const FName StyleSetName("Paper_LevelStyle");

	PreviewSymbolTexture = UTexture2D::CreateTransient(ImageSize.X, ImageSize.Y, PF_B8G8R8A8);
	PreviewSymbolImageBrush = CreateImageBrushFromStyleSet(ImageName, {PreviewSymbolTextureSize.X,PreviewSymbolTextureSize.Y}, StyleSetName, PreviewSymbolTexture, PreviewSymbolImageBrush);
	PreviewSymbolTextureGeo = &PreviewSymbolImage->GetTickSpaceGeometry();
	
	return SNew(SImage)
			.Image(PreviewSymbolImageBrush)
			.ColorAndOpacity(FLinearColor::White);
	
}


void SPaperLevelMenu::CreatePaperMapGrid()
{
	for(unsigned y = 0; y < 10; y++)
	{
		for(unsigned x = 0; x < 10; x++)
		{
			WorldGrid.Add({x,y}, Statics::CreateButton<SPaperLevelMenu>(this, FString::FromInt(x) + ":" + FString::FromInt(y), &SPaperLevelMenu::OnSymbolClicked, 1, nullptr));
		}
	}
}

void SPaperLevelMenu::CreateSymbolsGrid()
{
	for(unsigned y = 0; y < 6; y++)
	{
		for(unsigned x = 0; x < 6; x++)
		{
			int index = y * 6 + x;
			if (index < SymbolTextures.Num() && index <= 3)
				SymbolsGrid.Add({x,y}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnSymbolClicked, index, SymbolTextures[index]));
			else
				SymbolsGrid.Add({x,y}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnSymbolClicked, index, SymbolTextures[3]));
		}
	}
}

void SPaperLevelMenu::OnMapNameChanged(const FText& _newText)
{
	ToBeMapName = _newText.ToString();
}

void SPaperLevelMenu::OnSymbolClicked(int _symbol)
{
	if (_symbol < SymbolTextures.Num())
	{
		IsStamping = true;
		
		FTexture2DMipMap& Mip = PreviewSymbolTexture->GetPlatformData()->Mips[0];
		FTexture2DMipMap& Mip2 = SymbolTextures[_symbol]->GetPlatformData()->Mips[0];
		if (!Mip.BulkData.IsLocked() && !Mip2.BulkData.IsLocked())
		{
			auto Data = static_cast<RGBA*>(Mip.BulkData.Lock(LOCK_READ_WRITE));
			auto Data2 = static_cast<RGBA*>(Mip2.BulkData.Lock(LOCK_READ_WRITE));
		
			for(int i = 0; i < PreviewSymbolTexture->GetPlatformData()->SizeX*PreviewSymbolTexture->GetPlatformData()->SizeY; i++)
			{
				Data[i] = Data2[i];
			}
	
			Mip.BulkData.Unlock();
			Mip2.BulkData.Unlock();

			PreviewSymbolTexture->UpdateResource();

			PreviewSymbolImageBrush->SetResourceObject(PreviewSymbolTexture);
		}
	}
}

void SPaperLevelMenu::OnCreateLevelClicked()
{
	CreateAndAddLevelAsset();
}

void SPaperLevelMenu::OnRenameLevelClicked()
{
	// Rename the current level
	if (auto* currentLevel = GEditor->GetEditorWorldContext().World()->GetCurrentLevel())
	{
		FAssetRenameData RenameData;
		RenameData.Asset = currentLevel;
		RenameData.NewName = ToBeMapName;

		TArray<FAssetRenameData> AssetsToRename;
		AssetsToRename.Add(RenameData);
		
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.RenameAssets(AssetsToRename);
	}
}

void SPaperLevelMenu::OnClearClicked()
{
	//LastAction = &SPaperLevelMenu::OnClearClicked;
	ImprintCurrentMapOntoCopy();

	if (MapImageTexture->GetPlatformData()->Mips[0].BulkData.IsUnlocked())
	{
		if (auto mipData = static_cast<RGBA*>(MapImageTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE)))
		{
			EditTexel(MapImageTexture, {250, 250}, mipData, {255,255,255,255}, MapImageBrush,TEXTUREACTION::UPDATE, 250);
		}
	}
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
	const FName& StyleSetName, UTexture2D* Texture, FSlateImageBrush* ImageBrush)
{
	if (!Texture)
		return nullptr;
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().SearchAllAssets(true);

	// Set the texture settings and properties
	Texture->CompressionSettings = TC_Default;
	Texture->MipGenSettings = TMGS_SimpleAverage;

	
	if (Texture->GetPlatformData()->Mips.Num() == 0)
	{
		Texture->GetPlatformData()->Mips.Add(new FTexture2DMipMap{Texture->GetPlatformData()->SizeX, Texture->GetPlatformData()->SizeY});
	}
	
	Texture->UpdateResource();
	
	ImageBrush = new FSlateImageBrush(Texture, ImageSize);

	// Set the texture to white
	// 
	if (auto mipData = static_cast<RGBA*>(Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE)))
	{
		for(int y = 0; y < ImageSize.Y; y++)
		{
			for(int x = 0; x < ImageSize.X; x++)
			{
				if (y == ImageSize.Y - 1 && x == ImageSize.X - 1)
					EditTexel(Texture, {x,y}, mipData, {255,255,255,255}, ImageBrush,TEXTUREACTION::UPDATE);
				else
					EditTexel(Texture, {x,y}, mipData, {255,255,255,255},ImageBrush);
			}
		}
	}
	
	// return image brush
	return new FSlateImageBrush(
			   ImageBrush->GetResourceName(),
			   ImageSize
		   );
}

void SPaperLevelMenu::CreateDuplicateMapTextureForUndo(const FVector2D& ImageSize)
{
	//
	// Setup duplicate for undo tool
	//
	
	// Set the texture settings and properties
	LastMapImageTexture->CompressionSettings = TC_Default;
	LastMapImageTexture->MipGenSettings = TMGS_SimpleAverage;
	LastMapImageTexture->AddToRoot();
	if (LastMapImageTexture->GetPlatformData()->Mips.Num() == 0)
	{
		LastMapImageTexture->GetPlatformData()->Mips.Add(new FTexture2DMipMap{LastMapImageTexture->GetPlatformData()->SizeX, LastMapImageTexture->GetPlatformData()->SizeY});
	}

	LastMapImageTexture->UpdateResource();
	
}

FReply SPaperLevelMenu::OnImageMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (IsStamping)
		{
			IsStamping = false;

			ImprintCurrentMapOntoCopy();
			// Mips
			FTexture2DMipMap& Mip = PreviewSymbolTexture->GetPlatformData()->Mips[0];
			FTexture2DMipMap& Mip2 = PreviewSymbolTexture->GetPlatformData()->Mips[0];
			if (!Mip.BulkData.IsLocked() && !Mip2.BulkData.IsLocked())
			{
				auto Data = static_cast<RGBA*>(Mip.BulkData.Lock(LOCK_READ_WRITE));
				auto Data2 = static_cast<RGBA*>(Mip2.BulkData.Lock(LOCK_READ_WRITE));
				for(int y = 0; y < Mip.SizeY; y++)
				{
					for(int x = 0; x < Mip.SizeX; x++)
					{
						if (y == Mip.SizeY - 1 && x == Mip.SizeX - 1)
							EditTexel(MapImageTexture, {x,y}, Data,  Data[y * Mip.SizeX + x],MapImageBrush, TEXTUREACTION::UPDATE);
						else
							EditTexel(MapImageTexture, {x,y}, Data,  Data[y * Mip.SizeX + x],MapImageBrush);
					}
				}
			}
		}
		else
		{
			IsTrackingMousePos = true;
		}
		
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply SPaperLevelMenu::OnImageMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		IsTrackingMousePos = false;
		IsStamping = false;

		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

void SPaperLevelMenu::EditTexel(UTexture2D* _texture, FIntVector2 _texel, RGBA* _mipData, RGBA _newColor, FSlateImageBrush* _ImageBrush, TEXTUREACTION _postUpdateAction, int _radius)
{
	int32 index{};
	for(int32 i = FMath::Clamp<int32>(_texel.Y - _radius, 0, _texture->GetPlatformData()->SizeY); i < FMath::Clamp<int32>(_texel.Y + _radius, 0, _texture->GetPlatformData()->SizeY); i++)
	{
		for(int32 j = FMath::Clamp<int32>(_texel.X - _radius, 0, _texture->GetPlatformData()->SizeX);j < FMath::Clamp<int32>(_texel.X + _radius, 0, _texture->GetPlatformData()->SizeX); j++)
		{
			index = i * _texture->GetPlatformData()->SizeX + j;

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
			if (_texture)
			{
				_texture->GetPlatformData()->Mips[0].BulkData.Unlock();

				// Set the modified data to the texture
				_texture->UpdateResource();

				if (_ImageBrush)
					_ImageBrush->SetResourceObject(_texture);
			}

			
			
			break;
		}
	default:
		break;
	}
}

void* SPaperLevelMenu::LockMipsBulkData_Safe(UTexture2D* _texture)
{
	if (!_texture->GetPlatformData()->Mips[0].BulkData.IsLocked())
	{
		return _texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	}
	else
	{
		return nullptr;
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
			Statics::CreateButton<SPaperLevelMenu>(this, "Rename", &SPaperLevelMenu::OnRenameLevelClicked)
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
				fileName = "SellBinIcon";
				texturePath = FPaths::Combine(*pluginDir, TEXT("SellBinIcon.png"));
				break;
			}
		case 2:
			{
				fileName = "PlotIcon";
				texturePath = FPaths::Combine(*pluginDir, TEXT("PlotIcon.png"));
				break;
			}
		case 4:
			{
				fileName = "Circle";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Circle.png"));
				break;
			}
		case 5:
			{
				fileName = "Square";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Square.png"));
				break;
			}
		case 6:
			{
				fileName = "Triangle";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Triangle.png"));
				break;
			}
		default:
			{
				fileName = "Icon128";
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

		TArray<uint8> ResizedData;
		if (!ResizeImageData(RawData, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), 80, 80, ResizedData))
		{
			// Failed to resize the image data
			return;
		}

		auto texture = UTexture2D::CreateTransient(SymbolTextureSize.X, SymbolTextureSize.Y, PF_B8G8R8A8);
		texture->AddToRoot();
		SymbolTextures.Add(texture);

		FTexture2DMipMap& Mip = SymbolTextures[SymbolTextures.Num() - 1]->GetPlatformData()->Mips[0];
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(Data, ResizedData.GetData(), ResizedData.Num());
		Mip.BulkData.Unlock();

		SymbolTextures[SymbolTextures.Num() - 1]->UpdateResource();
	}
}

void SPaperLevelMenu::ImprintCurrentMapOntoCopy()
{
	// 
	if (MapImageTexture && LastMapImageTexture)
	{
		FTexture2DMipMap& Mip = MapImageTexture->GetPlatformData()->Mips[0];
		FTexture2DMipMap& Mip2 = LastMapImageTexture->GetPlatformData()->Mips[0];
		if (!Mip.BulkData.IsLocked() && !Mip2.BulkData.IsLocked())
		{
			auto Data = static_cast<RGBA*>(Mip.BulkData.Lock(LOCK_READ_WRITE));
			auto Data2 = static_cast<RGBA*>(Mip2.BulkData.Lock(LOCK_READ_WRITE));
			for(int i = 0; i < LastMapImageTexture->GetPlatformData()->SizeX*LastMapImageTexture->GetPlatformData()->SizeY; i++)
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
}

void SPaperLevelMenu::ImprintCopyMapOntoCurrent()
{
	if (MapImageTexture && LastMapImageTexture)
	{
		FTexture2DMipMap& Mip = MapImageTexture->GetPlatformData()->Mips[0];
		FTexture2DMipMap& Mip2 = LastMapImageTexture->GetPlatformData()->Mips[0];
		if (!Mip.BulkData.IsLocked() && !Mip2.BulkData.IsLocked())
		{
			auto Data = static_cast<RGBA*>(Mip.BulkData.Lock(LOCK_READ_WRITE));
			auto Data2 = static_cast<RGBA*>(Mip2.BulkData.Lock(LOCK_READ_WRITE));
			for(int i = 0; i < MapImageTexture->GetPlatformData()->SizeX*MapImageTexture->GetPlatformData()->SizeY; i++)
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
}

void SPaperLevelMenu::CreateAndAddLevelAsset()
{
	//auto pluginDir = IPluginManager::Get().FindPlugin("Paper_Level")->GetBaseDir() / TEXT("Resources");
	Statics::CreateObject("/GAME/SomeLevel", ToBeMapName);
}

void SPaperLevelMenu::SetPreviewSymbolImageVisiblity()
{
	if (IsStamping)
	{
		PreviewSymbolImage->SetVisibility(EVisibility::Visible);
	}
	else
	{
		PreviewSymbolImage->SetVisibility(EVisibility::Hidden);
	}
}

bool SPaperLevelMenu::ResizeImageData(const TArray<uint8>& SourceData, int32 SourceWidth, int32 SourceHeight,
	int32 DestWidth, int32 DestHeight, TArray<uint8>& DestData)
{
	// Check if the source image is already the desired size
	if (SourceWidth == DestWidth && SourceHeight == DestHeight)
	{
		DestData = SourceData;
		return true;
	}

	// Resize the destination data array to match the desired size
	const int32 NumChannels = 4; // RGBA format
	DestData.Empty(DestWidth * DestHeight * NumChannels);
	DestData.AddZeroed(DestWidth * DestHeight * NumChannels);

	// Calculate the scaling factors for resizing
	float ScaleX = static_cast<float>(DestWidth) / static_cast<float>(SourceWidth);
	float ScaleY = static_cast<float>(DestHeight) / static_cast<float>(SourceHeight);

	// Iterate over each destination pixel and sample the corresponding source pixel
	for (int32 Y = 0; Y < DestHeight; ++Y)
	{
		for (int32 X = 0; X < DestWidth; ++X)
		{
			// Calculate the corresponding source pixel coordinates
			int32 SourceX = static_cast<int32>(X / ScaleX);
			int32 SourceY = static_cast<int32>(Y / ScaleY);

			// Calculate the index of the source pixel in the source data array
			int32 SourceIndex = (SourceY * SourceWidth + SourceX) * NumChannels;

			// Calculate the index of the destination pixel in the destination data array
			int32 DestIndex = (Y * DestWidth + X) * NumChannels;

			// Copy the RGBA values from the source to the destination
			DestData[DestIndex + 0] = SourceData[SourceIndex + 0]; // Red
			DestData[DestIndex + 1] = SourceData[SourceIndex + 1]; // Green
			DestData[DestIndex + 2] = SourceData[SourceIndex + 2]; // Blue
			DestData[DestIndex + 3] = SourceData[SourceIndex + 3]; // Alpha
		}
	}

	return true;
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
