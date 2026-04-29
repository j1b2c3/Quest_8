#include "QGameInstance.h"

UQGameInstance::UQGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void UQGameInstance::AddToScore(int32 ScoreToAdd)
{
	TotalScore += ScoreToAdd;
}
