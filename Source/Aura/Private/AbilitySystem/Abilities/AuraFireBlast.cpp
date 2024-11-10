// Copyright Jordan McWilliams


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireball.h"

TArray<AAuraFireball*> UAuraFireBlast::SpawnFireballs()
{
	TArray<AAuraFireball*> Fireballs;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireBolts);
	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		AAuraFireball* Fireball = GetWorld()->SpawnActorDeferred<AAuraFireball>(
			FireballClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Fireball->DamageEffectParams = MakeDamageEffectParamsFromEffectDefaults();
		Fireball->ReturnToActor = GetAvatarActorFromActorInfo();

		FDamageEffectParams ExplosionDamageEffectParams = MakeDamageEffectParamsFromEffectDefaults();
		Fireball->ExplosionDamageParams = ExplosionDamageEffectParams;
		Fireball->ExplosionDamageParams.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Fireball->ExplosionDamageParams.RadialDamageOuterRadius = RadialDamageOuterRadius;
		Fireball->ExplosionDamageParams.KnockbackMagnitude = KnockbackMagnitude;
		Fireball->SetOwner(GetAvatarActorFromActorInfo());
		Fireballs.Add(Fireball);
		Fireball->FinishSpawning(SpawnTransform);
	}
	return Fireballs;
}
