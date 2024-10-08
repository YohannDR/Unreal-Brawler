// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntityBase.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class GLADIATORGAME_API AEnemy : public AEntityBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere)
	UAnimMontage* DeathMontage;

	AAIController* AiController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
	float ChaseMaxTime;

	// Sets default values for this character's properties
	AEnemy();

private:
	enum class Pose : uint32_t
	{
		IDLE,
		GOING_TO_TARGET,
		ATTACKING,
		DYING,
		DEAD,
	};

	FVector m_AttackDestination;
	Pose m_CurrentPose;
	float m_MontageLength;
	float m_ChaseTimer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartAttackPlayer();
	bool IsAttacking() const;
	bool IsDead() const;
	
	void OnDamageTaken_Implementation(float DamageAmount) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnAttackParried(AActor* Player);
	virtual void OnAttackParried_Implementation(AActor* Player);
};
