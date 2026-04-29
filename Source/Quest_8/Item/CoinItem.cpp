#include "CoinItem.h"

#include "../QGameState.h"
#include "Engine/World.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (const UWorld* World = GetWorld())
		{
			if (AQGameState* GameState = World->GetGameState<AQGameState>())
			{
				GameState->AddScore(PointValue);
			}
		}
		DestroyItem();
	}
}
