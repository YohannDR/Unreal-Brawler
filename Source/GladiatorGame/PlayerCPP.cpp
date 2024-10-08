// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCPP.h"
#include "EnhancedInputSubsystems.h"
#include <Kismet/GameplayStatics.h>
#include <EnhancedInputComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Enemy.h"


APlayerCPP::APlayerCPP()
{

}

void APlayerCPP::OnDamageTaken_Implementation(float DamageAmount)
{
	if (Health <= 0 && !Dead)
	{
		Dead = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		PlayAnimMontage(DeathMontage);
		UGameplayStatics::PlaySound2D(this, DeathSound);
		UCharacterMovementComponent* characterMovement = GetCharacterMovement();
		if (characterMovement)
			characterMovement->DisableMovement();
	}
}

void APlayerCPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(TargetLock)
		KeepTargetLocked();
	if(QTimer > 0.0f)
		QTimer -= DeltaTime;
	if (CooldownBetweenQTimer > 0.0f)
		CooldownBetweenQTimer -= DeltaTime;

}

void APlayerCPP::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		//Input mapping 
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(MappingContext, 0);
		}
		playerController->SetShowMouseCursor(false);
		//Set input mode to game only
		playerController->SetInputMode(m_InputMode);
		UGameplayStatics::SetViewportMouseCaptureMode(this, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
	}
}

void APlayerCPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(KeyboardAction, ETriggerEvent::Triggered, this, &APlayerCPP::MovePlayer);
		EnhancedInputComponent->BindAction(CameraAction, ETriggerEvent::Triggered, this, &APlayerCPP::MoveCamera);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCPP::Attack);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Triggered, this, &APlayerCPP::BlockTriggered);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Canceled, this, &APlayerCPP::BlockCanceledCompleted);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &APlayerCPP::BlockCanceledCompleted);
		EnhancedInputComponent->BindAction(LockAction, ETriggerEvent::Started, this, &APlayerCPP::Lock);
		EnhancedInputComponent->BindAction(QAction, ETriggerEvent::Started, this, &APlayerCPP::QSpell);
	}
}

void APlayerCPP::MoveCamera(const FInputActionValue& Value)
{
	FVector2D inputValue = Value.Get<FVector2D>();
	inputValue /= 3;

	AddControllerYawInput(inputValue.X);
	AddControllerPitchInput(inputValue.Y);

	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	cameraManager->ViewPitchMin = -20;
	cameraManager->ViewPitchMax = 70;
}

void APlayerCPP::MovePlayer(const FInputActionValue& Value)
{
	FVector2D inputValue = Value.Get<FVector2D>();
	if (KeyboardActive && inputValue != FVector2D::Zero())
	{
		FRotator controlRotation = GetControlRotation();
		FRotator xzRotation = FRotator(0, controlRotation.Yaw, controlRotation.Roll);
		AddMovementInput(UKismetMathLibrary::GetRightVector(controlRotation), inputValue.X);
		AddMovementInput(UKismetMathLibrary::GetForwardVector(xzRotation), inputValue.Y);
	}
}

void APlayerCPP::Attack(const FInputActionValue& Value)
{
	UAnimInstance* playerAnimInstance = GetMesh()->GetAnimInstance();
	if (!playerAnimInstance->IsAnyMontagePlaying() && !Dead)
	{
		FString name("Attack");
		name.AppendInt(FMath::RandHelper(5));
		MontageStartingSectionRandom(AttackMontage, name);
	}
}

void APlayerCPP::BlockTriggered(const FInputActionValue& Value)
{
	KeyboardActive = false;
	UAnimInstance* playerAnimInstance = GetMesh()->GetAnimInstance();
	if (!playerAnimInstance->IsAnyMontagePlaying() && !Dead)
	{
		IsBlock = true;
		playerAnimInstance->Montage_Play(ParryMontage);
	}
}

void APlayerCPP::BlockCanceledCompleted(const FInputActionValue& Value)
{
	UAnimInstance* playerAnimInstance = GetMesh()->GetAnimInstance();
	playerAnimInstance->Montage_StopWithBlendOut(0.2f, ParryMontage);
	IsBlock = false;
	KeyboardActive = true;
}

void APlayerCPP::Lock(const FInputActionValue& Value)
{
	m_FlipFlop ^= true;
	if (m_FlipFlop)
	{
		APlayerCameraManager* camManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		FVector camForward = camManager->GetCameraRotation().Vector() * 120.0f;
		FVector actorPos = GetActorLocation();

		LockTarget(actorPos + camForward, actorPos + (camForward * MaxDistanceToLock));
	}	
	else
	{
		TargetLock = nullptr;
	}
}

void APlayerCPP::QSpell(const FInputActionValue& Value)
{
	UAnimInstance* playerAnimInstance = GetMesh()->GetAnimInstance();
	if (QIndex == 0)
		QTimer = QCooldown;

	if (!playerAnimInstance->IsAnyMontagePlaying() && !Dead && QIndex <= 2)
	{
		FString qSection("Q_");
		qSection.AppendInt(QIndex);
		MontageQSpell(QMontage, qSection);
		GetWorldTimerManager().SetTimer(TimerHandle, [this]() {QIndex = 4; }, CooldownBetweenQ, true, CooldownBetweenQ);
	}
}

void APlayerCPP::LockTarget(FVector start, FVector end)
{
	FHitResult hitResult;
	TArray<AActor*> ignoredArray;
	ignoredArray.Add(this);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	bool bHasHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), start, end, 75.f, ObjectTypesArray, true,
		ignoredArray, EDrawDebugTrace::ForDuration, hitResult, true);


	if (bHasHit)
	{
		AActor* actorHit = hitResult.GetActor();
		AEnemy* enemyHit = Cast<AEnemy>(actorHit);
		if (enemyHit && enemyHit->Health > 0.f)
		{
			TargetLock = actorHit;

		}
	}
}

void APlayerCPP::KeepTargetLocked()
{
	if (TargetLock && m_FlipFlop && Cast<AEnemy>(TargetLock)->Health > 0)
	{
		FVector actorPos = GetActorLocation();
		FVector targetPos = TargetLock->GetActorLocation();
		APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		FRotator newrot = UKismetMathLibrary::FindLookAtRotation(actorPos, targetPos);
		playerController->SetControlRotation(newrot);
		if (UKismetMathLibrary::Vector_Distance(actorPos, targetPos) > MaxDistanceKeepLock)
			TargetLock = nullptr;
	}
	else
	{
		TargetLock = nullptr;
	}
}