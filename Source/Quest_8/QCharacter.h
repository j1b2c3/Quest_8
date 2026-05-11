#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "QCharacter.generated.h"

class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);


UCLASS(Blueprintable)
class AQCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AQCharacter();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDeath();
	
	UFUNCTION()
	void PlayDeathAnimation();

	void AddHealth(float Amount);
	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const { return Health; }

	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Death")
	FOnCharacterDeath OnCharacterDeath;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathMontage;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
};
