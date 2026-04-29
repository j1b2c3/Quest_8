#include "HP_Bar.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Quest_8/QCharacter.h"

void UHP_Bar::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AQCharacter* MyChar = Cast<AQCharacter>(PC->GetPawn()))
		{
			MyChar->OnHealthChanged.AddDynamic(this, &UHP_Bar::OnHealthChanged);
			OnHealthChanged(MyChar->GetHealth());
		}
	}
}

void UHP_Bar::OnHealthChanged(float NewHealth)
{
	if (UTextBlock* HPText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Health"))))
	{
		HPText->SetText(FText::AsNumber(NewHealth));
	}

	if (UProgressBar* ProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar"))))
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (AQCharacter* MyChar = Cast<AQCharacter>(PC->GetPawn()))
			{
				float HealthPercent = NewHealth / 100.f;
				ProgressBar->SetPercent(HealthPercent);
			}
		}

		FLinearColor BarColor;
		if (NewHealth > 0.0f && NewHealth <= 30.0f)
		{
			BarColor = FLinearColor::Red;
		}
		else if (NewHealth <= 65.0f)
		{
			BarColor = FLinearColor::Yellow;
		}
		else
		{
			BarColor = FLinearColor::Green;
		}
		ProgressBar->SetFillColorAndOpacity(BarColor);
	}

	bool bIsLow = NewHealth <= 30.0f;
	ESlateVisibility NewVis = bIsLow ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

	TArray<FString> ImageNames = {"red1", "red2", "red3", "red4"};
	for (const FString& Name : ImageNames)
	{
		if (UWidget* Img = GetWidgetFromName(FName(*Name)))
		{
			if (Img->GetVisibility() != NewVis)
			{
				Img->SetVisibility(NewVis);
			}
		}
	}
}
