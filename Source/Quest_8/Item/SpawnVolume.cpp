#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "BaseItem.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;

	CurrentWave = 0;
	ItemSpawnInterval = 0.5f - 0.1 * CurrentWave;
	WaveInterval = 10.0f;

	BlockDamagePerSecond = 10.0f;
}

void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	StartNextWave();
}

void ASpawnVolume::StartNextWave()
{
	if (CurrentWave >= 3)
	{
		return;
	}

	CurrentWave++;

	if (CurrentWave == 0)
	{
		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor && Actor->GetClass()->GetName().Contains(TEXT("BP_Block")))
			{
				Actor->SetActorHiddenInGame(true);

				TArray<UPrimitiveComponent*> PrimitiveComponents;
				Actor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
				for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
				{
					if (UPrimitiveComponent* PC = Cast<UPrimitiveComponent>(PrimComp))
					{
						PC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						PC->SetGenerateOverlapEvents(false);
					}
				}
			}
		}
	}
	else if (CurrentWave >= 1)
	{
		ShowAllBlocks();
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
		                                 FString::Printf(TEXT("%d웨이브"), CurrentWave));
	}

	FTimerHandle ItemSpawnTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ItemSpawnTimerHandle, this,
	                                       &ASpawnVolume::SpawnRandomItemDuringWave, ItemSpawnInterval, true);

	FTimerHandle WaveTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this,
	                                       &ASpawnVolume::StartNextWave, WaveInterval, false);
}

void ASpawnVolume::ShowAllBlocks()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && Actor->GetClass()->GetName().Contains(TEXT("BP_Block")))
		{
			FString WaveTag = CurrentWave == 1 ? "Wave1" : CurrentWave == 2 ? "Wave2" : "Wave3";
			if (Actor->ActorHasTag(FName(*WaveTag)))
			{
				Actor->SetActorHiddenInGame(false);
				if (WaveTag == "Wave1" || WaveTag == "Wave2")
				{
					BlockDamagePerSecond = 10.0f;
				}
				if (WaveTag == "Wave3")
				{
					BlockDamagePerSecond = 20.0f;
				}

				TArray<UPrimitiveComponent*> PrimitiveComponents;
				Actor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
				for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
				{
					if (UPrimitiveComponent* PC = Cast<UPrimitiveComponent>(PrimComp))
					{
						PC->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
						PC->SetGenerateOverlapEvents(true);

						if (!PC->OnComponentBeginOverlap.IsBound())
						{
							PC->OnComponentBeginOverlap.AddDynamic(this, &ASpawnVolume::OnBlockOverlapBegin);
						}
						if (!PC->OnComponentEndOverlap.IsBound())
						{
							PC->OnComponentEndOverlap.AddDynamic(this, &ASpawnVolume::OnBlockOverlapEnd);
						}
					}
				}
			}
		}
	}
}

void ASpawnVolume::OnBlockOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(FName("Player")))
	{
		FTimerHandle DamageTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle,
		                                       FTimerDelegate::CreateUObject(
			                                       this, &ASpawnVolume::ApplyBlockDamage, OtherActor),
		                                       1.0f, true);

		OverlapDamageTimers.Add(OtherActor, DamageTimerHandle);
	}
}

void ASpawnVolume::OnBlockOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OverlapDamageTimers.Contains(OtherActor))
	{
		GetWorld()->GetTimerManager().ClearTimer(OverlapDamageTimers[OtherActor]);
		OverlapDamageTimers.Remove(OtherActor);
	}
}

void ASpawnVolume::ApplyBlockDamage(AActor* DamagedActor)
{
	if (DamagedActor && !DamagedActor->IsPendingKillPending())
	{
		UGameplayStatics::ApplyDamage(DamagedActor, BlockDamagePerSecond, nullptr, this, UDamageType::StaticClass());
	}
}

void ASpawnVolume::SpawnRandomItemDuringWave()
{
	AActor* SpawnedActor = SpawnRandomItem();

	if (SpawnedActor)
	{
		ABaseItem* BaseItem = Cast<ABaseItem>(SpawnedActor);
		if (BaseItem)
		{
			float SpeedMultiplier = 1.0f + (CurrentWave - 1) * 1.0f;

			BaseItem->ItemSpeed *= SpeedMultiplier;
			if (BaseItem->ProjectileMovement)
			{
				BaseItem->ProjectileMovement->InitialSpeed = BaseItem->ItemSpeed;
				BaseItem->ProjectileMovement->Velocity = BaseItem->ProjectileMovement->Velocity.GetSafeNormal() *
					BaseItem->ItemSpeed;
			}
		}
	}
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(FMath::RandRange(-BoxExtent.X, BoxExtent.X), FMath::RandRange(-BoxExtent.Y, BoxExtent.Y),
	                           0.0f);
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}
	return nullptr;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable)
	{
		return nullptr;
	}

	TArray<FItemSpawnRow*> AllRows;

	static const FString ContextString(TEXT("ItemSpawnContext"));
	ItemDataTable->GetAllRows<FItemSpawnRow>(ContextString, AllRows);

	if (AllRows.IsEmpty())
	{
		return nullptr;
	}

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnRate;
		}
	}

	const float RandomValue = FMath::RandRange(0.0f, TotalChance);
	float AccumulatedChance = 0.0f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulatedChance += Row->SpawnRate;
		if (RandomValue <= AccumulatedChance)
		{
			return Row;
		}
	}

	return nullptr;
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass) const
{
	if (!ItemClass)
	{
		return nullptr;
	}

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ItemClass, GetRandomPointInVolume(), FRotator::ZeroRotator);

	return SpawnedActor;
}
