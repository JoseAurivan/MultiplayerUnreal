// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "WeaponMaster.generated.h"

UCLASS()
class MULTIPLAYERTEST_API AWeaponMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponMaster();

	UPROPERTY(Replicated,BlueprintReadWrite,EditAnywhere, Category="Mesh")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(Replicated,BlueprintReadWrite,EditAnywhere, Category="Mesh")
	USkeletalMesh* MyMesh;

	UPROPERTY(Replicated,BlueprintReadWrite,EditAnywhere, Category="Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(Replicated,BlueprintReadWrite,EditAnywhere, Category="VFX")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(Replicated,BlueprintReadWrite,EditAnywhere, Category="SFX")
	USoundBase* GunShot;

	UPROPERTY(Replicated,BlueprintReadWrite,EditAnywhere,Category="SFX")
	UAudioComponent* GunShotComponent;
	
	UPROPERTY(Replicated,BlueprintReadWrite,EditAnywhere, Category="ClientGunfight")
	float FireRate;

	UPROPERTY(Replicated,BlueprintReadWrite,EditAnywhere, Category="ClientGunfight")
	float Damage;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void ServerFireWeapon(UCameraComponent* CameraComponent);

	UFUNCTION(NetMulticast,Reliable,BlueprintCallable,CallInEditor,Category="VFX")
	void MulticastWeaponFireVfx();
	




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
