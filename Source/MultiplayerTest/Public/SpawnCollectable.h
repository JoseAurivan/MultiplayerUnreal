// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Actor.h"
#include "MultiplayerTest/MasterColecctable.h"

#include "SpawnCollectable.generated.h"

UCLASS()
class MULTIPLAYERTEST_API ASpawnCollectable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnCollectable();
	UPROPERTY(Replicated, BlueprintReadWrite, Category="Light", EditAnywhere)
	UPointLightComponent* LightComponent;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category="CollectableSpawn", EditAnywhere)
	TSubclassOf<AMasterColecctable> Collectable;

	UPROPERTY(Category="LOCAL", BlueprintReadOnly)
	AMasterColecctable* InstantiatedCollectable;

	TScriptDelegate<FWeakObjectPtr> InstantiateEvent;
	FTimerHandle TimerHandle;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Instantiate")
	void ServerInstantiate();

	UFUNCTION(Category="Respawn", BlueprintCallable, CallInEditor)
	void TimeToRespawn();

};
