#include "DebuffUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UDebuffUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bDebuffActive && DebuffBar)
	{
		UpdateDebuffTimer();
	}
}

void UDebuffUI::ShowDebuff(const FString& DebuffType, float Duration)
{
	if (!DebuffBar || !DebuffText)
	{
		return;
	}

	if (DebuffTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DebuffTimerHandle);
	}

	if (DebuffType == TEXT("Speed"))
	{
		DebuffText->SetText(FText::FromString(TEXT("이동 속도 감소!")));
	}
	else if (DebuffType == TEXT("Cooldown"))
	{
		DebuffText->SetText(FText::FromString(TEXT("발사 속도 감소!")));
	}

	DebuffDuration = Duration;
	DebuffStartTime = GetWorld()->GetTimeSeconds();
	bDebuffActive = true;

	DebuffBar->SetPercent(1.0f);

	GetWorld()->GetTimerManager().SetTimer(DebuffTimerHandle, this, &UDebuffUI::HideDebuff, Duration, false);
}

void UDebuffUI::HideDebuff()
{
	if (DebuffBar && DebuffText)
	{
		RemoveFromParent();

		GetWorld()->GetTimerManager().ClearTimer(DebuffTimerHandle);
	}
}

void UDebuffUI::UpdateDebuffTimer()
{
	if (!bDebuffActive || !DebuffBar)
	{
		return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();
	float ElapsedTime = CurrentTime - DebuffStartTime;
	float RemainingTime = DebuffDuration - ElapsedTime;

	if (RemainingTime > 0.0f)
	{
		float Percentage = RemainingTime / DebuffDuration;
		DebuffBar->SetPercent(Percentage);
	}
	else
	{
		DebuffBar->SetPercent(0.0f);
	}
}
