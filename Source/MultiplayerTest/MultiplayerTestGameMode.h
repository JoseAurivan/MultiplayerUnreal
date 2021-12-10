// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MultiplayerTestCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerTestGameMode.generated.h"

UCLASS(minimalapi)
class AMultiplayerTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerTestGameMode();
	
	virtual void BeginPlay() override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Respawn")
	TSubclassOf<AMultiplayerTestCharacter> Character;

	UFUNCTION(Server,Reliable,CallInEditor,Category="Respawn")
	void RespawnRequested(FTransform SpawnLocation, AController* Controller);
	
};



