// Bachelor of Software Engineering 
// Media Design School 
// Auckland 
// New Zealand 
// (c) Media Design School
// File Name : SPluginDesignerMenu.h 
// Description : SPluginDesignerMenu Header File
// Author : William Inman
// Mail : William.inman@mds.ac.nz

#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"

class SLATEDESIGNER_API SPluginDesignerMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPluginDesignerMenu)
	{}
	SLATE_END_ARGS()

	/**
	 * @brief Gets called when SNew(SPluginDesignerMenu)
	 * @param InArgs 
	 */
	void Construct(const FArguments& InArgs);
	
protected:
	/**
	 * @brief Create The Slate Designer Menu
	 */
	void InitSlateDesignerMenu();

	/**
	 * @brief Create the example menu (NPC)
	 */
	void InitExampleMenu();

	/**
	 * @brief Create a custom menu with the specified title
	 * @param _title
	 */
	void InitBasicMenu(FString _title);

	/**
	 * @brief Called when the 'new window' input field changes (sets windowName)
	 */
	void OnWindowNameChanged(const FText&);
	FText WindowName{};

	/**
	 * @brief Creates a new custom slate menu
	 */
	void OnCreateNewMenuClicked();
	/**
	 * @brief Deletes the last custom slate menu if available
	 */
	void OnDeleteLastMenuClicked();
	/**
	 * @brief Short cut function to open the plugin creator wizard (UN-USED)
	 */
	void OnCreatePluginClicked();
	
	SVerticalBox* ParentBox{nullptr};
	SVerticalBox* CurrentlyUsedMenus{nullptr};
	TArray<TSharedRef<SWidget>> SpawnedMenuTitles{};
	TArray<TSharedRef<SWidget>> SpawnedMenus{};
	
/**
 *  Example Menu Functions and Variables (NPC Menu)
 */
private:
	//
	// Spawn Button
	//
	void OnSpawnClicked();
	
	//
	// Name Input Field
	//
	void OnNameChanged(const FText&);
	FName GetNPCDisplayName();
	FText Name{};

	//
	// Asset Pickers
	//
	void OnMeshChanged(const FAssetData&);
	static FString GetMeshPath();
	void OnAnimBPChanged(const FAssetData&);
	static FString AnimBPPath();

	static FAssetData MeshAssetData;
	static FAssetData AnimationBPData;

	TSharedPtr<FAssetThumbnailPool> MeshThumbnailPool;
	TSharedPtr<FAssetThumbnailPool> AnimBPThumbnailPool;

	//
	// Transform
	//
	private:
	void OnPositionChanged(FVector::FReal _newValue, ETextCommit::Type _commitInfo, int _transformField, EAxisList::Type _axis, bool _bCommitted);
	TOptional<double> GetXPosition() const;
	TOptional<double> GetYPosition() const;
	TOptional<double> GetZPosition() const;
	void OnResetPosition();

	void OnRotationChanged(FVector::FReal _newValue, ETextCommit::Type _commitInfo, int _transformField, EAxisList::Type _axis, bool _bCommitted);
	TOptional<double> GetXRotation() const;
	TOptional<double> GetYRotation() const;
	TOptional<double> GetZRotation() const;
	void OnResetRotation();
	
	void OnScaleChanged(FVector::FReal _newValue, ETextCommit::Type _commitInfo, int _transformField, EAxisList::Type _axis, bool _bCommitted);
	TOptional<double> GetXScale() const;
	TOptional<double> GetYScale() const;
	TOptional<double> GetZScale() const;
	void OnResetScale();

	FTransform TransformToPlace{};
};
