// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPerson_Class415GameMode.h"
#include "FirstPerson_Class415Character.h"
#include "UObject/ConstructorHelpers.h"

AFirstPerson_Class415GameMode::AFirstPerson_Class415GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
