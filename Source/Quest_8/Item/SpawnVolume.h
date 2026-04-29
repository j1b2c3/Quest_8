#pragma once

#include "CoreMinimal.h"
#include "ItemSpawnRow.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class QUEST_8_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ASpawnVolume();
	virtual void BeginPlay() override;
	void StartNextWave();
	void ShowAllBlocks();
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void OnBlockOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp,
	                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void OnBlockOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                       int32 OtherBodyIndex);
	void ApplyBlockDamage(AActor* DamagedActor);
	void SpawnRandomItemDuringWave();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	UBoxComponent* SpawningBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	UDataTable* ItemDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int CurrentWave;

	float ItemSpawnInterval;
	float WaveInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float BlockDamagePerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TMap<AActor*, FTimerHandle> OverlapDamageTimers;

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnRandomItem();

	FItemSpawnRow* GetRandomItem() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass) const;
	FVector GetRandomPointInVolume() const;
};
