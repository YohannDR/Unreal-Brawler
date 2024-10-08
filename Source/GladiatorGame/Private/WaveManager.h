// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

class AAiManager;

UCLASS()
class AWaveManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaveManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FVector SpawnPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	float MinionSpawnDelay;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	UBlueprint* MinionBlueprint;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	UBlueprint* KassadinBlueprint;

private:
	uint32_t m_CurrentWave;
	AAiManager* m_AiManager;

	void SpawnBoss() const;
	void SpawnMinions() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void EndWave();
};
