// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GunDataAsset.generated.h"

UCLASS()
class QUEST_8_API UGunDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 초기 반동
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float DefaultCrosshairSpreadMultiplier;

	// 반동 증가량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float CrosshairSpreadIncrement;

	// 최대 반동
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float MaxCrosshairSpreadMultiplier;

	// 반동 회복 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float RecoverTime;

	// 연사 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 BulletCount;
};
