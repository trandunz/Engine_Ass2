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
	uint8 B{};
	uint8 G{};
	uint8 R{};
	uint8 A{};
};

class FCustomOutputDeviceError : public FOutputDeviceError
{
public:
	FString LastErrorMessage;

	virtual void Serialize(const TCHAR* Msg, ELogVerbosity::Type Verbosity, const class FName& Category) override
	{
		LastErrorMessage = Msg;
		UE_LOG(LogTemp, Warning, TEXT("GError: %s"), *LastErrorMessage);
	}

	virtual void HandleError() override
	{
		UE_LOG(LogTemp, Warning, TEXT("GError: %s"), *LastErrorMessage);
	}
};

class PAPER_LEVEL_API SPaperLevelMenu : public SCompoundWidget
{
	
public:
	SLATE_BEGIN_ARGS(SPaperLevelMenu)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual void OnWindowClosed(const TSharedRef<SWindow>& WindowBeingClosed);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
	void InitPaperLevelMenu();
	void InitMouseCallbacks();

	TSharedRef<SBorder> InitPaperLevelMenuContents();

	void HandleTexturePainting();
	void HandleSymbolStamping();
	
	TSharedRef<SBorder> CreateMapImage();
	TSharedRef<SImage> CreatePreviewSymbolImage();
	void CreatePaperMapGrid();
	void CreateSymbolsGrid();
	
	void OnMapNameChanged(const FText&);
	
	void OnSymbolClicked(int _symbol);

	void OnCreateLevelClicked();
	void OnRenameLevelClicked();
	void OnSpawnObjectsInLevelClicked();
	void OnClearClicked();
	void OnEraserClicked();
	void OnUndoClicked();
	void OnRedoClicked();

	void OnLevelChange(unsigned);

	struct FSlateImageBrush* CreateImageBrushFromStyleSet(FName ImageName, const FVector2D& ImageSize, const FName& StyleSetName, UTexture2D* Texture, FSlateImageBrush* ImageBrush);
	void CreateDuplicateMapTextureForUndo(const FVector2D& ImageSize);
	
	FReply OnImageMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	FReply OnImageMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	void EditTexel(UTexture2D* _texture, FIntVector2 _texel, RGBA* _mipData, RGBA _newColor, FSlateImageBrush* _ImageBrush, TEXTUREACTION _postUpdateAction = TEXTUREACTION::NON, int _radius = 1);

	void* LockMipsBulkData_Safe(UTexture2D* _texture);
	void UnlockMipsBulkData_Safe(UTexture2D* _texture);

	TSharedRef<SVerticalBox> CreateControlsVertBox();
	TSharedRef<SVerticalBox> CreateSymblsVertBox();
	TSharedRef<SVerticalBox> CreateWrldBndsVertBox();
	TSharedRef<SVerticalBox> CreateGnrlVertBox();

	void CreateSymbolsTextures();
	
	void ImprintCurrentMapOntoCopy();
	void ImprintCopyMapOntoCurrent();

	void CreateAndAddLevelAsset();

	void SetPreviewSymbolImageVisiblity();

	void CopyCurrentSymbolOntoMap();

	void SpawnObjectInWorldFromSymbolID(int _x, int _y);

	bool ResizeImageData(const TArray<uint8>& SourceData, int32 SourceWidth, int32 SourceHeight,
	int32 DestWidth, int32 DestHeight, TArray<uint8>& DestData);

	void UnlockAllMips();

	bool HasSymbolsBeenPlaced();
	bool HasWorldBoundaryBeenDrawn();

	UObject* LoadInSymbolBlueprints(FString _name);

	void AddNewSymbolFromBlueprint(FString _blueprintPath);

	void OpenAssetPicker();

	void PlaceAndSpawnCustomSymbol(FVector2D _mousePos);

	void ToggleWorldBoundaryFreedraw();

	void SpawnFreshlyDrawnWorldBoundary();

	void SpawnColliderInWorldFromPosition(int _x, int _y);

	void OnWorldSizeXChanged(double _newValue);
	void OnWorldSizeYChanged(double _newValue);
	TOptional<double> GetWorldSizeX() const;
	TOptional<double> GetWorldSizeY() const;
	void OnResetWorldSize();

	void FocusMapMiddle();

	void CheckForActorsToSpawn();

	void CreateParentWidgets();
private:
	float WorldBoundaryZExtent{200};
	
	FIntVector2 DesiredMapSize{1000, 1000};
	
	TArray<UObject*> LoadedSymbolBlueprints;
	
	FVector2D SymbolTextureSize{80,80};
	FVector2D PreviewSymbolTextureSize{80,80};
	
	FCustomOutputDeviceError* CustomErrorDevice;
	
	FString ToBeMapName{"Map Name..."};

	bool IsStamping{false};
	
	bool IsTrackingMousePos{};

	bool IsDrawingWorldBoundary{};
	
	SImage* MapImageToPaint{nullptr};
	const struct FGeometry* TextureGeo{};
	struct FSlateImageBrush* MapImageBrush{};
	UTexture2D* MapImageTexture{nullptr};

	TArray<AActor*> SpawnedActors{};
	TArray<AActor*> ActorsToSpawn{};

	TArray<AActor*> SpawnedWBActors{};
	TArray<AActor*> WBActorsToSpawn{};
	
	TArray<uint8> SymbolPositionData {};
	uint8 CurrentlySelectedSymbol{};
	SImage* PreviewSymbolImage{nullptr};
	const struct FGeometry* PreviewSymbolTextureGeo{};
	struct FSlateImageBrush* PreviewSymbolImageBrush{};
	UTexture2D* PreviewSymbolTexture{nullptr};
	
	UTexture2D* LastMapImageTexture{nullptr};
	
	TArray<UTexture2D*> SymbolTextures{};
	
	SVerticalBox* ParentBox{nullptr};
	SOverlay* SymbolPreviewOverlay{nullptr};
	SOverlay* GrandOverlay{nullptr};
	
	TMap<FUintVector2, TSharedRef<SBorder>> WorldGrid{};
	
	TMap<FUintVector2, TSharedRef<SBorder>> SymbolsGrid{};
	TArray<SButton*> SymbolsButtons{};
	
	SWidget* AssetPickerWidget{nullptr};
	FAssetData LastSelectedAsset;
};


