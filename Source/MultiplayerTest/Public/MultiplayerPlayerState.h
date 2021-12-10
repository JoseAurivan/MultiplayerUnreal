// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayerPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERTEST_API AMultiplayerPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Score")
	int PlayerScore = 0;

	UFUNCTION(BlueprintCallable,CallInEditor,Category="Score")
	void IncreaseScore();
	
};
