// Fill out your copyright notice in the Description page of Project Settings.


#include "AiManager.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Private/WaveManager.h"
#include <string>
#include <Kismet/GameplayStatics.h>
#include <Blueprint/AIBlueprintHelperLibrary.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AAiManager::AAiManager()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAiManager::BeginPlay()
{
    Super::BeginPlay();

    m_Idle = false;

    TArray<AActor*> actors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), actors);

    m_WaveManager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass()));

    for (AActor* a : actors)
    {
        m_Enemies.Add(Cast<AEnemy>(a));
    }

    m_DecisionTimer = DecisionDelay;
}

// Called every frame
void AAiManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (m_Idle)
        return;

    if (m_Enemies.Num() == 0)
    {
        m_Idle = true;
        m_WaveManager->EndWave();
        return;
    }

    CheckEnemyShouldAttack(DeltaTime);
    ComputeEnemyPositions();
}

void AAiManager::CheckEnemyShouldAttack(float DeltaTime)
{
    m_DecisionTimer -= DeltaTime;

    if (m_DecisionTimer > 0)
        return;

    m_DecisionTimer = DecisionDelay;

    int32 id = FMath::RandRange(0, m_Enemies.Num() - 1);

    m_Enemies[id]->StartAttackPlayer();
}

void AAiManager::ComputeEnemyPositions()
{
    m_Enemies.RemoveAll([&](const AEnemy* e) { return !e || e->IsDead(); });

    const float dAngle = 360.f / m_Enemies.Num();

    ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    const FVector playerPos = player->GetActorLocation();

    const FVector radius = FVector(StrafeRadius, 0, 0);

    for (size_t i = 0; i < m_Enemies.Num(); i++)
    {
        if (m_Enemies[i]->IsAttacking())
            continue;

        const FVector rotated = radius.RotateAngleAxis(dAngle * i, FVector(0, 0, 1));

        AAIController* const ai = m_Enemies[i]->AiController;
        ai->MoveToLocation(playerPos + rotated);
        ai->SetFocus(player);
    }
}

void AAiManager::RegisterEnemy(AEnemy* const enemy)
{
    m_Idle = false;

    m_Enemies.Add(enemy);
}
