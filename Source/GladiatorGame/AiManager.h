// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enemy.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AiManager.generated.h"

class AWaveManager;

UCLASS()
class GLADIATORGAME_API AAiManager : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	float DecisionDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	float StrafeRadius;

	// Sets default values for this actor's properties
	AAiManager();

private:
	AWaveManager* m_WaveManager;
	bool m_Idle;

protected:
	float m_DecisionTimer;

	TArray<AEnemy*> m_Enemies;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CheckEnemyShouldAttack(float DeltaTime);
	void ComputeEnemyPositions();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RegisterEnemy(AEnemy* const enemy);
};
