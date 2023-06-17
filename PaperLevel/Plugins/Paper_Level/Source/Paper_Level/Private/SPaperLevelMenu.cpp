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

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Components/BoxComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/ObjectLibrary.h"
#include "Containers/UnrealString.h"
#include "GameFramework/PlayerStart.h"
#include "Misc/OutputDeviceNull.h"

#include "Editor/EditorEngine.h"
#include "EditorViewportClient.h"
#include "EditorViewportCommands.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPaperLevelMenu::Construct(const FArguments& InArgs)
{
	// Misc
	SetCanTick(true);
	
	auto grandOverlay = SNew(SOverlay);
	GrandOverlay = &grandOverlay.Get();
	auto parent = SNew(SVerticalBox);
	ParentBox = &parent.Get();
	
	InitPaperLevelMenu();

	TSharedRef<SScrollBox> scrollBox = SNew(SScrollBox);
	scrollBox->AddSlot()
	.AttachWidget(ParentBox->AsShared());
	
	GrandOverlay->AddSlot()
	.AttachWidget(scrollBox);

	// Setup Callbacks
	InitMouseCallbacks();
	
	CustomErrorDevice = new FCustomOutputDeviceError{};
	TSharedPtr<SWindow> OwningWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
	if (OwningWindow.IsValid())
	{
		OwningWindow->SetOnWindowClosed(FOnWindowClosed::CreateSP(this, &SPaperLevelMenu::OnWindowClosed));
	}
	FEditorDelegates::MapChange.AddRaw(this, &SPaperLevelMenu::OnLevelChange);
	
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
		GrandOverlay->AsShared()
	];
}

void SPaperLevelMenu::OnWindowClosed(const TSharedRef<SWindow>& WindowBeingClosed)
{
	if (WindowBeingClosed == FSlateApplication::Get().GetActiveTopLevelWindow())
	{
		UnlockAllMips();
		
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

		SymbolPositionData.Empty();
	}
}


void SPaperLevelMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	CheckForActorsToSpawn();
	HandleTexturePainting();
	SetPreviewSymbolImageVisiblity();
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

void SPaperLevelMenu::InitMouseCallbacks()
{
	GrandOverlay->SetOnMouseButtonDown(FPointerEventHandler::CreateRaw(this, &SPaperLevelMenu::OnImageMouseButtonDown));
	GrandOverlay->SetOnMouseButtonUp(FPointerEventHandler::CreateRaw(this, &SPaperLevelMenu::OnImageMouseButtonUp));
	ParentBox->SetOnMouseButtonDown(FPointerEventHandler::CreateRaw(this, &SPaperLevelMenu::OnImageMouseButtonDown));
	ParentBox->SetOnMouseButtonUp(FPointerEventHandler::CreateRaw(this, &SPaperLevelMenu::OnImageMouseButtonUp));
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
	if (IsStamping)
	{
		HandleSymbolStamping();
		return;
	}
	
	if (TextureGeo && IsTrackingMousePos && IsDrawingWorldBoundary)
	{
		auto localMousePosition = TextureGeo->AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
		
		if (MapImageTexture && MapImageTexture->GetPlatformData())
		{
			if (!MapImageTexture->GetPlatformData()->Mips[0].BulkData.IsLocked())
			{
				if (auto mipData = static_cast<RGBA*>(MapImageTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE)))
				{
					int roundedPosY = FMath::RoundToInt(localMousePosition.Y);
					int roundedPosX = FMath::RoundToInt(localMousePosition.X);
					int index = roundedPosY * MapImageTexture->GetPlatformData()->SizeX + roundedPosX;
					if (index >= 0 && index < SymbolPositionData.Num())
					{
						SymbolPositionData[index] = 20;
							
						EditTexel( MapImageTexture,
							{roundedPosX,roundedPosY},
							mipData,
							{0,0,0,1},
							MapImageBrush,
							TEXTUREACTION::UPDATE,
							3);
					}
				}
				UnlockAllMips();
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
			.DesiredSizeOverride(FVector2D(500.0f, 500.0f))
			.ColorAndOpacity(FLinearColor::White);

	MapImageToPaint = &image.Get();

	TextureGeo = &MapImageToPaint->GetTickSpaceGeometry();
	
	// wrap image in a border
	return SNew(SBorder)
		.Padding(10)
		.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(Statics::GetInnerBackgroundColor())
		.DesiredSizeScale(FVector2D(1.0f, 1.0f))
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

	for(int i = 0; i < 500*500; i++)
		SymbolPositionData.Add(10);
	
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
	SymbolsGrid.Add({0,0}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnSymbolClicked, 0, SymbolTextures[0]));
	SymbolsGrid.Add({1,0}, Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::OnSymbolClicked, 1, SymbolTextures[8]));
	
}

void SPaperLevelMenu::OnMapNameChanged(const FText& _newText)
{
	ToBeMapName = _newText.ToString();
}

void SPaperLevelMenu::OnSymbolClicked(int _symbol)
{
	if (_symbol < 1)
	{
		IsStamping = true;
		IsDrawingWorldBoundary = false;

		CurrentlySelectedSymbol = _symbol;
		
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
	else
	{
		OpenAssetPicker();
	}
	UnlockAllMips();
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

void SPaperLevelMenu::OnSpawnObjectsInLevelClicked()
{
	if (HasSymbolsBeenPlaced())
	{
		for(int y = 0; y < MapImageTexture->GetPlatformData()->SizeY; y++)
		{
			for(int x = 0; x < MapImageTexture->GetPlatformData()->SizeX; x++)
			{
				SpawnObjectInWorldFromSymbolID(x, y);
			}
		}
		Statics::DisplayNotification("Spawned New Object In World");
	}
	else
	{
		Statics::DisplayNotification("Failed Spawned New Object In World", false);
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

	for(auto actor : SpawnedActors)
	{
		if (actor)
		{
			actor->Destroy();
			actor = nullptr;
		}
	}
	SpawnedActors.Empty();
	for(auto actor : ActorsToSpawn)
	{
		if (actor)
		{
			actor->Destroy();
			actor = nullptr;
		}
	}
	ActorsToSpawn.Empty();

	for(auto actor : SpawnedWBActors)
	{
		if (actor)
		{
			actor->Destroy();
			actor = nullptr;
		}
	}
	SpawnedWBActors.Empty();
	for(auto actor : WBActorsToSpawn)
	{
		if (actor)
		{
			actor->Destroy();
			actor = nullptr;
		}
	}
	WBActorsToSpawn.Empty();

	SymbolPositionData.Empty();
	for(int i = 0; i < MapImageTexture->GetPlatformData()->SizeX * MapImageTexture->GetPlatformData()->SizeY; i++)
		SymbolPositionData.Add(10);
	
	UnlockAllMips();

	GEditor->SelectNone(false, true);
	
	Statics::DisplayNotification("Paper Map Cleared");
}

void SPaperLevelMenu::OnEraserClicked()
{
	//LastAction = &SPaperLevelMenu::OnEraserClicked;
}

void SPaperLevelMenu::OnUndoClicked()
{
	//ImprintCopyMapOntoCurrent();
}

void SPaperLevelMenu::OnRedoClicked()
{
	//LastAction = &SPaperLevelMenu::OnRedoClicked;
}

void SPaperLevelMenu::OnLevelChange(unsigned)
{
	OnClearClicked();
}

FSlateImageBrush* SPaperLevelMenu::CreateImageBrushFromStyleSet(FName ImageName, const FVector2D& ImageSize,
                                                                const FName& StyleSetName, UTexture2D* Texture, FSlateImageBrush* ImageBrush)
{
	if (!Texture)
		return nullptr;
	
	// Set the texture settings and properties
	Texture->CompressionSettings = TC_Default;
	Texture->MipGenSettings = TMGS_SimpleAverage;
	Texture->AddToRoot();
	
	if (Texture->GetPlatformData()->Mips.Num() == 0)
	{
		Texture->GetPlatformData()->Mips.Add(new FTexture2DMipMap{Texture->GetPlatformData()->SizeX, Texture->GetPlatformData()->SizeY});
	}
	
	Texture->UpdateResource();
	
	ImageBrush = new FSlateImageBrush(Texture, ImageSize);

	// Set the texture to white
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
			
			CopyCurrentSymbolOntoMap();
		}
		else
		{
			IsTrackingMousePos = true;
		}

		UnlockAllMips();
		
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply SPaperLevelMenu::OnImageMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		SpawnFreshlyDrawnWorldBoundary();
		
		IsTrackingMousePos = false;
		IsStamping = false;
		
		UnlockAllMips();

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
	if (_texture)
	{
		if (_texture->GetPlatformData())
		{
			if (_texture->GetPlatformData()->Mips.Num() > 0)
			{
				if (!_texture->GetPlatformData()->Mips[0].BulkData.IsLocked())
				{
					return _texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				}
			}
		}
	}

	return nullptr;
}

void SPaperLevelMenu::UnlockMipsBulkData_Safe(UTexture2D* _texture)
{
	if (_texture)
	{
		if (_texture->GetPlatformData())
		{
			if (_texture->GetPlatformData()->Mips.Num() > 0)
			{
				if (_texture->GetPlatformData()->Mips[0].BulkData.IsLocked())
				{
					_texture->GetPlatformData()->Mips[0].BulkData.Unlock();
				}
			}
		}
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
		
		{
			auto horizonBox = SNew(SHorizontalBox);
			horizonBox->AddSlot()
				.HAlign(HAlign_Center)
				.AutoWidth()
				[
					SymbolsGrid[{0,0}]
				];
			horizonBox->AddSlot()
				.HAlign(HAlign_Center)
				.AutoWidth()
				[
					SymbolsGrid[{1,0}]
				];
			
		
			symblsVertBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				horizonBox
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

		if (SymbolTextures.Num() >= 7)
		{
			auto horizonBox = SNew(SHorizontalBox);
			horizonBox->AddSlot()
			.HAlign(HAlign_Center)
			.FillWidth(1)
			[
				Statics::CreateButton<SPaperLevelMenu>(this, "", &SPaperLevelMenu::ToggleWorldBoundaryFreedraw, SymbolTextures[7]) // Free draw
			];
		
			WorldBoundVertBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				horizonBox
			];
		}
	}

	return WorldBoundVertBox;
}

TSharedRef<SVerticalBox> SPaperLevelMenu::CreateGnrlVertBox()
{
	auto GeneralBoundVertBox = SNew(SVerticalBox);
	
	GeneralBoundVertBox->AddSlot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			Statics::CreateNewTitle("General")
		];
		
	auto horizonBox = SNew(SHorizontalBox);
	horizonBox->AddSlot()
	.HAlign(HAlign_Fill)
	[
		Statics::CreateButton<SPaperLevelMenu>(this, "Focus", &SPaperLevelMenu::FocusMapMiddle)
	];
	horizonBox->AddSlot()
	.HAlign(HAlign_Fill)
	[
		Statics::CreateButton<SPaperLevelMenu>(this, "Clear", &SPaperLevelMenu::OnClearClicked)
	];
	GeneralBoundVertBox->AddSlot()
	.AutoHeight()
	[
		Statics::CreateVectorInputField2D<SPaperLevelMenu>(this, "Size", &SPaperLevelMenu::GetWorldSizeX, &SPaperLevelMenu::GetWorldSizeY, &SPaperLevelMenu::OnWorldSizeXChanged, &SPaperLevelMenu::OnWorldSizeYChanged, &SPaperLevelMenu::OnResetWorldSize)
	];
	GeneralBoundVertBox->AddSlot()
	.HAlign(HAlign_Center)
	.AutoHeight()
	[
		horizonBox
	];

	return GeneralBoundVertBox;
}

void SPaperLevelMenu::CreateSymbolsTextures()
{
	for(int i = 0; i < 9; i++)
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
		case 7:
			{
				fileName = "Pencil";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Pencil.png"));
				break;
			}
		case 8:
			{
				fileName = "Custom";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Custom.png"));
				break;
			}
		default:
			{
				fileName = "Icon128";
				texturePath = FPaths::Combine(*pluginDir, TEXT("Icon128.png"));
				break;
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

	UnlockAllMips();
}

void SPaperLevelMenu::ImprintCurrentMapOntoCopy()
{
	if (MapImageTexture && LastMapImageTexture)
	{
		if (MapImageTexture->GetPlatformData()->Mips.Num() > 0)
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

	UnlockAllMips();
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

	UnlockAllMips();
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

void SPaperLevelMenu::CopyCurrentSymbolOntoMap()
{
	auto localMousePosition = TextureGeo->AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());

	if (localMousePosition.Y >=  MapImageTexture->GetPlatformData()->SizeY
		|| localMousePosition.X >= MapImageTexture->GetPlatformData()->SizeX
		|| localMousePosition.Y < 0 || localMousePosition.X < 0)
	{
		UnlockAllMips();
		return;
	}

	/*ImprintCurrentMapOntoCopy();*/
	// Mips
	FTexture2DMipMap& Mip = PreviewSymbolTexture->GetPlatformData()->Mips[0];
	FTexture2DMipMap& Mip2 = MapImageTexture->GetPlatformData()->Mips[0];
	if (!Mip.BulkData.IsLocked() && !Mip2.BulkData.IsLocked())
	{
		auto Data = static_cast<RGBA*>(Mip.BulkData.Lock(LOCK_READ_ONLY));
		auto Data2 = static_cast<RGBA*>(Mip2.BulkData.Lock(LOCK_READ_WRITE));
		int32 index{};
		int32 previewSymbolIndex{};
		for(int32 i = (int32)((int32)localMousePosition.Y - (int32)(PreviewSymbolTexture->GetPlatformData()->SizeY/2)); i < (int32)((int32)localMousePosition.Y + (int32)(PreviewSymbolTexture->GetPlatformData()->SizeY/2)); i++)
		{
			for(int32 j = (int32)((int32)localMousePosition.X - (int32)(PreviewSymbolTexture->GetPlatformData()->SizeX/2)); j < (int32)((int32)localMousePosition.X + (int32)(PreviewSymbolTexture->GetPlatformData()->SizeX/2)); j++)
			{
				previewSymbolIndex++;

				if (j <  MapImageTexture->GetPlatformData()->SizeX && j >= 0 && i <  MapImageTexture->GetPlatformData()->SizeY && i >= 0)
				{
					index = i * MapImageTexture->GetPlatformData()->SizeX + j;
					if (index >= 0 && index < MapImageTexture->GetPlatformData()->SizeX * MapImageTexture->GetPlatformData()->SizeY
						&& index <= i * MapImageTexture->GetPlatformData()->SizeX + (MapImageTexture->GetPlatformData()->SizeX - 1)
						&& index >= i * MapImageTexture->GetPlatformData()->SizeX)
					{
						//UE_LOG(LogTemp, Warning, TEXT("Index Clicked: %s"), *FString::FromInt(index) )
						if (Data[previewSymbolIndex].A < 255)
						{
							Data2[index].A = Data2[index].A;
							Data2[index].G = Data2[index].G;
							Data2[index].B = Data2[index].B;
							Data2[index].R = Data2[index].R;
						}
						else
						{
							Data2[index].A = 255;
							Data2[index].G = Data[previewSymbolIndex].G;
							Data2[index].B = Data[previewSymbolIndex].B;
							Data2[index].R = Data[previewSymbolIndex].R;
						}
					}
				}
			}
		}
		index =  ((int32)(localMousePosition.Y)) * MapImageTexture->GetPlatformData()->SizeX + ((int32)localMousePosition.X);
		index = FMath::Clamp(index, 0, (MapImageTexture->GetPlatformData()->SizeY * MapImageTexture->GetPlatformData()->SizeX) - 1);
		SymbolPositionData[index] = CurrentlySelectedSymbol;
		
		//UE_LOG(LogTemp, Warning, TEXT("Placed symbol %s onto map at position %s %s"), *FString::FromInt(CurrentlySelectedSymbol), *FString::FromInt(localMousePosition.X), *FString::FromInt(localMousePosition.Y));
		Mip.BulkData.Unlock();
		Mip2.BulkData.Unlock();
		MapImageTexture->UpdateResource();
		MapImageBrush->SetResourceObject(MapImageTexture);
	}

	UnlockAllMips();

	if (CurrentlySelectedSymbol >= 1)
	{
		PlaceAndSpawnCustomSymbol(localMousePosition);
	}
	else
	{
		SpawnObjectInWorldFromSymbolID(localMousePosition.X, localMousePosition.Y);
	}
}

void SPaperLevelMenu::SpawnObjectInWorldFromSymbolID(int _x, int _y)
{
	int arraySizeX {MapImageTexture->GetPlatformData()->SizeX};
	int arraySizeY {MapImageTexture->GetPlatformData()->SizeY};
	FVector spawnPos = {(float)_x, (float)_y, 0.0f};
	spawnPos.X -= MapImageTexture->GetPlatformData()->SizeX / 2;
	spawnPos.Y -= MapImageTexture->GetPlatformData()->SizeY / 2;
	
	if (spawnPos.X < -arraySizeX / 2
		|| spawnPos.X > arraySizeX / 2
		|| spawnPos.Y < -arraySizeY / 2
		|| spawnPos.Y > arraySizeY / 2)
	{
		return;
	}

	spawnPos.X *= ((float)((int32)(DesiredMapSize.X))) / ((float)((int32)(arraySizeX)));
	spawnPos.Y *= ((float)((int32)(DesiredMapSize.Y))) / ((float)((int32)(arraySizeY)));
	
	switch(SymbolPositionData[_y * arraySizeX + _x])
	{
	case 0:
		{
			FString newName{"New Player Start" };
			auto currentLevel = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();
			auto newActor = GEditor->AddActor(currentLevel, APlayerStart::StaticClass(), FTransform{spawnPos + (FVector::UpVector * 100)});
			
			newActor->SetActorLabel(newName);
			ActorsToSpawn.Add(newActor);
			Statics::DisplayNotification("Spawned New Player Start In World");
			return;
			break;
		}
	default:
		break;
	}

	Statics::DisplayNotification("Failed To Spawn New Object In World", false);
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
	
	DestData.Empty(DestWidth * DestHeight * 4);
	DestData.AddZeroed(DestWidth * DestHeight * 4);

	// Calculate the scaling factors for resizing
	float ScaleX = static_cast<float>(DestWidth) / static_cast<float>(SourceWidth);
	float ScaleY = static_cast<float>(DestHeight) / static_cast<float>(SourceHeight);

	// Iterate over each destination pixel and sample the corresponding source pixel
	for (int32 Y = 0; Y < DestHeight; Y++)
	{
		for (int32 X = 0; X < DestWidth; X++)
		{
			// Calculate the index of the source pixel in the source data array
			int32 SourceIndex = (((int32)(Y / ScaleY)) * SourceWidth + ((int32)(X / ScaleX))) * 4;

			// Calculate the index of the destination pixel in the destination data array
			int32 DestIndex = (Y * DestWidth + X) * 4;

			// Copy the RGBA values from the source to the destination
			DestData[DestIndex + 0] = SourceData[SourceIndex + 0]; // Red
			DestData[DestIndex + 1] = SourceData[SourceIndex + 1]; // Green
			DestData[DestIndex + 2] = SourceData[SourceIndex + 2]; // Blue
			DestData[DestIndex + 3] = SourceData[SourceIndex + 3]; // Alpha
		}
	}

	return true;
}

void SPaperLevelMenu::UnlockAllMips()
{
	if (MapImageTexture)
		UnlockMipsBulkData_Safe(MapImageTexture);
	if (PreviewSymbolTexture)
		UnlockMipsBulkData_Safe(PreviewSymbolTexture);
	if (LastMapImageTexture)
		UnlockMipsBulkData_Safe(LastMapImageTexture);
	if (SymbolTextures.Num() > 0)
	{
		for(auto texture : SymbolTextures)
		{
			if (texture)
			{
				UnlockMipsBulkData_Safe(texture);
			}
		}
	}
	
}

bool SPaperLevelMenu::HasSymbolsBeenPlaced()
{
	bool hasBeenPlaced{};
	for (int32 y = 0; y < MapImageTexture->GetPlatformData()->SizeY; y++)
	{
		for (int32 x = 0; x < MapImageTexture->GetPlatformData()->SizeX; x++)
		{
			int index = y * MapImageTexture->GetPlatformData()->SizeX + x;
			if (SymbolPositionData[index] < 10)
			{
				return true;
			}
		}
	}
	return hasBeenPlaced;
}

bool SPaperLevelMenu::HasWorldBoundaryBeenDrawn()
{
	bool hasBeenPlaced{};
	for (int32 y = 0; y < MapImageTexture->GetPlatformData()->SizeY; y++)
	{
		for (int32 x = 0; x < MapImageTexture->GetPlatformData()->SizeX; x++)
		{
			int index = y * MapImageTexture->GetPlatformData()->SizeX + x;
			if (SymbolPositionData[index] == 20)
			{
				return true;
			}
		}
	}
	return hasBeenPlaced;
}

UObject* SPaperLevelMenu::LoadInSymbolBlueprints(FString _name)
{
	FString path = "/Game/" + _name;
	if (UObject* theBlueprint = LoadObject<UObject>(nullptr, *path))
	{
		LoadedSymbolBlueprints.Add(theBlueprint);
		
		return theBlueprint;
	}
	
	return nullptr;
}

void SPaperLevelMenu::AddNewSymbolFromBlueprint(FString _blueprintPath)
{
	
}

void SPaperLevelMenu::OpenAssetPicker()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	// Create a struct to store the selected asset
	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateLambda([this](const FAssetData& AssetData) {
		LastSelectedAsset = AssetData;
		IsStamping = true;
		IsDrawingWorldBoundary = false;
		CurrentlySelectedSymbol = 3;
		FTexture2DMipMap& Mip = PreviewSymbolTexture->GetPlatformData()->Mips[0];
		FTexture2DMipMap& Mip2 = SymbolTextures[3]->GetPlatformData()->Mips[0];
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
		TSharedPtr<SWindow> AssetPickerWindow = FSlateApplication::Get().FindWidgetWindow(AssetPickerWidget->AsShared());
		if (AssetPickerWindow.IsValid())
		{
			FSlateApplication::Get().RequestDestroyWindow(AssetPickerWindow.ToSharedRef());
		}
	});
	AssetPickerConfig.bAllowNullSelection = true; // Set to true if you want to allow selecting no asset
	AssetPickerConfig.bAllowDragging = true;
	AssetPickerConfig.Filter.bRecursiveClasses = true;
	AssetPickerConfig.Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
	
	// Open the asset picker in a modal dialog window
	TSharedRef<SWidget> assetPickerWidget = ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig);
	AssetPickerWidget = &assetPickerWidget.Get();
	
	// Calculate the size of the modal dialog window based on the viewport size
	FVector2D ViewportSize = {500,500};

	// Create a modal dialog window.
	TSharedRef<SWindow> ModalWindow = SNew(SWindow)
		.Title(FText::FromString("Select an Asset"))
		.SizingRule(ESizingRule::FixedSize) // Use FixedSize to prevent stretching
		.ClientSize(ViewportSize) // Set the size of the modal dialog window
		.AutoCenter(EAutoCenter::PreferredWorkArea)
		.SupportsMaximize(false)
		.SupportsMinimize(false);
	
	// Set the content of the modal window
	ModalWindow->SetContent(assetPickerWidget);

	// Add the modal window to the slate application
	FSlateApplication::Get().AddWindow(ModalWindow);

	// debug log
	UE_LOG(LogTemp, Warning, TEXT("Try Open Asset Picker"));
}

void SPaperLevelMenu::PlaceAndSpawnCustomSymbol(FVector2D _mousePos)
{
	int arraySizeX {MapImageTexture->GetPlatformData()->SizeX};
	int arraySizeY {MapImageTexture->GetPlatformData()->SizeY};
	FVector spawnPos = {(float)((int32)(_mousePos.X)), (float)((int32)(_mousePos.Y)), 0.0f};
	spawnPos.X -= MapImageTexture->GetPlatformData()->SizeX / 2;
	spawnPos.Y -= MapImageTexture->GetPlatformData()->SizeY / 2;
	if (spawnPos.X > -arraySizeX / 2
		&& spawnPos.X < arraySizeX / 2
		&& spawnPos.Y > -arraySizeY / 2
		&& spawnPos.Y < arraySizeY / 2)
	{
		spawnPos.X *= ((float)((int32)(DesiredMapSize.X))) / ((float)((int32)(arraySizeX)));
		spawnPos.Y *= ((float)((int32)(DesiredMapSize.Y))) / ((float)((int32)(arraySizeY)));
		
		FString newName{"New " };
		
		//newName += " X:";
		//newName += FString::FromInt(spawnPos.X);
		//newName += " Y:" + FString::FromInt(spawnPos.Y);

		UE_LOG(LogTemp, Warning, TEXT("New Symbol Desired"));
		UClass* assetClass = nullptr;
		// Get the loaded asset object
		UObject* assetObject = LastSelectedAsset.GetAsset();
		// Check if the asset object is a Blueprint
		if (assetObject && assetObject->IsA<UBlueprint>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Got asset"));
			// Cast the asset object to UBlueprint
			UBlueprint* BlueprintAsset = Cast<UBlueprint>(assetObject);
			if (BlueprintAsset)
			{
				// Get the generated class from the Blueprint
				assetClass = BlueprintAsset->GeneratedClass;
				newName += BlueprintAsset->GetName();
			}
		}
    
		if (assetClass)
		{
			ULevel* currentLevel = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();

			// Create an instance of the asset class and add it to the level
			auto newActor = GEditor->AddActor(currentLevel, assetClass, FTransform{spawnPos});
			if (currentLevel && newActor)
			{
				newActor->SetActorLabel(newName);
				newActor->FinishSpawning(FTransform{spawnPos});
				currentLevel->Actors.Add(newActor);
				UE_LOG(LogTemp, Warning, TEXT("New Asset Spawned!"));
							
				ActorsToSpawn.Add(newActor);
				FString displayNotificationText{"Spawned "};
				displayNotificationText += newName;
				displayNotificationText += " In World";
				Statics::DisplayNotification(displayNotificationText);
				return;
			}
		}

		Statics::DisplayNotification("Failed Spawned New Object In World", false);
	}
}

void SPaperLevelMenu::ToggleWorldBoundaryFreedraw()
{
	IsDrawingWorldBoundary = !IsDrawingWorldBoundary;
	IsStamping = false;
}

void SPaperLevelMenu::SpawnFreshlyDrawnWorldBoundary()
{
	if (IsDrawingWorldBoundary)
	{
		IsDrawingWorldBoundary = false;
	
		if (HasWorldBoundaryBeenDrawn())
		{
			for(auto actor : SpawnedWBActors)
			{
				actor->Destroy(); 
				actor = nullptr;
			}
			SpawnedWBActors.Empty();
		
			for(int y = 0; y < MapImageTexture->GetPlatformData()->SizeY; y++)
			{
				for(int x = 0; x < MapImageTexture->GetPlatformData()->SizeX; x++)
				{
					int index = y * MapImageTexture->GetPlatformData()->SizeX + x;
					if (SymbolPositionData[index] == 20)
					{
						SpawnColliderInWorldFromPosition(x, y);
					}
				}
			}

			Statics::DisplayNotification("Created New World Boundary Successfully");
		}
		else
		{
			Statics::DisplayNotification("Failed Create New World Boundary", false);
		}
	}
}

void SPaperLevelMenu::SpawnColliderInWorldFromPosition(int _x, int _y)
{
	int arraySizeX {MapImageTexture->GetPlatformData()->SizeX};
	int arraySizeY {MapImageTexture->GetPlatformData()->SizeY};
	FVector spawnPos = {(float)_x, (float)_y, 0.0f};
	spawnPos.X -= MapImageTexture->GetPlatformData()->SizeX / 2;
	spawnPos.Y -= MapImageTexture->GetPlatformData()->SizeY / 2;

	if (spawnPos.X < -arraySizeX / 2
	|| spawnPos.X > arraySizeX / 2
	|| spawnPos.Y < -arraySizeY / 2
	|| spawnPos.Y > arraySizeY / 2)
	{
		return;
	}
	
	spawnPos.X *= ((float)((int32)(DesiredMapSize.X))) / ((float)((int32)(arraySizeX)));
	spawnPos.Y *= ((float)((int32)(DesiredMapSize.Y))) / ((float)((int32)(arraySizeY)));

	auto currentLevel = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();
	AActor* SpawnedActor = GEditor->AddActor(currentLevel, AActor::StaticClass(), FTransform(spawnPos));
	UBoxComponent* BoxComponent = NewObject<UBoxComponent>(SpawnedActor);
	BoxComponent->RegisterComponent();
	BoxComponent->SetBoxExtent({((float)((int32)(DesiredMapSize.X))) / ((float)((int32)(arraySizeX))),((float)((int32)(DesiredMapSize.Y))) / ((float)((int32)(arraySizeY))),WorldBoundaryZExtent});
	BoxComponent->SetWorldLocation(spawnPos + (FVector::UpVector * WorldBoundaryZExtent));
	SpawnedActor->SetRootComponent(BoxComponent);
	SpawnedActor->SetActorLabel("World-Boundary Collider");
	WBActorsToSpawn.Add(SpawnedActor);
}

void SPaperLevelMenu::OnWorldSizeXChanged(double _newValue)
{
	if (_newValue > 0)
	{
		DesiredMapSize.X = _newValue;
	}
}

void SPaperLevelMenu::OnWorldSizeYChanged(double _newValue)
{
	if (_newValue > 0)
	{
		DesiredMapSize.Y = _newValue;
	}
}

TOptional<double> SPaperLevelMenu::GetWorldSizeX() const
{
	return DesiredMapSize.X;
}

TOptional<double> SPaperLevelMenu::GetWorldSizeY() const
{
	return DesiredMapSize.Y;
}

void SPaperLevelMenu::OnResetWorldSize()
{
	DesiredMapSize = {1000,1000};
}

void SPaperLevelMenu::FocusMapMiddle()
{
	if (GEditor && GEditor->GetActiveViewport())
	{
		FEditorViewportClient* ViewportClient = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
		if (ViewportClient)
		{
			ViewportClient->SetViewMode(VMI_Unlit);
			ViewportClient->SetViewportType(LVT_OrthoXY);
			ViewportClient->SetOrthoZoom(10000);
			ViewportClient->SetViewLocation({0,0,0});
			ViewportClient->Invalidate();
		}
	}
}

void SPaperLevelMenu::CheckForActorsToSpawn()
{
	if (SpawnedActors.Num() <= 0)
	{
		for(int i = 0; i < ActorsToSpawn.Num(); i++)
		{
			SpawnedActors.Add(ActorsToSpawn[i]);
		}
		ActorsToSpawn.Empty();
	}

	if (SpawnedWBActors.Num() <= 0)
	{
		for(int i = 0; i < WBActorsToSpawn.Num(); i++)
		{
			SpawnedWBActors.Add(WBActorsToSpawn[i]);
		}
		WBActorsToSpawn.Empty();
	}
}

void SPaperLevelMenu::CreateParentWidgets()
{
	auto grandOverlay = SNew(SOverlay);
	GrandOverlay = &grandOverlay.Get();
	auto parent = SNew(SVerticalBox);
	ParentBox = &parent.Get();
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
