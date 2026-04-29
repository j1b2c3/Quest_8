#include "HealItem.h"

#include "../QCharacter.h"

AHealItem::AHealItem()
{
	HealAmount = 30.0f;
	ItemType = "Heal";
}

void AHealItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AQCharacter* Chara = Cast<AQCharacter>(Activator))
		{
			Chara->AddHealth(HealAmount);
		}
		DestroyItem();
	}
}
