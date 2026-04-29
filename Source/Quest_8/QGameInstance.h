#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "QGameInstance.generated.h"

UCLASS()
class QUEST_8_API UQGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UQGameInstance();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game")
	int32 TotalScore;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game")
	int32 CurrentLevelIndex;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void AddToScore(int32 ScoreToAdd);
};
