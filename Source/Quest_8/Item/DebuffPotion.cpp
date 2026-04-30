#include "DebuffPotion.h"
#include "../QCharacter.h"
#include "../QplayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

ADebuffPotion::ADebuffPotion()
{
	ItemType = "Debuff";
}

void ADebuffPotion::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if (!Activator || !Activator->ActorHasTag("Player"))
	{
		return;
	}

	AQCharacter* Chara = Cast<AQCharacter>(Activator);
	if (!Chara)
	{
		return;
	}

	bool bSlowMovement = FMath::RandBool();

	if (bSlowMovement)
	{
		if (SpeedTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(SpeedTimerHandle);
			if (Chara && Chara->GetCharacterMovement())
			{
				float RestoredSpeed = FMath::Min(Chara->GetCharacterMovement()->MaxWalkSpeed * 2.0f, OriginalSpeed);
				Chara->GetCharacterMovement()->MaxWalkSpeed = RestoredSpeed;
			}
		}

		if (Chara->GetCharacterMovement())
		{
			Chara->GetCharacterMovement()->MaxWalkSpeed *= 0.5f;
		}

		TWeakObjectPtr<AQCharacter> WeakChara(Chara);

		GetWorld()->GetTimerManager().SetTimer(SpeedTimerHandle, [WeakChara, this]()
		{
			if (WeakChara.IsValid() && WeakChara->GetCharacterMovement())
			{
				float RestoredSpeed = FMath::Min(WeakChara->GetCharacterMovement()->MaxWalkSpeed * 2.0f, OriginalSpeed);
				WeakChara->GetCharacterMovement()->MaxWalkSpeed = RestoredSpeed;
			}
		}, 5.0f, false);

		AQPlayerController* PC = Cast<AQPlayerController>(Chara->GetController());
		if (PC)
		{
			PC->AddDebuffUI(TEXT("Speed"), 5.0f);
		}
	}
	else
	{
		AQPlayerController* PC = Cast<AQPlayerController>(Chara->GetController());
		if (PC)
		{
			if (CooldownTimerHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
				if (PC)
				{
					float RestoredCooldown = FMath::Max(PC->FireRate * 0.5f, OriginalCooldown);
					PC->FireRate = RestoredCooldown;
				}
			}

			PC->FireRate *= 2.0f;

			TWeakObjectPtr<AQPlayerController> WeakPC(PC);

			GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, [WeakPC, this]()
			{
				if (WeakPC.IsValid())
				{
					float RestoredCooldown = FMath::Max(WeakPC->FireRate * 0.5f, OriginalCooldown);
					WeakPC->FireRate = RestoredCooldown;
				}
			}, 5.0f, false);

			PC->AddDebuffUI(TEXT("Cooldown"), 5.0f);
		}
	}
	DestroyItem();
}
