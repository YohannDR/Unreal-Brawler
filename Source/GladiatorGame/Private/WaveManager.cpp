// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveManager.h"
#include "../AiManager.h"
#include "../Enemy.h"
#include <string>
#include <Kismet/GameplayStatics.h>

// Sets default values
AWaveManager::AWaveManager()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWaveManager::BeginPlay()
{
    Super::BeginPlay();

    m_AiManager = Cast<AAiManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAiManager::StaticClass()));
}

// Called every frame
void AWaveManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWaveManager::SpawnBoss() const
{
    const FTransform transform(FRotator::ZeroRotator, SpawnPosition + FVector(0, 0, 100));
    AActor* spawned = GetWorld()->SpawnActor<AActor>(KassadinBlueprint->GeneratedClass, transform);
    AEnemy* enemy = Cast<AEnemy>(spawned);
    m_AiManager->RegisterEnemy(enemy);
}

void AWaveManager::SpawnMinions() const
{
    const uint32_t nbrToSpawn = 5 + m_CurrentWave / 2;
    
    TArray<FTimerHandle> unusedHandles;

    for (uint32_t i = 0; i < nbrToSpawn; i++)
        unusedHandles.Add(FTimerHandle());
    
    for (uint32_t i = 0; i < nbrToSpawn; i++)
    {
        GetWorldTimerManager().SetTimer(unusedHandles[i], [&]()
            {
                const FTransform transform(FRotator::ZeroRotator, SpawnPosition);
                AActor* actor = GetWorld()->SpawnActor<AActor>(MinionBlueprint->GeneratedClass, transform);
                m_AiManager->RegisterEnemy(Cast<AEnemy>(actor));
            }, MinionSpawnDelay + MinionSpawnDelay * i, false);
    }
}

void AWaveManager::EndWave()
{
    m_CurrentWave++;

    if (m_CurrentWave % 3 == 0)
        SpawnBoss();
    else
        SpawnMinions();
}
