// Fill out your copyright notice in the Description page of Project Settings.

#include "EntityBase.h"
#include "Kismet/GameplayStatics.h"
#include <string>

// Sets default values
AEntityBase::AEntityBase()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEntityBase::BeginPlay()
{
    Super::BeginPlay();

    OnTakeAnyDamage.AddDynamic(this, &AEntityBase::EventAnyDamage);
    OnActorBeginOverlap.AddDynamic(this, &AEntityBase::EventBeginOverlap);

    Health = MaxHealth;
}

// Called every frame
void AEntityBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    InvincibilityTimer -= DeltaTime;
}

// Called to bind functionality to input
void AEntityBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FName AEntityBase::GetCurrentAnimation() const
{
    const UAnimInstance* anim = GetMesh()->GetAnimInstance();
    return anim->Montage_GetCurrentSection(anim->GetCurrentActiveMontage());
}

void AEntityBase::OnDamageTaken_Implementation(float DamageAmount)
{
}

bool AEntityBase::OnReceiveCollision_Implementation(AActor* OverlappedActor, AActor* OtherActor)
{
    return true;
}

void AEntityBase::EventAnyDamage(AActor* DamagedActor, float DamageAmount, UDamageType const* DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    Health -= DamageAmount;

    InvincibilityTimer = InvincibilityDuration;

    OnDamageTaken(DamageAmount);
}

void AEntityBase::EventBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (OtherActor == this || !OtherActor->IsA<AEntityBase>())
        return;

    if (!OnReceiveCollision(OverlappedActor, OtherActor))
        return;

    if (InvincibilityTimer > 0.f)
        return;

    UGameplayStatics::ApplyDamage(this, Cast<AEntityBase>(OtherActor)->GetCurrentAnimationDamage(), nullptr, OtherActor, nullptr);
}
