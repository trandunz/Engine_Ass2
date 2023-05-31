// Bachelor of Software Engineering 
// Media Design School 
// Auckland 
// New Zealand 
// (c) Media Design School
// File Name : Statics.h 
// Description : Statics Header File
// Author : William Inman
// Mail : William.inman@mds.ac.nz

#pragma once
#include "CoreMinimal.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/SCompoundWidget.h"
#include "Templates/SharedPointer.h"
#include "AIController.h"
#include "AssetViewWidgets.h"
#include "SAssetView.h"
#include "SlateOptMacros.h"
#include "GameFramework/Character.h"
#include "Misc/Attribute.h"
#include "InputCoreTypes.h"
#include "LevelEditorViewport.h"
#include "Blueprint/UserWidget.h"
#include "Layout/Margin.h"
#include "Fonts/SlateFontInfo.h"
#include "Layout/Visibility.h"
#include "Widgets/SNullWidget.h"
#include "Editor/PropertyEditor/Private/SDetailRowIndent.h"
#include "Editor/PropertyEditor/Private/SDetailExpanderArrow.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateColor.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/SlateTypes.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Containers/UnrealString.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DetailWidgetRow.h"
#include "Editor.h"
#include "EditorMetadataOverrides.h"
#include "IDetailDragDropHandler.h"
#include "IDetailPropertyExtensionHandler.h"
#include "PropertyEditorModule.h"
#include "Editor/PropertyEditor/Private/SConstrainedBox.h"
#include "Editor/PropertyEditor/Private/SDetailCategoryTableRow.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Settings/EditorExperimentalSettings.h"
#include "Styling/StyleColors.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "PropertyEditorPermissionList.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Widgets/Input/SVectorInputBox.h"
#include "Math/Transform.h"
#include "UObject/ConstructorHelpers.h"
#include "PropertyEditorModule.h"
#include "Editor/PropertyEditor/Private/UserInterface/PropertyEditor/PropertyEditorConstants.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

class SLATEDESIGNER_API Statics : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(Statics)
	{}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);

	/**
	 * @brief Returns a border containing a textblock set up for a title
	 * @param _title 
	 * @return 
	 */
	static TSharedRef<SBorder> CreateNewTitle(FString _title);

	/**
	 * @brief Wraps the specified widget inside of a scroll box and returns a border
	 * @tparam T 
	 * @param _widget 
	 * @return 
	 */
	template<class T>
	static TSharedRef<SBorder> WrapWidgetInScrollBox(TSharedRef<T> _widget);

	/**
	 * @brief Returns a border containing a centered textblock
	 * @param _text 
	 * @param _underline 
	 * @return 
	 */
	static TSharedRef<SHorizontalBox> CreateCenteredNewText(FString _text, bool _underline = false);

	/**
	 * @brief Returns a border containing a textblock on the left and a input field on the right
	 * @tparam T 
	 * @param _this 
	 * @param _name 
	 * @param _onNameChanged 
	 * @return 
	 */
	template<class T>
	static TSharedRef<SBorder> CreateNamedInputField(T* _this, FString _name, TDelegate<void(const FText&)>::TMethodPtr<T> _onNameChanged);

	/**
	 * @brief Returns a border containing a textblock on the left and a buttton on the right
	 * @tparam T 
	 * @param _this 
	 * @param _name 
	 * @param _onNameChanged 
	 * @param _onPressed 
	 * @return 
	 */
	template<class T>
	static TSharedRef<SBorder> CreateNamedButton(T* _this, FString _name, TDelegate<void(const FText&)>::TMethodPtr<T> _onNameChanged, TDelegate<void()>::TMethodPtr<T> _onPressed);

	/**
	 * @brief Returns a border containing a textblock on the left and a asset / blueprint picker on the right
	 * @param _name 
	 * @param _filter 
	 * @param _getPathFunc 
	 * @param _onValueChanged 
	 * @param _thumbnailPool 
	 * @return 
	 */
	static TSharedRef<SBorder> CreateBlueprintPicker(FString _name, UClass* _filter,  TIdentity_T<TDelegate<FString()>::TFuncPtr<>> _getPathFunc, TDelegate<void(const FAssetData&)> _onValueChanged, TSharedPtr<FAssetThumbnailPool>& _thumbnailPool);

	/**
	 * @brief Returns a border containing a textblock on the left, a vector input (x,y,z) in the middle, and a button to reset the input field on the right
	 * @tparam T 
	 * @param _this 
	 * @param _name 
	 * @param _getXFunc 
	 * @param _getYFunc 
	 * @param _getZFunc 
	 * @param _onChanged 
	 * @param _onReset 
	 * @param _vectorType 
	 * @return 
	 */
	template<class T>
	static TSharedRef<SBorder> CreateVectorInputField(T* _this, FString _name,
		TDelegate<TOptional<double>()>::TConstMethodPtr<T> _getXFunc,
		TDelegate<TOptional<double>()>::TConstMethodPtr<T> _getYFunc,
		TDelegate<TOptional<double>()>::TConstMethodPtr<T> _getZFunc,
		TDelegate<void(double)>::TMethodPtr<T, ETextCommit::Type, int, EAxisList::Type, bool> _onChanged,
		TDelegate<void()>::TMethodPtr<T> _onReset,
		int _vectorType = 0);

	/**
	 * @brief Returns a border containing a button
	 * @tparam T 
	 * @param _this 
	 * @param _text 
	 * @param _onPressed 
	 * @return 
	 */
	template<class T>
	static TSharedRef<SBorder> CreateButton(T* _this, FString _text, TDelegate<void()>::TMethodPtr<T> _onPressed);

	/**
	 * @brief Returns a border containing an input field
	 * @tparam T 
	 * @param _this 
	 * @param _default 
	 * @param _onNameChanged 
	 * @return 
	 */
	template<class T>
	static TSharedRef<SBorder> CreateInputField(T* _this, FString _default, TDelegate<void(const FText&)>::TMethodPtr<T> _onNameChanged);

	/**
	 * @brief Displays a UE notification with the specified message
	 * @param _text 
	 * @param _success 
	 */
	static void DisplayNotification(FString _text, bool _success = true);
	
	static FSlateColor GetInnerBackgroundColor();
	static FSlateColor GetRowBackgroundColor(int32 _indentLevel, bool _isHovered);
};

inline void Statics::Construct(const FArguments& InArgs)
{
}

template<class T>
inline TSharedRef<SBorder> Statics::WrapWidgetInScrollBox(TSharedRef<T> _widget)
{
	return SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(FLinearColor(0,0,0))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SBorder)
			.Padding(5)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			[
				SNew(SScrollBox)
				+SScrollBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					_widget
				]
				]
			];
}

inline TSharedRef<SBorder> Statics::CreateNewTitle(FString _title)
{
	return SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(FLinearColor(0,0,0))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SBorder)
			.Padding(5)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(GetInnerBackgroundColor())
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Top)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(_title))
					.Margin(10)
				]
			]
		];
}

inline TSharedRef<SHorizontalBox> Statics::CreateCenteredNewText(FString _text, bool _underline)
{
	return SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(_text))
					.Margin(5)
				];
}

template<typename T>
inline TSharedRef<SBorder> Statics::CreateNamedInputField(T* _this, FString _name, TDelegate<void(const FText&)>::TMethodPtr<T> _onNameChanged)
{
	return SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(FLinearColor(0,0,0))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SBorder)
			.Padding(5)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Margin(5)
					.Text(FText::FromString(_name))
				]
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(""))
					.OnTextChanged(_this, _onNameChanged)
				]
				]
			];
}

template <typename T>
inline TSharedRef<SBorder> Statics::CreateNamedButton(T* _this, FString _name,
	TDelegate<void(const FText&)>::TMethodPtr<T> _onNameChanged, TDelegate<void()>::TMethodPtr<T> _onPressed)
{
	return SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(FLinearColor(0,0,0))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SBorder)
			.Padding(5)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString(_name))
					.OnPressed(_this, _onPressed)
				]
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(""))
					.OnTextChanged(_this, _onNameChanged)
				]
				]
			];
}

inline TSharedRef<SBorder> Statics::CreateBlueprintPicker(FString _name, UClass* _filter,
TIdentity_T<TDelegate<FString()>::TFuncPtr<>> _getPathFunc, TDelegate<void(const FAssetData&)> _onValueChanged, TSharedPtr<FAssetThumbnailPool>& _thumbnailPool)
{
	return SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(FLinearColor(0,0,0))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SBorder)
			.Padding(5)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Margin(5)
					.Text(FText::FromString(_name))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SObjectPropertyEntryBox)
						.AllowedClass(_filter)
						.EnableContentPicker(true)
						.DisplayBrowse(true)
						.DisplayThumbnail(true)
						.AllowCreate(true)
						.AllowClear(true)
						.ThumbnailPool(_thumbnailPool)
						.ObjectPath_Static(_getPathFunc)
						.DisplayUseSelected(true)
						.OnObjectChanged(_onValueChanged)
				]
				]
			];
}

template <typename T>
inline TSharedRef<SBorder> Statics::CreateVectorInputField(T* _this, FString _name,
	TDelegate<TOptional<double>()>::TConstMethodPtr<T> _getXFunc,
	TDelegate<TOptional<double>()>::TConstMethodPtr<T> _getYFunc,
	TDelegate<TOptional<double>()>::TConstMethodPtr<T> _getZFunc,
	TDelegate<void(double)>::TMethodPtr<T, ETextCommit::Type, int, EAxisList::Type, bool> _onChanged,
	TDelegate<void()>::TMethodPtr<T> _onReset, int _vectorType)
{
	return SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(FLinearColor(0,0,0))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
					.BorderBackgroundColor(GetInnerBackgroundColor())
					.Padding(5)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Margin(5)
							.Text(FText::FromString(_name))
						]
						+SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						[
						SNew(SNumericVectorInputBox<FVector::FReal>)
								.X(_this,_getXFunc)
								.Y(_this,_getYFunc)
								.Z(_this,_getZFunc)
								.bColorAxisLabels(true)
								.OnXChanged(_this, _onChanged, ETextCommit::Default, _vectorType, EAxisList::X, false)
								.OnYChanged(_this, _onChanged, ETextCommit::Default, _vectorType, EAxisList::Y, false)
								.OnZChanged(_this, _onChanged, ETextCommit::Default, _vectorType, EAxisList::Z, false)
								.OnXCommitted(_this, _onChanged, _vectorType, EAxisList::X, true)
								.OnYCommitted(_this, _onChanged, _vectorType, EAxisList::Y, true)
								.OnZCommitted(_this, _onChanged, _vectorType, EAxisList::Z, true)
								.AllowSpin(true)
								.SpinDelta(1)
								
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Center)
						[
						SNew(SButton)
							.HAlign(HAlign_Center)
							.Text(FText::FromString("Reset"))
							.OnPressed(_this, _onReset)
						]
					]
				]
			];
}

template <typename T>
inline TSharedRef<SBorder> Statics::CreateButton(T* _this, FString _text, TDelegate<void()>::TMethodPtr<T> _onPressed)
{
	return SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(FLinearColor(0,0,0))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SBorder)
			.Padding(5)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString(_text))
					.OnPressed(_this, _onPressed)
				]
				]
			];
}

inline FSlateColor Statics::GetInnerBackgroundColor()
{
	FSlateColor Color;
	const int32 IndentLevel = 0;
	Color = GetRowBackgroundColor(IndentLevel, false);

	return Color;
}

inline FSlateColor Statics::GetRowBackgroundColor(int32 _indentLevel, bool _isHovered)
{
	int32 ColorIndex = 0;
	int32 Increment = 1;

	for (int i = 0; i < _indentLevel; ++i)
	{
		ColorIndex += Increment;

		if (ColorIndex == 0 || ColorIndex == 3)
		{
			Increment = -Increment;
		}
	}

	static const uint8 ColorOffsets[] =
	{
		0, 4, (4 + 2), (6 + 4), (10 + 6)
	};

	const FSlateColor BaseSlateColor = _isHovered ? 
		FAppStyle::Get().GetSlateColor("Colors.Header") : 
		FAppStyle::Get().GetSlateColor("Colors.Panel");

	const FColor BaseColor = BaseSlateColor.GetSpecifiedColor().ToFColor(true);

	const FColor ColorWithOffset(
		BaseColor.R + ColorOffsets[ColorIndex], 
		BaseColor.G + ColorOffsets[ColorIndex], 
		BaseColor.B + ColorOffsets[ColorIndex]);

	return FSlateColor(FLinearColor::FromSRGBColor(ColorWithOffset));
}

inline void Statics::DisplayNotification(FString _text, bool _success)
{
	FNotificationInfo Info(FText::FromString(_text));
	if (_success)
		Info.Image = FCoreStyle::Get().GetBrush(TEXT("NotificationList.DefaultMessage"));
	else
		Info.Image = FCoreStyle::Get().GetBrush(TEXT("NotificationList.FailImage"));

	Info.ExpireDuration = 5.0f;
	Info.bUseLargeFont = true;
	Info.bUseThrobber = true;
	Info.bUseSuccessFailIcons = false;
	Info.bUseLargeFont = false;
	Info.bFireAndForget = true;

	FSlateNotificationManager::Get().AddNotification(Info);
}

template<class T>
inline TSharedRef<SBorder> Statics::CreateInputField(T* _this, FString _default, TDelegate<void(const FText&)>::TMethodPtr<T> _onNameChanged)
{
	return SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(FLinearColor(0,0,0))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SBorder)
			.Padding(5)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString(_default))
				.OnTextChanged(_this, _onNameChanged)
			]
		];
}

