// Copyright Epic Games, Inc. All Rights Reserved.

#include "LiquidX_TestGameMode.h"
#include "LiquidX_TestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALiquidX_TestGameMode::ALiquidX_TestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
