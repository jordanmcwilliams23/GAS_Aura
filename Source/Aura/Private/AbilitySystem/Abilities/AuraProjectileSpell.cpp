// Copyright Jordan McWilliams


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"


void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

AAuraProjectile* UAuraProjectileSpell::SpawnProjectile(const FGameplayTag& CombatSocketTag, const FVector& ProjectileTargetLocation)
{
	if (const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); !bIsServer) return nullptr;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), CombatSocketTag);
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		 if (bShouldZeroOutPitch) Rotation.Pitch = 0.f;
		SpawnTransform.SetRotation(Rotation.Quaternion());
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
		ContextHandle.SetAbility(this);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		ContextHandle.AddHitResult(HitResult);
		ContextHandle.AddActors(Actors);
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);

		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageType, ScaledDamage);
		Projectile->SetOwner(GetAvatarActorFromActorInfo());
		Projectile->DamageEffectSpecHandle = SpecHandle;
		Projectile->FinishSpawning(SpawnTransform);
		return Projectile;
	}
	return nullptr;
}
