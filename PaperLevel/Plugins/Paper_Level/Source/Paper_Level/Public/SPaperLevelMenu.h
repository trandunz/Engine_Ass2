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

	/**
 	* @brief Create The Slate Designer Menu
 	*/
	void InitSlateDesignerMenu();

private:
	SVerticalBox* ParentBox{nullptr};
	UE::Math::TMatrix<SBorder*> WorldGrid{};
};
