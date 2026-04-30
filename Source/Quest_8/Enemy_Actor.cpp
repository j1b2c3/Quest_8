#include "Enemy_Actor.h"


AEnemy_Actor::AEnemy_Actor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemy_Actor::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StartAsyncTrace();
}

void AEnemy_Actor::StartAsyncTrace()
{
	FTraceDelegate TraceDelegate;
	TraceDelegate.BindUObject(this, &AEnemy_Actor::OnAsyncTraceComplete);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FVector StartLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();

	// 70도를 TraceCount만큼 나누어 발사
	float HalfAngle = TraceAngle / 2.0f;
	float AngleStep = TraceAngle / (TraceCount - 1);

	for (int32 i = 0; i < TraceCount; i++)
	{
		// 현재 인덱스에 맞춰 회전 각도 계산 (-35도 ~ +35도)
		float CurrentAngle = -HalfAngle + (AngleStep * i);

		// Z축(UpVector)을 기준으로 전방 벡터를 회전시킴
		FVector TraceDirection = ForwardVector.RotateAngleAxis(CurrentAngle, FVector::UpVector);
		FVector EndLocation = StartLocation + (TraceDirection * 1000.f);

		GetWorld()->AsyncLineTraceByChannel(
			EAsyncTraceType::Single,
			StartLocation,
			EndLocation,
			ECC_Visibility,
			Params,
			FCollisionResponseParams::DefaultResponseParam,
			&TraceDelegate
		);
	}
}

void AEnemy_Actor::OnAsyncTraceComplete(const FTraceHandle& Handle, FTraceDatum& Data)
{
	if (Data.OutHits.Num() > 0)
	{
		const FHitResult& Hit = Data.OutHits[0];
		if (Hit.GetActor() && Hit.GetActor()->ActorHasTag("Player"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, TEXT("Player Detected!"));
		}

		DrawDebugLine(GetWorld(), Data.Start, Hit.ImpactPoint, FColor::Green, false, 0.1f, 0, 1.f);
	}
	else
	{
		DrawDebugLine(GetWorld(), Data.Start, Data.End, FColor::Red, false, 0.1f, 0, 0.5f);
	}
}
