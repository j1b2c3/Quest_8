#include "Laser.h"

ALaser::ALaser()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALaser::BeginPlay()
{
	Super::BeginPlay();
}

void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Start = GetActorLocation();
	FVector End = Start + (GetVelocity() * DeltaTime);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		Destroy();
	}
}
