#include "QCharacter.h"

#include "QGameState.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"


class UTextBlock;

AQCharacter::AQCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	MaxHealth = 100.f;
	Health = MaxHealth;
}


void AQCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnHealthChanged.Broadcast(Health);

	OnCharacterDeath.AddDynamic(this, &AQCharacter::PlayDeathAnimation);
}

void AQCharacter::OnDeath()
{
	AQGameState* GameState = GetWorld() ? GetWorld()->GetGameState<AQGameState>() : nullptr;
	if (GameState)
	{
		GameState->OnGameOver();
	}
}

void AQCharacter::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}
}

void AQCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health);
}

float AQCharacter::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, AController* EventInstigator,
                              AActor* DamageCauser)
{
	if (Health <= 0.0f)
	{
		return 0.0f;
	}

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - ActualDamage, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health);

	if (Health <= 0.0f)
	{
		OnCharacterDeath.Broadcast();

		FTimerHandle DeathTimerHandle;
		GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AQCharacter::OnDeath, 1.0f, false);
	}

	return ActualDamage;
}
