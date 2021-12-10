// Fill out your copyright notice in the Description page of Project Settings.


#include "GunColecctable.h"

#include "MultiplayerTestCharacter.h"

AGunColecctable::AGunColecctable()
{
	bReplicates = true;
}

void AGunColecctable::NotifyHit(UPrimitiveComponent* MyComp,
                                AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
                                FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	AMultiplayerTestCharacter* hit = Cast<AMultiplayerTestCharacter>(Other);
	if (hit)
	{
		hit->ClientEquip(WeaponMaster);
		Destroy();
	}
}
