// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UDamageTextComponent;
struct FInputActionInstance;
struct FGameplayTag;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
class ITargetInterface;
class UAuraAbilitySystemComponent;
class USplineComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE FHitResult GetCursorHit() {return CursorHit;}

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageNumber, AActor* TargetActor);
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() {bShiftKeyDown = true;}

	void ShiftReleased() {bShiftKeyDown = false;}
	bool bShiftKeyDown = false;

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	void Move(const struct FInputActionValue& InputActionValue);
	void CursorTrace();
	TScriptInterface<ITargetInterface> LastActor;
	TScriptInterface<ITargetInterface> CurrentActor;

	void AbilityInputTagPressed(const FInputActionInstance& Instance, FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(const FInputActionValue& Value, FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	void AutoRun();
	
	FHitResult CursorHit;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};
