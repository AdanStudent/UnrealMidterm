// Fill out your copyright notice in the Description page of Project Settings.

#include "GenericHUD.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"

AGenericHUD::AGenericHUD() 
{
	GameplayHUDClass = NULL;

	DeadMenuClass = NULL;

	PauseMenuClass = NULL;
}

bool AGenericHUD::ShowSpecificMenu(TSubclassOf<class UUserWidget> ClassToShow, bool GameOnly, bool ShowMouse)
{
	HideAllMenus();

	if (ClassToShow)
	{
		//create widget then add to screen
		UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), ClassToShow);
		CreatedWidget->AddToPlayerScreen();

		if (GameOnly)
		{
			//set out input mode to game only which allows us full control of the player
			GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
		}
		else
		{
			//set out input mode to UI only, just in case we really don't want the player to be able to move
			FInputModeUIOnly InputMode;

			//this allows us to take the mouse out of the viewport
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		}

		//update the mouse cursor
		GetOwningPlayerController()->bShowMouseCursor = ShowMouse;

		//exit the function
		return true;
	}

	return false;
}

void AGenericHUD::HideAllMenus()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(this);
}
