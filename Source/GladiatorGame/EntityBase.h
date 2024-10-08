// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EntityBase.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class GLADIATORGAME_API AEntityBase : public ACharacter
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
    float InvincibilityTimer;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
    float InvincibilityDuration = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
    float AttackDamage = 1.f;

    // Sets default values for this character's properties
    AEntityBase();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:	
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FName GetCurrentAnimation() const;

    UFUNCTION(BlueprintImplementableEvent)
    float GetCurrentAnimationDamage() const;

    UFUNCTION(BlueprintNativeEvent)
    void OnDamageTaken(float DamageAmount);
    virtual void OnDamageTaken_Implementation(float DamageAmount);

    UFUNCTION(BlueprintNativeEvent, BlueprintPure)
    bool OnReceiveCollision(AActor* OverlappedActor, AActor* OtherActor);
    virtual bool OnReceiveCollision_Implementation(AActor* OverlappedActor, AActor* OtherActor);

private:
    UFUNCTION()
    void EventAnyDamage(AActor* DamagedActor, float DamageAmount, UDamageType const* DamageEvent, AController* EventInstigator, AActor* DamageCauser);

    UFUNCTION()
    void EventBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
