// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnCollectable.h"
#include "Net/UnrealNetwork.h"
#include "Components/LightComponent.h"

// Sets default values
ASpawnCollectable::ASpawnCollectable()
{
	bReplicates = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	if(LightComponent)
	{
		LightComponent -> SetupAttachment(RootComponent);
		LightComponent->Intensity = 250000.0f;
		LightComponent->AttenuationRadius = 65.0f;
		LightComponent->LightColor = FColor::Purple;
	}

}

// Called when the game starts or when spawned
void ASpawnCollectable::BeginPlay()
{
	Super::BeginPlay();
	ServerInstantiate();
	
}

// Called every frame
void ASpawnCollectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
}

void ASpawnCollectable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASpawnCollectable::ServerInstantiate_Implementation()
{
	FActorSpawnParameters* SpawnParameters = new FActorSpawnParameters;
	if(Collectable)
	{
		InstantiatedCollectable = GetWorld()->SpawnActor<AMasterColecctable>(
			Collectable,
			GetActorLocation(),
			GetActorRotation(),
			*SpawnParameters);
		
		InstantiateEvent.BindUFunction(this,"TimeToRespawn");
		InstantiatedCollectable->OnDestroyed.Add(InstantiateEvent);
	}
}

void ASpawnCollectable::TimeToRespawn()
{
	GetWorldTimerManager().SetTimer(TimerHandle,this,&ASpawnCollectable::ServerInstantiate,0.5f,false,20.0f);
}

