// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <string>
#include <Kismet/GameplayStatics.h>
#include <Blueprint/AIBlueprintHelperLibrary.h>

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AiController = UAIBlueprintHelperLibrary::GetAIController(this);
	m_CurrentPose = Pose::IDLE;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (m_CurrentPose)
	{
		case Pose::IDLE:
			break;

		case Pose::GOING_TO_TARGET:
			m_ChaseTimer -= DeltaTime;

			if (m_ChaseTimer <= 0)
			{
				// Couldn't get to player, reset to idle
				m_CurrentPose = Pose::IDLE;
			}

			if (AiController->MoveToLocation(m_AttackDestination, AttackRange) == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				m_MontageLength = GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
				m_CurrentPose = Pose::ATTACKING;
			}
			break;

		case Pose::ATTACKING:
			m_MontageLength -= DeltaTime;

			if (m_MontageLength <= 0)
			{
				m_CurrentPose = Pose::IDLE;
			}
			break;

		case Pose::DYING:
			m_MontageLength -= DeltaTime;

			if (m_MontageLength <= 0)
			{
				m_CurrentPose = Pose::DEAD;
			}
			break;

		case Pose::DEAD:
			break;
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::StartAttackPlayer()
{
	if (m_CurrentPose != Pose::IDLE)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, "attacking");

	m_CurrentPose = Pose::GOING_TO_TARGET;
	m_ChaseTimer = ChaseMaxTime;

	ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	m_AttackDestination.X = player->GetActorLocation().X;
	m_AttackDestination.Y = player->GetActorLocation().Y;
	m_AttackDestination.Z = GetActorLocation().Z;
}

bool AEnemy::IsAttacking() const
{
	return m_CurrentPose != Pose::IDLE;
}

bool AEnemy::IsDead() const
{
	return m_CurrentPose == Pose::DYING || m_CurrentPose == Pose::DEAD;
}

void AEnemy::OnDamageTaken_Implementation(float DamageAmount)
{
	if (IsDead())
		return;

	if (Health <= 0)
	{
		m_CurrentPose = Pose::DYING;
		m_MontageLength = GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);

		AiController->SetFocus(nullptr);
		AiController->MoveToLocation(GetActorLocation());

		SetActorEnableCollision(false);
	}
}

void AEnemy::OnAttackParried_Implementation(AActor* Player)
{
	UGameplayStatics::ApplyDamage(this, 1.f, nullptr, Player, nullptr);
}
