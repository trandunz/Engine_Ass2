// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class PAPER_LEVEL_API SPaperLevelMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPaperLevelMenu)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	/**
 	* @brief Create The Slate Designer Menu
 	*/
	void InitPaperLevelMenu();

	TSharedRef<SBorder> InitPaperLevelMenuContents();

	void CreatePaperMapGrid();
	void CreateSymbolsGrid();
	
	void OnButtonClicked();
	void OnMapNameChanged(const FText&);

	void OnEraserClicked();
	void OnUndoClicked();
	void OnRedoClicked();

private:
	FString ToBeMapName{"Default Map Name"};
	
	SVerticalBox* ParentBox{nullptr};
	TMap<FUintVector2, TSharedRef<SBorder>> WorldGrid{};
	TMap<FUintVector2, TSharedRef<SBorder>> SymbolsGrid{};
};
