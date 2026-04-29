#include "QGameState.h"
#include "QGameInstance.h"
#include "QPlayerController.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Item/BaseItem.h"

AQGameState::AQGameState()
{
	GameScore = 0;
	LevelDuration = 30.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
}


void AQGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(UpdateHUDTimerHandle, this, &AQGameState::UpdateHUD, 0.1f, true);
}


int32 AQGameState::GetGameScore() const
{
	return GameScore;
}

void AQGameState::AddScore(int32 ScoreToAdd)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UQGameInstance* GI_GameInstance = Cast<UQGameInstance>(GameInstance))
		{
			GI_GameInstance->AddToScore(ScoreToAdd);
		}
	}

	GameScore += ScoreToAdd;
}

void AQGameState::StartLevel()
{
	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (PC)
	{
		if (AQPlayerController* QPlayerController = Cast<AQPlayerController>(PC))
		{
			QPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UQGameInstance* QGameInstance = Cast<UQGameInstance>(GameInstance))
		{
			CurrentLevelIndex = QGameInstance->CurrentLevelIndex;
		}
	}

	TArray<AActor*> FoundActors;
	for (AActor* Actor : FoundActors)
	{
		if (ABaseItem* BaseItem = Cast<ABaseItem>(Actor))
		{
			BaseItem->ItemSpeed = (CurrentLevelIndex + 1) * 500.0f;

			if (BaseItem->ProjectileMovement)
			{
				BaseItem->ProjectileMovement->InitialSpeed = BaseItem->ItemSpeed;
				BaseItem->ProjectileMovement->Velocity = BaseItem->ProjectileMovement->Velocity.GetSafeNormal() *
					BaseItem->ItemSpeed;
			}
		}
	}
	GetWorldTimerManager().SetTimer(
		TimerHandle, this, &AQGameState::OnLevelTimeUp, LevelDuration, false
	);
}


void AQGameState::OnLevelTimeUp()
{
	EndLevel();
}

void AQGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().ClearTimer(UpdateHUDTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UQGameInstance* GI_GameInstance = Cast<UQGameInstance>(GameInstance))
		{
			AddScore(GameScore);
			CurrentLevelIndex++;
			GI_GameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}
	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void AQGameState::OnGameOver()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AQPlayerController* QPlayerController = Cast<AQPlayerController>(PC))
		{
			QPlayerController->SetPause(true);
			QPlayerController->ShowMainMenu(true);
		}
	}
}

void AQGameState::UpdateHUD() const
{
	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC)
	{
		return;
	}
	if (PC)
	{
		AQPlayerController* QPlayerController = Cast<AQPlayerController>(PC);
		if (!QPlayerController)
		{
			return;
		}
		if (QPlayerController)
		{
			if (UUserWidget* HUDWidget = QPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(TimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time : %.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UQGameInstance* GI_GameInstance = Cast<UQGameInstance>(GameInstance))
						{
							ScoreText->SetText(
								FText::FromString(FString::Printf(TEXT("Score : %d"), GI_GameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndex = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndex->SetText(FText::FromString(FString::Printf(TEXT("Level : %d"), CurrentLevelIndex + 1)));
				}
			}
		}
	}
}
