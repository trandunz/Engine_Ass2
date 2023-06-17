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
#include "AssetToolsModule.h"
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
#include "Brushes/SlateImageBrush.h"
#include "Editor/PropertyEditor/Private/UserInterface/PropertyEditor/PropertyEditorConstants.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "PluginUtils.h"
#include "SlateOptMacros.h"
#include "SourceCodeNavigation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "PluginBrowser/Private/PluginBrowserModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/MessageDialog.h"
#include "HAL/FileManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "PluginBrowser/Private/SFilePathBlock.h"
#include "SlateFwd.h"
#include "Input/Reply.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IPluginWizardDefinition.h"
#include "PluginBrowser/Private/NewPluginDescriptorData.h"
#include "Slate/Public/Framework/Application/SlateApplication.h"
#include "ModuleDescriptor.h"
#include "Animation/AnimBlueprint.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureDefines.h"

class SLATEDESIGNER_API Statics : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(Statics)
	{}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);

	static bool CloneBlueprint(UObject* SourceBlueprint, const FString NewBlueprintName, const FString NewBlueprintPath);
	static bool RenameBlueprint(UObject* SourceBlueprint, const FString NewBlueprintName);
	static FReply CreateObject(const FString NewBlueprintPath, const FString NewBlueprintName);
	static bool DoesPathContainValidObject(const FString& ObjectPath);

	
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
 	* @brief Wraps the specified widget inside of a scroll box and returns a border
 	* @tparam T 
 	* @param _widget 
 	* @return 
 	*/
	template<class T>
	static TSharedRef<SBorder> WrapWidgetInBorder(TSharedRef<T> _widget);

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

	template <class T>
	static TSharedRef<SBorder> CreateVectorInputField2D(T* _this, FString _name,
	TDelegate<TOptional<double>()>::TConstMethodPtr<T> _valueX,
	TDelegate<TOptional<double>()>::TConstMethodPtr<T> _valueY,
	TDelegate<void(double)>::TMethodPtr<T> _onChangedX,
	TDelegate<void(double)>::TMethodPtr<T> _onChangedY,
	TDelegate<void()>::TMethodPtr<T> _onReset);

	/**
	 * @brief Returns a border containing a button
	 * @tparam T 
	 * @param _this 
	 * @param _text 
	 * @param _onPressed
	 * @param _texture 
	 * @return 
	 */
	template<class T>
	static TSharedRef<SBorder> CreateButton(T* _this, FString _text, TDelegate<void()>::TMethodPtr<T> _onPressed, UTexture2D* _texture);
	template<class T>
	static TSharedRef<SBorder> CreateButton(T* _this, FString _text, TDelegate<void(int)>::TMethodPtr<T> _onPressed, int _intParam, UTexture2D* _texture);

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

inline bool Statics::CloneBlueprint(UObject* _sourceBlueprint, const FString _newBlueprintName, const FString _newBlueprintPath)
{
	//
	// Get Asset Tools and Create Asset
	
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	//
	// Run duplication

	auto DuplicatedBlueprint = (AssetTools.DuplicateAsset
		(_newBlueprintName, _newBlueprintPath, _sourceBlueprint));

	if (DuplicatedBlueprint)
		DuplicatedBlueprint->RemoveFromRoot();
	if (_sourceBlueprint)
		_sourceBlueprint->RemoveFromRoot();
	
	//
	//	Validity check
	if (!DuplicatedBlueprint)
	{
		UE_LOG(LogTemp, Warning, TEXT("CloneBlueprint: Failed! new blueprint is NULL"));
		return false;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("CloneBlueprint: Success! Blueprint made."));
	
	return true;
}

inline bool Statics::RenameBlueprint(UObject* SourceBlueprint, const FString NewBlueprintName)
{
	return false;
}

inline FReply Statics::CreateObject(const FString _sourceBlueprintPath, const FString _newBlueprintName)
{
	//
	// Check if there is a valid folder

	//	- must use /Game/ or else we get an invalid source...
	const FString DesiredFolder = "/Game/";

	//
	// Get the source blueprint

	auto SourceObject = LoadObject<UObject>(nullptr, *_sourceBlueprintPath);
	if (!SourceObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateObject: Failure! Invalid source object!"));
		return FReply::Unhandled();
	}

	//
	// Duplicate the object in the new path and name

	//	- declare name and path variables
	FString NewBlueprintName = "";

	//	- try find valid name

	const FString PathToCheck = DesiredFolder + _newBlueprintName;
	if (DoesPathContainValidObject(PathToCheck))
	{
		//	- try find valid name if already existing
		for (int32 i = 1; i < 999; i++)
		{
			//	- define new name and path
			FString NewName = _newBlueprintName + FString::FromInt(i);
			FString NewPath = DesiredFolder + NewName;

			//	- check if available
			if (!DoesPathContainValidObject(NewPath))
			{
				NewBlueprintName = NewName;
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateObject: %s does not already exist."), *PathToCheck);
		NewBlueprintName = _newBlueprintName;
	}
	
	//	- check name validity
	if (NewBlueprintName == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateObject: Failure! 999+ Items of same name!"));
		return FReply::Unhandled();
	}

	//
	// Run blueprint duplicate
	CloneBlueprint(SourceObject, NewBlueprintName, DesiredFolder);
	
	return FReply::Handled();
}

inline bool Statics::DoesPathContainValidObject(const FString& ObjectPath)
{
	TSoftObjectPtr<UObject> ObjectPtr(ObjectPath);

	// Check if the object pointer is valid
	if (ObjectPtr.IsValid())
	{
		// Check if the object has been loaded
		UObject* ResolvedObject = ObjectPtr.Get();
		if (ResolvedObject)
		{
			// Object is valid and loaded
			return true;
		}
		else
		{
			// Object is valid but not loaded
			// You can optionally load the object using ObjectPtr.LoadSynchronous() here
			return false;
		}
	}
	else
	{
		// Object is not valid
		return false;
	}
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

template <class T>
TSharedRef<SBorder> Statics::WrapWidgetInBorder(TSharedRef<T> _widget)
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
					_widget
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

template <class T>
inline TSharedRef<SBorder> Statics::CreateVectorInputField2D(T* _this, FString _name,
	TDelegate<TOptional<double>()>::TConstMethodPtr<T> _valueX,
	TDelegate<TOptional<double>()>::TConstMethodPtr<T> _valueY,
	TDelegate<void(double)>::TMethodPtr<T> _onChangedX,
	TDelegate<void(double)>::TMethodPtr<T> _onChangedY,
	TDelegate<void()>::TMethodPtr<T> _onReset)
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
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SNumericEntryBox<double>)
								.OnValueChanged(_this, _onChangedX)
								.Value(_this, _valueX)
							]
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SNumericEntryBox<double>)
								.OnValueChanged(_this, _onChangedY)
								.Value(_this, _valueY)
							]
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

template <class T>
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

template <typename T>
inline TSharedRef<SBorder> Statics::CreateButton(T* _this, FString _text, TDelegate<void()>::TMethodPtr<T> _onPressed, UTexture2D* _texture)
{
	auto button = SNew(SButton)
					.Text(FText::FromString(_text))
					.OnPressed(_this, _onPressed);

	button.Get().SetContent(
			SNew(SImage)
			.Image(new FSlateImageBrush(Cast<UTexture2D>(_texture), UE::Slate::FDeprecateVector2DParameter{50.0, 50.0})));
	
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
					button
				]
				]
			];
}

template <class T>
TSharedRef<SBorder> Statics::CreateButton(T* _this, FString _text, TDelegate<void(int)>::TMethodPtr<T> _onPressed,
	int _intParam, UTexture2D* _texture)
{
	auto button = SNew(SButton)
					.Text(FText::FromString(_text))
					.VAlign(VAlign_Center)
					.OnPressed(_this, _onPressed, _intParam);

	button.Get().SetContent(
			SNew(SImage)
			.Image(new FSlateImageBrush(Cast<UTexture2D>(_texture), UE::Slate::FDeprecateVector2DParameter{50.0, 50.0})));
	
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
						button
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

