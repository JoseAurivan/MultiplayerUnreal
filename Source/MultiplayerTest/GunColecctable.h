// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterColecctable.h"
#include "WeaponMaster.h"

#include "GunColecctable.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERTEST_API AGunColecctable : public AMasterColecctable
{
	GENERATED_BODY()

	public:
		AGunColecctable();
	virtual void NotifyHit(UPrimitiveComponent* MyComp,
		AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
		FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TSubclassOf<AWeaponMaster> WeaponMaster;


};
