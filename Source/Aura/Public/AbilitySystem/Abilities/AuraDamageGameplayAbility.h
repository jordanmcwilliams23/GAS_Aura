// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
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
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage", meta=(Categories="Damage"))
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat Damage;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat DebuffChance;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat DebuffDamage;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat DebuffFrequency;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat DebuffDuration;
};
