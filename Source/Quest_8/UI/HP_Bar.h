#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HP_Bar.generated.h"


UCLASS()
class QUEST_8_API UHP_Bar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void OnHealthChanged(float NewHealth);
};
