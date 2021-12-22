// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerTestCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "HealthComponent.h"
#include "MultiplayerPlayerState.h"
#include "MultiplayerTestGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


//////////////////////////////////////////////////////////////////////////
// AMultiplayerTestCharacter



AMultiplayerTestCharacter::AMultiplayerTestCharacter()
{
	bReplicates = true;
	bIsEquipped = false;
	bIsAds = false;
	bIsEquipped = false;
	DamageReceived = 0;
	bIsDead = false;
	bIsCrounched = false;

	//Set Health Component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetIsReplicated(true);
	HealthComponent->SetActive(true);
	AddOwnedComponent(HealthComponent);

	//Replicating Mesh
	GetMesh()->SetIsReplicated(true);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetIsReplicated(true);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;


	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = minWalkSpeed;
	GetCharacterMovement()->SetIsReplicated(true);


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AMultiplayerTestCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnPoint.SetLocation(GetActorLocation());
	SpawnPoint.SetRotation(GetActorQuat());
}



//////////////////////////////////////////////////////////////////////////
// Input


void AMultiplayerTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMultiplayerTestCharacter::ClientStartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMultiplayerTestCharacter::ClientStopSprint);

	PlayerInputComponent->BindAction("Ads", IE_Pressed, this, &AMultiplayerTestCharacter::ClientAdsOn);
	PlayerInputComponent->BindAction("Ads", IE_Released, this, &AMultiplayerTestCharacter::ClientAdsOff);

	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &AMultiplayerTestCharacter::MulticastFireWeapon);

	PlayerInputComponent->BindAction("Crounch",IE_Pressed,this,&AMultiplayerTestCharacter::ClientPLayerCrounch);


	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiplayerTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiplayerTestCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMultiplayerTestCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMultiplayerTestCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMultiplayerTestCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMultiplayerTestCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMultiplayerTestCharacter::OnResetVR);
}




void AMultiplayerTestCharacter::OnResetVR()
{
	// If MultiplayerTest is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in MultiplayerTest.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMultiplayerTestCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMultiplayerTestCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}


void AMultiplayerTestCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerTestCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerTestCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMultiplayerTestCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


////// NETWORKING //////
//ALL FUNCTIONS HERE ARE REPLICATED//

void AMultiplayerTestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//ACTOR REPLICATED
	DOREPLIFETIME(AMultiplayerTestCharacter, maxWalkSpeed);
	DOREPLIFETIME(AMultiplayerTestCharacter, minWalkSpeed);
	DOREPLIFETIME(AMultiplayerTestCharacter, adsSpeed);
	DOREPLIFETIME(AMultiplayerTestCharacter, bIsEquipped);
	DOREPLIFETIME(AMultiplayerTestCharacter, bIsAds)
	DOREPLIFETIME(AMultiplayerTestCharacter, EquippedWeapon);
	DOREPLIFETIME(AMultiplayerTestCharacter, HealthComponent);
	DOREPLIFETIME(AMultiplayerTestCharacter, DamageReceived);
	DOREPLIFETIME(AMultiplayerTestCharacter, bIsDead);
	DOREPLIFETIME(AMultiplayerTestCharacter, M_FireWeapon);
	DOREPLIFETIME(AMultiplayerTestCharacter, bIsCrounched);
	DOREPLIFETIME(AMultiplayerTestCharacter, crounchSpeed);
}

void AMultiplayerTestCharacter::ClientStopSprint_Implementation()
{
	if (!bIsAds && !bIsCrounched)
	{
		GetCharacterMovement()->MaxWalkSpeed = minWalkSpeed;
		ServerStopSprint();
	}
}

void AMultiplayerTestCharacter::ClientStartSprint_Implementation()
{
	if (!bIsAds && !bIsCrounched)
	{
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed;
		ServerStartSprint();
	}
}

void AMultiplayerTestCharacter::ServerStartSprint_Implementation()
{
	if (!bIsAds)
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed;
	
}

void AMultiplayerTestCharacter::ServerStopSprint_Implementation()
{
	if (!bIsAds)
		GetCharacterMovement()->MaxWalkSpeed = minWalkSpeed;
}


void AMultiplayerTestCharacter::ClientEquip_Implementation(TSubclassOf<AWeaponMaster> WeaponMaster)
{
	bIsEquipped = true;
	ServerEquip(WeaponMaster);
	ServerSpawnWeapon(WeaponMaster);
}

void AMultiplayerTestCharacter::ServerEquip_Implementation(TSubclassOf<AWeaponMaster> WeaponMaster)
{
	bIsEquipped = true;
}

void AMultiplayerTestCharacter::ClientAdsOn_Implementation()
{
	bIsAds = true;
	if(!bIsCrounched)
		GetCharacterMovement()->MaxWalkSpeed = adsSpeed;
	CameraBoom->TargetArmLength = 150.0f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
	ServerAdsOn();
}

void AMultiplayerTestCharacter::ClientAdsOff_Implementation()
{
	bIsAds = false;
	if(!bIsCrounched)
		GetCharacterMovement()->MaxWalkSpeed = minWalkSpeed;
	CameraBoom->TargetArmLength = 300.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	ServerAdsOff();
}

void AMultiplayerTestCharacter::ServerAdsOn_Implementation()
{
	bIsAds = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	if(!bIsCrounched)
		GetCharacterMovement()->MaxWalkSpeed = adsSpeed;
	bUseControllerRotationYaw = true;
}

void AMultiplayerTestCharacter::ServerAdsOff_Implementation()
{
	bIsAds = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	if(!bIsCrounched)
		GetCharacterMovement()->MaxWalkSpeed = minWalkSpeed;
	bUseControllerRotationYaw = false;
}

void AMultiplayerTestCharacter::ServerSpawnWeapon_Implementation(TSubclassOf<AWeaponMaster> WeaponMaster)
{
	FVector SocketLocation;
	SocketLocation = GetMesh()->GetSocketLocation("Weapon");
	FRotator SocketRotation;
	SocketRotation = GetMesh()->GetSocketRotation("Weapon");
	FActorSpawnParameters* SpawnParameters = new FActorSpawnParameters;
	SpawnParameters->Instigator = this;
	SpawnParameters->Owner = this;
	EquippedWeapon = GetWorld()->SpawnActor<AWeaponMaster>(WeaponMaster, SocketLocation, SocketRotation,
	                                                       *SpawnParameters);
	EquippedWeapon->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			true),
		"Weapon");
}

void AMultiplayerTestCharacter::MulticastFireWeapon_Implementation()
{
	if (bIsEquipped)
	{
		EquippedWeapon->ServerFireWeapon(FollowCamera);
		PlayAnimMontage(M_FireWeapon, 1, NAME_None);
	}
}

void AMultiplayerTestCharacter::ClientUpdateScore_Implementation()
{
	//Gets last player to apply damage
	AMultiplayerPlayerState* causerState = Cast<AMultiplayerPlayerState>(GetPlayerState());
	//increases score to that player
	if(causerState)
	{
		causerState->IncreaseScore();
		UpdateUIScore();
	}
}

void AMultiplayerTestCharacter::MulticastPlayerDeath_Implementation()
{	
	bIsDead = true;
	DestroyUI();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetCapsuleComponent()->UpdateCollisionProfile();
	GetMovementComponent()->StopMovementImmediately();	
	GetMovementComponent()->Deactivate();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ClientRespawnCharacter();

}

void AMultiplayerTestCharacter::ClientRespawnCharacter_Implementation()
{
	GetWorldTimerManager().SetTimer(
		TimeToRespawn,
		this,
		&AMultiplayerTestCharacter::ServerRespawnPawn,
		0.5f,
		false,
		2.0f
		);
}

void AMultiplayerTestCharacter::ServerRespawnPawn_Implementation()
{
	AMultiplayerTestGameMode* GameMode = Cast<AMultiplayerTestGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->RespawnRequested(SpawnPoint,GetController());
}

void AMultiplayerTestCharacter::ClientPLayerCrounch_Implementation()
{
	if(!bIsCrounched)
		GetCharacterMovement()->MaxWalkSpeed=crounchSpeed;
		
	else	
		GetCharacterMovement()->MaxWalkSpeed=minWalkSpeed;
			
	ServerPLayerCrounch();
	
}

void AMultiplayerTestCharacter::ServerPLayerCrounch_Implementation()
{
	if(!bIsCrounched)
	{
		bIsCrounched = true;
		GetCharacterMovement()->MaxWalkSpeed=crounchSpeed;
		
	}else
	{
		bIsCrounched = false;
		GetCharacterMovement()->MaxWalkSpeed=minWalkSpeed;
		
	}
}