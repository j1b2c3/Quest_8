// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "DebuffUI.generated.h"

UCLASS()
class QUEST_8_API UDebuffUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void ShowDebuff(const FString& DebuffType, float Duration);
	void HideDebuff();
	void UpdateDebuffTimer();
	bool bDebuffActive;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DebuffText;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* DebuffBar;

	FTimerHandle DebuffTimerHandle;
	float DebuffDuration;
	double DebuffStartTime;
};
