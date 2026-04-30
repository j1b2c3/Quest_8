#pragma once

#include "CoreMinimal.h"
#include "GunDataAsset.h"
#include "InputActionValue.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "QPlayerController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class AQPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AQPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetMoveActionByKeyboard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;
	UPROPERTY()
	UUserWidget* CrosshairWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HPWidgetClass;
	UPROPERTY()
	UUserWidget* HPWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY()
	UUserWidget* HUDWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY()
	UUserWidget* MainMenuWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> DebuffWidgetClass;
	UPROPERTY()
	UUserWidget* DebuffWidgetInstance;

	float CurrentCrosshairSpreadMultiplier;
	float CrosshairSpreadIncrement;
	float MaxCrosshairSpreadMultiplier;
	float FireRate;
	float RecoverTime;
	float LastFireTime;

	void AddDebuffUI(const FString& Type, float Duration);

	float LastSpreadRecoveryTime;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainMenu(bool bIsRestart);
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();

	UFUNCTION(BlueprintPure, Category = "HUD")
	UUserWidget* GetHUDWidget() const;
	void UpdatePlayerRotation();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AActor> LaserClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	USoundBase* LaserSound;

protected:
	uint32 bMoveToMouseCursor : 1;


	virtual void SetupInputComponent() override;
	void ClearWidget();

	virtual void BeginPlay() override;

	void OnInputStarted();
	void EquipGun(UGunDataAsset* NewGunData);
	void RecoverCrosshairSpread();
	void OnFire();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnMoveByKeyboard(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UGunDataAsset* CurrentGunData;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UGunDataAsset* RifleData;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UGunDataAsset* ShotgunData;

private:
	FVector CachedDestination;

	float FollowTime;
};
