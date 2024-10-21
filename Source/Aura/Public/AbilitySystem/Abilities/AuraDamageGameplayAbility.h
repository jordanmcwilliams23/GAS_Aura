// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void CauseDamage(AActor* TargetActor);

	int32 GetRoundedDamageAtLevel(int32 Level) const;

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromEffectDefaults(
		AActor* TargetActor = nullptr,
		FVector RadialDamageOrigin = FVector::ZeroVector,
		bool bInOverrideKnockbackDirection = false,
		FVector OverrideKnockbackDirection = FVector::ZeroVector,
		bool bInOverrideDeathImpulseDirection = false,
		FVector OverrideDeathImpulseDirection = FVector::ZeroVector) const;

	UFUNCTION(BlueprintPure)
	float GetDamageAtCurrentLevel() const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage", meta=(Categories="Damage"))
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat Damage = 20.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffChance = 25.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffDuration = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FVector2f DeathImpulseMagnitudeMinMax = FVector2f(3000.f, 6000.f);

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockbackMagnitude = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockbackChance = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	bool bIsRadialDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float RadialDamageInnerRadius = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float RadialDamageOuterRadius = 0.f;
};
