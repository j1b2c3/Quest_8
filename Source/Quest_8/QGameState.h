#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "QGameState.generated.h"


UCLASS()
class QUEST_8_API AQGameState : public AGameState
{
	GENERATED_BODY()

public:
	AQGameState();

	void EndLevel();
	void OnLevelTimeUp();
	void StartLevel();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game")
	int32 GameScore;


	FTimerHandle TimerHandle;
	FTimerHandle UpdateHUDTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	TArray<FName> LevelMapNames;


	UFUNCTION(BlueprintPure, Category = "Game")
	int32 GetGameScore() const;
	UFUNCTION(BlueprintCallable, Category = "Game")
	void AddScore(int32 ScoreToAdd);
	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnGameOver();

	void UpdateHUD() const;
};
