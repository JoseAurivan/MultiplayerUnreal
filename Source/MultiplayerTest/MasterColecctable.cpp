// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterColecctable.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMasterColecctable::AMasterColecctable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));

}

void AMasterColecctable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMasterColecctable,MainMesh);
}

void AMasterColecctable::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
}

// Called when the game starts or when spawned
void AMasterColecctable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMasterColecctable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

