// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "MultiplayerTestCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	DamageEvent.BindUFunction(this, "TakeDamage");
	GetOwner()->OnTakePointDamage.Add(DamageEvent);
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::TakeDamage()
{
	//Receives Damage 
	AMultiplayerTestCharacter* DamagedPlayer = Cast<AMultiplayerTestCharacter>(GetOwner());
	//Updates Health at Code Level
	Health -= DamagedPlayer->DamageReceived;
	//Updates UI Health (Blueprint Implemented)
	DamagedPlayer->UpdateUI();

	//Checks Death
	if (Health <= 0)
	{
		//Kills the player at server level
		Cast<AMultiplayerTestCharacter>(GetOwner())->MulticastPlayerDeath();
		//Increases the score of the last player who gave damage
		DamagedPlayer->DamageCauser->ClientUpdateScore();
	}
	
}
