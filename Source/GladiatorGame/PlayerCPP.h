// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntityBase.h"
#include "InputMappingContext.h"
#include "PlayerCPP.generated.h"


UCLASS()
class GLADIATORGAME_API APlayerCPP : public AEntityBase
{
	GENERATED_BODY()

public:
	APlayerCPP();
	void OnDamageTaken_Implementation(float DamageAmount) override;

	
	// Character
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool KeyboardActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Dead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsBlock;

	//Spell
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float QCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float QTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownBetweenQ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownBetweenQTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float QDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int QIndex;

	//Lock
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistanceToLock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistanceKeepLock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetLock;

	//Montage
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ParryMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* QMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* DeathMontage;

	UFUNCTION(BlueprintImplementableEvent)
	void MontageStartingSectionRandom(UAnimMontage* Montage, const FString &name);

	UFUNCTION(BlueprintImplementableEvent)
	void MontageQSpell(UAnimMontage* Montage, const FString& name);
	
	//Sounds
	UPROPERTY(EditAnywhere)
	USoundBase* DeathSound;

protected:
	virtual void Tick(float DeltaTime) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* KeyboardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BlockAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LockAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* QAction;

	void MoveCamera(const FInputActionValue& Value);
	void MovePlayer(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
	void BlockTriggered(const FInputActionValue& Value);
	void BlockCanceledCompleted(const FInputActionValue& Value);
	void QSpell(const FInputActionValue& Value);


	UFUNCTION(BlueprintCallable)
	void LockTarget(FVector start, FVector end);
	
	void Lock(const FInputActionValue& Value);
	void KeepTargetLocked();

private:
	FInputModeGameOnly m_InputMode;
	bool m_FlipFlop = false;
	FTimerHandle TimerHandle;
};