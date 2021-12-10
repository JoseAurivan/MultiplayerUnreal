// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponMaster.h"

#include "MultiplayerTestCharacter.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"



// Sets default values
AWeaponMaster::AWeaponMaster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	//Weapon Mesh
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("MyMesh");
	if(MyMesh)
		MeshComponent->SetSkeletalMesh(MyMesh);

	//Gun shot sound SFX
	GunShotComponent = CreateDefaultSubobject<UAudioComponent>("GunShotSound");
	GunShotComponent->bAutoActivate = false;


	
}

void AWeaponMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponMaster, MyMesh);
	DOREPLIFETIME(AWeaponMaster, MeshComponent);
	DOREPLIFETIME(AWeaponMaster, DamageType);
	DOREPLIFETIME(AWeaponMaster, Damage);
	DOREPLIFETIME(AWeaponMaster, MuzzleFlash);
	DOREPLIFETIME(AWeaponMaster, GunShot);
	DOREPLIFETIME(AWeaponMaster, GunShotComponent);
}

void AWeaponMaster::ServerFireWeapon_Implementation(UCameraComponent* CameraComponent)
{
	MulticastWeaponFireVfx();
	FHitResult* Hit = new FHitResult();
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + (CameraComponent->GetForwardVector() * 10000.0f);
	GetWorld()->LineTraceSingleByChannel(*Hit, Start, End, ECC_Visibility);
	if (Hit)
	{
		AMultiplayerTestCharacter* DamagedActor = Cast<AMultiplayerTestCharacter>(Hit->GetActor());
		if(DamagedActor)
		{
			DamagedActor->DamageReceived=UGameplayStatics::ApplyPointDamage(Hit->GetActor(),Damage,Start,*Hit,GetInstigator()->Controller,GetOwner(),DamageType);
			DamagedActor->DamageCauser = Cast<AMultiplayerTestCharacter>(GetOwner());
			DamagedActor->SpawnBlood();
		}
	}
		
}

void AWeaponMaster::MulticastWeaponFireVfx_Implementation()
{
	UGameplayStatics::SpawnEmitterAttached(
	MuzzleFlash,
	MeshComponent,
	FName(TEXT("Muzzle")),
	*new FVector(0.0f,0.0f,0.0f),
	*new FRotator(0.0f,0.0f,0.0f),
	*new FVector(1.0f,1.0f,1.0f),
	EAttachLocation::KeepRelativeOffset,
	true,
	EPSCPoolMethod::None,
	true);

	GunShotComponent->Play();
}



// Called when the game starts or when spawned
void AWeaponMaster::BeginPlay()
{
	Super::BeginPlay();
	//Setting Gun Sound
	if(GunShot->IsValidLowLevelFast())
		GunShotComponent->SetSound(GunShot);
}

// Called every frame
void AWeaponMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
