// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HealthComponent.h"
#include "MultiplayerPlayerState.h"
#include "WeaponMaster.h"
#include "GameFramework/Character.h"
#include "MultiplayerTestCharacter.generated.h"

UCLASS(config=Game)
class AMultiplayerTestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;




public:
	virtual void BeginPlay() override;
	
	AMultiplayerTestCharacter();

	UPROPERTY(Replicated,EditAnywhere)
	float DamageReceived;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	AMultiplayerTestCharacter* DamageCauser;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Spawn")
	FTransform SpawnPoint ;

	FTimerHandle TimeToRespawn;

//////////////////
///NETWORKING////


public:
	
	UPROPERTY(Replicated)
	float minWalkSpeed = 500.0f;

	UPROPERTY(Replicated)
	float maxWalkSpeed = 800.0f;

	UPROPERTY(Replicated)
	float adsSpeed = 200.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere)
	bool bIsEquipped;

	UPROPERTY(Replicated,BlueprintReadOnly,VisibleAnywhere)
	bool bIsAds;

	UPROPERTY(Replicated,BlueprintReadOnly,VisibleAnywhere)
	bool bIsDead;

	UPROPERTY(Replicated,VisibleAnywhere)
	AWeaponMaster* EquippedWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	UPROPERTY(Replicated, EditAnywhere, Category="Animation")
	UAnimMontage* M_FireWeapon;

	UFUNCTION(Client,Unreliable,BlueprintCallable, CallInEditor, Category="Score")
	void ClientUpdateScore();
	
	UFUNCTION(BlueprintImplementableEvent,Category="UI",BlueprintCallable)
	void UpdateUI();

	UFUNCTION(BlueprintImplementableEvent,Category="UI",BlueprintCallable)
	void UpdateUIScore();

	UFUNCTION(BlueprintImplementableEvent,Category="UI",BlueprintCallable)
	void DestroyUI();

	UFUNCTION(BlueprintImplementableEvent,Category="VFX",BlueprintCallable)
	void SpawnBlood();

	UFUNCTION(Client,Unreliable)
	void ClientStartSprint();

	UFUNCTION(Client,Unreliable)
	void ClientStopSprint();

	UFUNCTION(Server,Reliable)
	void ServerStartSprint();

	UFUNCTION(Server,Reliable)
	void ServerStopSprint();

	UFUNCTION(Client, Unreliable)
	void ClientEquip(TSubclassOf<AWeaponMaster> WeaponMaster);

	UFUNCTION(Server, Reliable)
	void ServerEquip(TSubclassOf<AWeaponMaster> WeaponMaster);

	UFUNCTION(Client,Unreliable,Category="Aim")
	void ClientAdsOn();

	UFUNCTION(Client,Unreliable,Category="Aim")
	void ClientAdsOff();

	UFUNCTION(Server,Reliable,Category="Aim")
	void ServerAdsOn();

	UFUNCTION(Server,Reliable,Category="Aim")
	void ServerAdsOff();

	UFUNCTION(Server, Reliable)
	void ServerSpawnWeapon(TSubclassOf<AWeaponMaster> WeaponMaster);

	UFUNCTION(NetMulticast,Reliable)
	void MulticastPlayerDeath();
	
	UFUNCTION(NetMulticast,Reliable)
	void MulticastFireWeapon();

	UFUNCTION(Client,Unreliable,BlueprintCallable,CallInEditor,Category="Respawn")
	void ClientRespawnCharacter();
	
	UFUNCTION(Server,Reliable,BlueprintCallable,Category="Respawn")
	void ServerRespawnPawn();
	
	


protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

