#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy_Actor.generated.h"


UCLASS()
class QUEST_8_API AEnemy_Actor : public AActor
{
	GENERATED_BODY()

public:
	AEnemy_Actor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float TraceAngle = 70.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	int32 TraceCount = 10;

public:
	virtual void Tick(float DeltaTime) override;

	void StartAsyncTrace();

	void OnAsyncTraceComplete(const FTraceHandle& Hande, FTraceDatum& Data);
};
