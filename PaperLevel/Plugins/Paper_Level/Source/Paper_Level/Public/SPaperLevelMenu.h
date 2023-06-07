// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

enum class TEXTUREACTION
{
	NON,
	UPDATE
};

struct RGBA
{
	FFloat32 A{};
	FFloat32 G{};
	FFloat32 B{};
	FFloat32 R{};
};

class PAPER_LEVEL_API SPaperLevelMenu : public SCompoundWidget
{
	
public:
	SLATE_BEGIN_ARGS(SPaperLevelMenu)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
	void InitPaperLevelMenu();

	TSharedRef<SBorder> InitPaperLevelMenuContents();

	void HandleTexturePainting();
	
	TSharedRef<SBorder> CreateMapImage();
	void CreatePaperMapGrid();
	void CreateSymbolsGrid();
	
	void OnButtonClicked();
	void OnMapNameChanged(const FText&);

	void OnSpawnSymbolClicked();
	void OnSellBinSymbolClicked();
	void OnGrowPlotSymbolClicked();

	void OnClearClicked();
	void OnEraserClicked();
	void OnUndoClicked();
	void OnRedoClicked();

	struct FSlateImageBrush* CreateImageBrushFromStyleSet(FName ImageName, const FVector2D& ImageSize, const FName& StyleSetName);

	FReply OnImageMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);;
	FReply OnImageMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);;

	void EditTexel(UTexture2D* _texture, FIntVector2 _texel, RGBA* _mipData, RGBA _newColor, TEXTUREACTION _postUpdateAction = TEXTUREACTION::NON, int _radius = 1);

	void UnlockMipsBulkData_Safe(UTexture2D* _texture);

	TSharedRef<SVerticalBox> CreateControlsVertBox();
	TSharedRef<SVerticalBox> CreateSymblsVertBox();
	TSharedRef<SVerticalBox> CreateWrldBndsVertBox();
	TSharedRef<SVerticalBox> CreateGnrlVertBox();

	void CreateSymbolsTextures();
	
	void ImprintCurrentMapOntoCopy();
	void ImprintCopyMapOntoCurrent();
	
private:
	FString ToBeMapName{"Default Map Name"};
	
	bool IsTrackingMousePos{};

	const struct FGeometry* TextureGeo{};
	SImage* MapImageToPaint{};
	struct FSlateImageBrush* MapImageBrush{};
	UTexture2D* MapImageTexture{};
	UTexture2D* LastMapImageTexture{};
	TArray<UTexture2D*> SymbolTextures{};
	uint8* ImageTextureData{};
	
	SVerticalBox* ParentBox{nullptr};
	TMap<FUintVector2, TSharedRef<SBorder>> WorldGrid{};
	TMap<FUintVector2, TSharedRef<SBorder>> SymbolsGrid{};
};


