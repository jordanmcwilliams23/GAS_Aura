// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;
/**
 * 
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UFUNCTION(BlueprintCallable, Category="Projectile")
	AAuraProjectile* SpawnProjectile(const FGameplayTag& CombatSocketTag, const FVector& ProjectileTargetLocation);

	UPROPERTY(EditDefaultsOnly)
	bool bShouldZeroOutPitch = false;
};
