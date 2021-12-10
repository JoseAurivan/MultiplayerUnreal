// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerTestGameMode.h"

#include "MultiplayerPlayerState.h"
#include "MultiplayerTestCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Tasks/GameplayTask_SpawnActor.h"
#include "UObject/ConstructorHelpers.h"

AMultiplayerTestGameMode::AMultiplayerTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/Gameplay/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PlayerStateClass = AMultiplayerPlayerState::StaticClass();
}

void AMultiplayerTestGameMode::BeginPlay()
{
}

AActor* AMultiplayerTestGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> StartLocations;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),APlayerStart::StaticClass(),StartLocations);
	for (auto StartLocation : StartLocations)
	{
		if(!StartLocation->ActorHasTag(TEXT("Taken")))
		{
			StartLocation->Tags.Add(TEXT("Taken"));
			return StartLocation;
		}
	}
	return nullptr;
}

void AMultiplayerTestGameMode::RespawnRequested_Implementation(FTransform SpawnLocation, AController* Controller)
{
	FActorSpawnParameters* SpawnParameters = new FActorSpawnParameters;
	SpawnParameters->Owner = this;
	AMultiplayerTestCharacter* SpawnedCharacter = GetWorld()->SpawnActor<AMultiplayerTestCharacter>(Character,
		SpawnLocation.GetLocation(),
		SpawnLocation.GetRotation().Rotator(),
		*SpawnParameters);
	Controller->Possess(SpawnedCharacter);
}
