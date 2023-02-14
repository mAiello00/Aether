// Copyright Epic Games, Inc. All Rights Reserved.

#include "AetherGameMode.h"
#include "AetherCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAetherGameMode::AAetherGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
