// Bachelor of Software Engineering 
// Media Design School 
// Auckland 
// New Zealand 
// (c) Media Design School
// File Name : SPluginDesignerMenu.cpp 
// Description : SPluginDesignerMenu Implementation File
// Author : William Inman
// Mail : William.inman@mds.ac.nz

#include "SPluginDesignerMenu.h"
#include "Statics.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
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


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

// Initialize our static variables
FAssetData SPluginDesignerMenu::MeshAssetData{};
FAssetData SPluginDesignerMenu::AnimationBPData{};

void SPluginDesignerMenu::Construct(const FArguments& InArgs)
{
	// Set window name to be "MyNewMenu" by default
	WindowName = FText::FromString("MyNewMenu");

	// create a new vertical box to hold everything
	TSharedRef<SVerticalBox> newParent = SNew(SVerticalBox);
	// create a vertical box for storing the names of all the menus
	TSharedRef<SVerticalBox> newCurrentlyUsedMenus = SNew(SVerticalBox);
	// Create a scroll box to hold the newParent vertical box
	TSharedRef<SScrollBox> scrollBox = SNew(SScrollBox);
	
	// Create Parent Vertical Box To Hold All Menu's
	ParentBox = &newParent.Get();

	CurrentlyUsedMenus = &newCurrentlyUsedMenus.Get();
	
	// Setup all menu's
	InitSlateDesignerMenu();
	
	// add the parent vertical box too the scroll box
	scrollBox->AddSlot()
	.AttachWidget(newParent);
	
	// Assign Finished Parent (the scroll box)
	ChildSlot
	[
		scrollBox
	];
}

void SPluginDesignerMenu::InitSlateDesignerMenu()
{
	// Hierarchy Helper
	//
	// ParentBox
	//		Border (black)
	//			Border (Grey)
	//				Vertical Box
	//					HorizontalBox
	//						Button
	//						Vertical Box
	//							Button
	//							Input Field
	//						Button
	//					ScrollBox

	// Create the menu
	auto newMenu = SNew(SBorder)
		.Padding(20)
		.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(FLinearColor(0.0f,0.0f,0.0f))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(Statics::GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					[
						Statics::CreateButton<SPluginDesignerMenu>(this, "Create Example Menu", &SPluginDesignerMenu::InitExampleMenu)
					]
					+SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							Statics::CreateButton<SPluginDesignerMenu>(this, "Create New Menu", &SPluginDesignerMenu::OnCreateNewMenuClicked)
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							Statics::CreateInputField<SPluginDesignerMenu>(this, "MyNewMenu", &SPluginDesignerMenu::OnWindowNameChanged)
						]
					]
					+SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					[
						Statics::CreateButton<SPluginDesignerMenu>(this, "Delete Last Menu", &SPluginDesignerMenu::OnDeleteLastMenuClicked)
					]
				]
				+SVerticalBox::Slot()
				.MaxHeight(100)
				[
					Statics::WrapWidgetInScrollBox(CurrentlyUsedMenus->AsShared())
				]
			]
		];

	// Add menu to the parent
	ParentBox->AddSlot()
	.AutoHeight()
	[
		newMenu
	];
}

void SPluginDesignerMenu::InitExampleMenu()
{
	// Hierarchy Helper
	//
	// ParentBox
	//		Border (black)
	//			Border (Grey)
	//				Vertical Box
	//					Title text
	//					Name input field and text
	//					Mesh Picker
	//					AnimBP Picker
	//					Position Vector Field
	//					Rotation Vector Field
	//					Scale Vector Field
	//					Spawn Button

	// Display Notification
	Statics::DisplayNotification("Example Slate Menu Created");

	// Add menu to menu name list
	auto newMenuTitle = Statics::CreateCenteredNewText("NPC (Example Menu)");
	SpawnedMenuTitles.Push(newMenuTitle);
	CurrentlyUsedMenus->AddSlot()
	.AutoHeight()
	[
		newMenuTitle
	];

	// Setup thumbnail pools for asset pickers
	MeshThumbnailPool = MakeShareable(new FAssetThumbnailPool(24));
	AnimBPThumbnailPool = MakeShareable(new FAssetThumbnailPool(24));

	// Create the menu
	auto newMenu =
		SNew(SBorder)
		.Padding(20)
		.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(FLinearColor(0.0f,0.0f,0.0f))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(Statics::GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					Statics::CreateNewTitle("NPC (Example Menu)")
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					Statics::CreateNamedInputField<SPluginDesignerMenu>(this, "Name: ", &SPluginDesignerMenu::OnNameChanged)
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					Statics::CreateBlueprintPicker("Mesh", USkeletalMesh::StaticClass(), &SPluginDesignerMenu::GetMeshPath, FOnSetObject::CreateSP(this, &SPluginDesignerMenu::OnMeshChanged), MeshThumbnailPool)
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					Statics::CreateBlueprintPicker("Animation Blueprint", UAnimBlueprint::StaticClass(), &SPluginDesignerMenu::AnimBPPath , FOnSetObject::CreateSP(this, &SPluginDesignerMenu::OnAnimBPChanged), AnimBPThumbnailPool)
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					Statics::CreateVectorInputField<SPluginDesignerMenu>(this, "Position", &SPluginDesignerMenu::GetXPosition, &SPluginDesignerMenu::GetYPosition, &SPluginDesignerMenu::GetZPosition, &SPluginDesignerMenu::OnPositionChanged, &SPluginDesignerMenu::OnResetPosition)
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					Statics::CreateVectorInputField<SPluginDesignerMenu>(this, "Rotation", &SPluginDesignerMenu::GetXRotation, &SPluginDesignerMenu::GetYRotation, &SPluginDesignerMenu::GetZRotation, &SPluginDesignerMenu::OnRotationChanged, &SPluginDesignerMenu::OnResetRotation, 1)
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					Statics::CreateVectorInputField<SPluginDesignerMenu>(this, "Scale", &SPluginDesignerMenu::GetXScale, &SPluginDesignerMenu::GetYScale, &SPluginDesignerMenu::GetZScale, &SPluginDesignerMenu::OnScaleChanged, &SPluginDesignerMenu::OnResetScale, 2)
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					Statics::CreateButton<SPluginDesignerMenu>(this, "Spawn", &SPluginDesignerMenu::OnSpawnClicked)
				]
			]
		];

	// Add the menu to the list of menus
	SpawnedMenus.Push(newMenu);
	// Add it to the parent
	ParentBox->AddSlot()
	.AutoHeight()
	[
		newMenu
	];
}

void SPluginDesignerMenu::InitBasicMenu(FString _title)
{
	// Hierarchy Helper
    //
    // ParentBox
    //		Border (black)
    //			Border (Grey)
    //				Vertical Box
    //					Title text

	// Display Notification
	Statics::DisplayNotification("Slate Menu '" + _title + "' Created");

	// Add menu to menu name list
	auto newMenuTitle = Statics::CreateCenteredNewText(_title);
	SpawnedMenuTitles.Push(newMenuTitle);
	CurrentlyUsedMenus->AddSlot()
	.AutoHeight()
	[
		newMenuTitle
	];

	// Create menu
	auto newMenu = SNew(SBorder)
		.Padding(20)
		.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(FLinearColor(0.0f,0.0f,0.0f))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
			.BorderBackgroundColor(Statics::GetInnerBackgroundColor())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					Statics::CreateNewTitle(_title)
				]
			]
		];

	// Add it to menu list
	SpawnedMenus.Push(newMenu);

	// Add it to the parent widget
	ParentBox->AddSlot()
	.AutoHeight()
	[
		newMenu
	];
}

void SPluginDesignerMenu::OnNameChanged(const FText& _newName)
{
	Name = _newName;
}

FName SPluginDesignerMenu::GetNPCDisplayName()
{
	return TEXT("NPC");
}

void SPluginDesignerMenu::OnWindowNameChanged(const FText&  _newName)
{
	WindowName = _newName;
}

void SPluginDesignerMenu::OnSpawnClicked()
{
	// Display Notification
	Statics::DisplayNotification("Spawned " + Name.ToString());
	
	// Spawn Character
	auto* currentLevel = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();
	auto* newNPC = GEditor->AddActor(currentLevel, ACharacter::StaticClass(), TransformToPlace);

	// Apply plugin Data
	newNPC->SetActorLabel(Name.ToString()); // Name
	if (auto* newCharacter = Cast<ACharacter>(newNPC))
	{
		if (auto* skeletalMeshAsset = Cast<USkeletalMesh>(MeshAssetData.GetAsset()))
		{
			newCharacter->GetMesh()->SetSkeletalMeshAsset(skeletalMeshAsset); // Skeletal Mesh
		}

		// Set thee mesh to be in the center of the charaacter capsule
		newCharacter->GetMesh()->SetRelativeLocation({0,0,-90});
		newCharacter->GetMesh()->SetRelativeRotation({0,-90,0});

		// Set the animation blueprint
		newCharacter->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		if (auto* animBP = Cast<UAnimBlueprint>(AnimationBPData.GetAsset()))
		{
			newCharacter->GetMesh()->SetAnimInstanceClass((UClass*)animBP->GetAnimBlueprintGeneratedClass());
		}

		// Auto possess the AI controller (not added as a field so does nothing)
		newCharacter->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	}
}

void SPluginDesignerMenu::OnCreateNewMenuClicked()
{
	// Create a new menu with the name specified by the input field
	InitBasicMenu(WindowName.ToString());
}

void SPluginDesignerMenu::OnDeleteLastMenuClicked()
{
	// Display Notification
	Statics::DisplayNotification("Last Menu Deleted");

	// Remove the last item from name list
	if (SpawnedMenuTitles.Num() > 0)
	{
		CurrentlyUsedMenus->RemoveSlot(SpawnedMenuTitles[SpawnedMenuTitles.Num() - 1]);
		SpawnedMenuTitles.Pop();
	}
	// Remove last item from menu list
	if (SpawnedMenus.Num() > 0)
	{
		ParentBox->RemoveSlot(SpawnedMenus[SpawnedMenus.Num() - 1]);
		SpawnedMenus.Pop();
	}
}

void SPluginDesignerMenu::OnCreatePluginClicked()
{
	// Open plugin wizard / creator
	const FName pluginCreatorTabName( TEXT( "PluginCreator" ) );
	FGlobalTabmanager::Get()->TryInvokeTab(pluginCreatorTabName);
}

void SPluginDesignerMenu::OnMeshChanged(const FAssetData& _assetData)
{
	MeshAssetData = _assetData;
}

FString SPluginDesignerMenu::GetMeshPath()
{
	if (MeshAssetData.IsValid())
	{
		return MeshAssetData.ObjectPath.ToString();
	}
	return "";
}

void SPluginDesignerMenu::OnAnimBPChanged(const FAssetData& _assetData)
{
	AnimationBPData = _assetData;
}

FString SPluginDesignerMenu::AnimBPPath()
{
	if (AnimationBPData.IsValid())
	{
		return AnimationBPData.ObjectPath.ToString();
	}
	return "";
}

void SPluginDesignerMenu::OnPositionChanged(FVector::FReal _newValue, ETextCommit::Type _commitInfo, int _transformField,
	EAxisList::Type _axis, bool _bCommitted)
{
	FVector currentPosition = TransformToPlace.GetLocation();
	auto newVector = FVector((_axis & EAxisList::X) ?  FVector(_newValue).X : currentPosition.X,
							 (_axis & EAxisList::Y) ?  FVector(_newValue).Y : currentPosition.Y,
							 (_axis & EAxisList::Z) ?  FVector(_newValue).Z : currentPosition.Z);
	TransformToPlace.SetTranslation(newVector);
}

TOptional<double> SPluginDesignerMenu::GetXPosition() const
{
	return TransformToPlace.GetTranslation().X;
}

TOptional<double> SPluginDesignerMenu::GetYPosition() const
{
	return TransformToPlace.GetTranslation().Y;
}

TOptional<double> SPluginDesignerMenu::GetZPosition() const
{
	return TransformToPlace.GetTranslation().Z;
}

void SPluginDesignerMenu::OnResetPosition()
{
	TransformToPlace.SetLocation({0.0f, 0.0f, 0.0f});
}

void SPluginDesignerMenu::OnRotationChanged(FVector::FReal _newValue, ETextCommit::Type _commitInfo, int _transformField, EAxisList::Type _axis, bool _bCommitted)
{
	auto currentRotation = TransformToPlace.GetRotation().Rotator();
	auto newVector = FVector((_axis & EAxisList::X) ?  FVector(_newValue).X : currentRotation.Roll,
							 (_axis & EAxisList::Y) ?  FVector(_newValue).Y : currentRotation.Pitch,
							 (_axis & EAxisList::Z) ?  FVector(_newValue).Z : currentRotation.Yaw);
	TransformToPlace.SetRotation(UE::Math::TQuat<double>::MakeFromRotator(FRotator::MakeFromEuler(newVector)));
}

TOptional<double> SPluginDesignerMenu::GetXRotation() const
{
	return TransformToPlace.GetRotation().Euler().X;
}

TOptional<double> SPluginDesignerMenu::GetYRotation() const
{
	// Convert back to degrees
	return TransformToPlace.GetRotation().Euler().Y;
}

TOptional<double> SPluginDesignerMenu::GetZRotation() const
{
	return TransformToPlace.GetRotation().Euler().Z;
}

void SPluginDesignerMenu::OnResetRotation()
{
	TransformToPlace.SetRotation(UE::Math::TQuat<double>::MakeFromRotator({0.0,0.0,0.0}));
}

void SPluginDesignerMenu::OnScaleChanged(FVector::FReal _newValue, ETextCommit::Type _commitInfo, int _transformField, EAxisList::Type _axis, bool _bCommitted)
{
	FVector currentScale = TransformToPlace.GetScale3D();
	auto newVector = FVector((_axis & EAxisList::X) ?  FVector(_newValue).X : currentScale.X,
							 (_axis & EAxisList::Y) ?  FVector(_newValue).Y : currentScale.Y,
							 (_axis & EAxisList::Z) ?  FVector(_newValue).Z : currentScale.Z);
	TransformToPlace.SetScale3D(newVector);
}

TOptional<double> SPluginDesignerMenu::GetXScale() const
{
	return TransformToPlace.GetScale3D().X;
}

TOptional<double> SPluginDesignerMenu::GetYScale() const
{
	return TransformToPlace.GetScale3D().Y;
}

TOptional<double> SPluginDesignerMenu::GetZScale() const
{
	return TransformToPlace.GetScale3D().Z;
}

void SPluginDesignerMenu::OnResetScale()
{
	TransformToPlace.SetScale3D({1.0f,1.0f,1.0f});
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
