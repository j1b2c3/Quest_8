#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Crosshair.generated.h"


UCLASS()
class QUEST_8_API UCrosshair : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairImage;

	void UpdateCrosshairSpread(float Spread);
};
