#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "DebuffPotion.generated.h"

UCLASS()
class QUEST_8_API ADebuffPotion : public ABaseItem
{
	GENERATED_BODY()

public:
	ADebuffPotion();

	virtual void ActivateItem(AActor* Activator) override;
	FTimerHandle SpeedTimerHandle;
	FTimerHandle CooldownTimerHandle;

	float OriginalSpeed = 600.f;
	float OriginalCooldown = 0.2f;
};
