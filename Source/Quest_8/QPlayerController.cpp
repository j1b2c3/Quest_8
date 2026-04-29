#include "QPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "QGameInstance.h"
#include "QGameState.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "UI/DebuffUI.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AQPlayerController::AQPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
	LastSpreadRecoveryTime = 0.f;
}

void AQPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TempHandle;
	GetWorldTimerManager().SetTimer(TempHandle, [this]()
	{
		if (IsValid(this))
		{
			FString CurrentMapName = GetWorld()->GetMapName();
			if (CurrentMapName.Contains("UILevel")) { ShowMainMenu(false); }
			else { ShowGameHUD(); }
		}
	}, 0.1f, false);

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("UILevel"))
	{
		ShowMainMenu(false);
	}
	else
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (AQPlayerController* QPlayerController = Cast<AQPlayerController>(PlayerController))
			{
				QPlayerController->ShowGameHUD();
			}
		}
	}

	FTimerHandle CrosshairSpreadRecoveryTimer;
	GetWorldTimerManager().SetTimer(CrosshairSpreadRecoveryTimer, this, &AQPlayerController::RecoverCrosshairSpread,
	                                0.5f, true);
}

void AQPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(SetMoveAction, ETriggerEvent::Started, this,
		                                   &AQPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetMoveAction, ETriggerEvent::Triggered, this,
		                                   &AQPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetMoveAction, ETriggerEvent::Completed, this,
		                                   &AQPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetMoveAction, ETriggerEvent::Canceled, this,
		                                   &AQPlayerController::OnSetDestinationReleased);

		EnhancedInputComponent->BindAction(SetMoveActionByKeyboard, ETriggerEvent::Started, this,
		                                   &AQPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetMoveActionByKeyboard, ETriggerEvent::Triggered, this,
		                                   &AQPlayerController::OnMoveByKeyboard);
		EnhancedInputComponent->BindAction(SetMoveActionByKeyboard, ETriggerEvent::Completed, this,
		                                   &AQPlayerController::StopMovement);
		EnhancedInputComponent->BindAction(SetMoveActionByKeyboard, ETriggerEvent::Canceled, this,
		                                   &AQPlayerController::StopMovement);

		EnhancedInputComponent->BindAction(SetShootAction, ETriggerEvent::Started, this,
		                                   &AQPlayerController::OnFire);
		EnhancedInputComponent->BindAction(SetShootAction, ETriggerEvent::Triggered, this,
		                                   &AQPlayerController::OnFire);
	}

}

void AQPlayerController::ClearWidget()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
	}
	if (CrosshairWidgetInstance)
	{
		CrosshairWidgetInstance->RemoveFromParent();
	}
	if (HPWidgetInstance)
	{
		HPWidgetInstance->RemoveFromParent();
	}
	if (DebuffWidgetInstance)
	{
		DebuffWidgetInstance->RemoveFromParent();
	}
}

void AQPlayerController::OnInputStarted()
{
	StopMovement();
}

void AQPlayerController::RecoverCrosshairSpread()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastFireTime >= 0.5f)
	{
		CurrentCrosshairSpreadMultiplier = FMath::Max(CurrentCrosshairSpreadMultiplier - 0.2f, 1.0f);
	}
}

void AQPlayerController::OnFire()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastFireTime < FireCooldown)
	{
		return;
	}
	LastFireTime = CurrentTime;

	if (!GetWorld())
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	if (LaserSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaserSound, ControlledPawn->GetActorLocation());
	}

	CurrentCrosshairSpreadMultiplier = FMath::Min(
		CurrentCrosshairSpreadMultiplier + CrosshairSpreadIncrement,
		MaxCrosshairSpreadMultiplier
	);

	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);

	FVector WorldLocation, WorldDirection;
	if (CrosshairWidgetInstance && IsValid(CrosshairWidgetInstance))
	{
		const FVector2D CrosshairSize = CrosshairWidgetInstance->GetDesiredSize();
		const float CrosshairHalfWidth = (CrosshairSize.X / 2.0f) * CurrentCrosshairSpreadMultiplier;
		const float CrosshairHalfHeight = (CrosshairSize.Y / 2.0f) * CurrentCrosshairSpreadMultiplier;

		const float RandomOffsetX = FMath::RandRange(-CrosshairHalfWidth, CrosshairHalfWidth);
		const float RandomOffsetY = FMath::RandRange(-CrosshairHalfHeight, CrosshairHalfHeight);

		const float AdjustedMouseX = MouseX + RandomOffsetX;
		const float AdjustedMouseY = MouseY + RandomOffsetY;

		DeprojectScreenPositionToWorld(AdjustedMouseX, AdjustedMouseY, WorldLocation, WorldDirection);
	}
	else
	{
		DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);
	}

	FHitResult Hit;
	const FVector TraceStart = WorldLocation;
	const FVector TraceEnd = WorldLocation + (WorldDirection * 100000.0f);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility);

	FVector TargetLocation = bHit ? Hit.ImpactPoint : TraceEnd;

	FVector PawnLocation = ControlledPawn->GetActorLocation();
	PawnLocation.Z += 50.0f;
	TargetLocation.Z = PawnLocation.Z;

	const FVector LaserDirection = (TargetLocation - PawnLocation).GetSafeNormal();
	const FVector LaserStartLocation = PawnLocation + (LaserDirection * 50.0f);
	const FRotator SpawnActorRotation = LaserDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = ControlledPawn;
	SpawnParams.Instigator = ControlledPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(LaserClass, LaserStartLocation, SpawnActorRotation, SpawnParams);
}

void AQPlayerController::ShowMainMenu(bool bIsRestart)
{
	ClearWidget();
	if (!MainMenuWidgetClass)
	{
		return;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();
			bShowMouseCursor = true;
		}

		UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText")));
		if (ButtonText)
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(
				MainMenuWidgetInstance->GetWidgetFromName(TEXT("TotalScoreText"))))
			{
				if (UQGameInstance* QGameInstance = Cast<UQGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(
						FText::FromString(FString::Printf(TEXT("Total Score : %d"), QGameInstance->TotalScore)));
				}
			}
		}
	}
}

void AQPlayerController::AddDebuffUI(const FString& Type, float Duration)
{
	UVerticalBox* DebuffBox = Cast<UVerticalBox>(HUDWidgetInstance->GetWidgetFromName(TEXT("DebuffBox")));

	if (DebuffWidgetClass && DebuffBox)
	{
		UDebuffUI* NewDebuff = CreateWidget<UDebuffUI>(this, DebuffWidgetClass);
		if (NewDebuff)
		{
			DebuffBox->InsertChildAt(0, NewDebuff);
			NewDebuff->ShowDebuff(Type, Duration);
		}
	}
}

void AQPlayerController::ShowGameHUD()
{
	if (!GetWorld())
	{
		return;
	}

	if (CrosshairWidgetClass && !CrosshairWidgetInstance)
	{
		CrosshairWidgetInstance = CreateWidget<UUserWidget>(this, CrosshairWidgetClass);
		CrosshairWidgetInstance->AddToViewport();
		CrosshairWidgetInstance->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));

		bShowMouseCursor = false;
	}

	if (HPWidgetClass && !HPWidgetInstance)
	{
		HPWidgetInstance = CreateWidget<UUserWidget>(this, HPWidgetClass);
		HPWidgetInstance->AddToViewport();
	}

	if (HUDWidgetClass && !HUDWidgetInstance)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}

		AQGameState* QGameState = GetWorld() ? GetWorld()->GetGameState<AQGameState>() : nullptr;
		if (QGameState)
		{
			QGameState->UpdateHUD();
		}
	}
}


void AQPlayerController::OnSetDestinationTriggered()
{
	FollowTime += GetWorld()->GetDeltaSeconds();
	FHitResult Hit;
	bool bHitSuccessful = false;
	bHitSuccessful = GetHitResultUnderCursor(ECC_Visibility, true, Hit);
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AQPlayerController::OnSetDestinationReleased()
{
	if (FollowTime <= ShortPressThreshold)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator,
		                                               FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}
	FollowTime = 0.f;
}

void AQPlayerController::OnMoveByKeyboard(const FInputActionValue& Value)
{
	StopMovement();
	FVector2D MovementVector = Value.Get<FVector2D>();
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		const FVector ForwardDirection = FVector(1, 0, 0);
		const FVector RightDirection = FVector(0, 1, 0);
		ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AQPlayerController::StartGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("Level1"));

	if (UQGameInstance* QGameInstance = Cast<UQGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		QGameInstance->CurrentLevelIndex = 0;
		QGameInstance->TotalScore = 0;
	}

	SetPause(false);
}

UUserWidget* AQPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void AQPlayerController::UpdatePlayerRotation()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	FHitResult Hit;
	bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (bHit)
	{
		FVector PawnLocation = ControlledPawn->GetActorLocation();
		FVector TargetLocation = Hit.ImpactPoint;

		TargetLocation.Z = PawnLocation.Z;

		FVector DirectionToMouse = (TargetLocation - PawnLocation).GetSafeNormal();

		if (!DirectionToMouse.IsNearlyZero())
		{
			FRotator TargetRotation = DirectionToMouse.Rotation();
			ControlledPawn->SetActorRotation(TargetRotation);
		}
	}
}

void AQPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdatePlayerRotation();

	if (CrosshairWidgetInstance && IsValid(CrosshairWidgetInstance))
	{
		float MouseX, MouseY;
		GetMousePosition(MouseX, MouseY);
		CrosshairWidgetInstance->SetPositionInViewport(FVector2D(MouseX, MouseY));
		CrosshairWidgetInstance->SetRenderScale(FVector2D(CurrentCrosshairSpreadMultiplier, 1.0f));
	}
}
