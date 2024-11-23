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
public:
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UFUNCTION(BlueprintCallable, Category="Projectile")
	AAuraProjectile* SpawnProjectile(const FGameplayTag& CombatSocketTag, const FVector& ProjectileTargetLocation);
	
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag, AActor* HomingTarget, const float PitchOverride = 0.f, const FVector OverrideSpawnLocation = FVector::ZeroVector);

	UPROPERTY(EditDefaultsOnly)
	bool bShouldZeroOutPitch = false;

	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 5;

	UPROPERTY(EditDefaultsOnly, Category="Homing")
	float HomingAccelerationMin = 1600.f;

	UPROPERTY(EditDefaultsOnly, Category="Homing")
	float HomingAccelerationMax = 3200.f;

	UPROPERTY(EditDefaultsOnly, Category="Homing")
	bool bHomingProjectiles = true;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float ProjectileSpread = 90.f;
};
