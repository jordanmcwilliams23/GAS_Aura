// Copyright Jordan McWilliams


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"


void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

AAuraProjectile* UAuraProjectileSpell::SpawnProjectile(const FGameplayTag& CombatSocketTag, const FVector& ProjectileTargetLocation)
{
	if (const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); !bIsServer) return nullptr;

	if (GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		FVector SocketLocation = FVector::ZeroVector;
		if (CombatSocketTag.MatchesTag(FAuraGameplayTags::Get().CombatSocket))
			SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), CombatSocketTag);
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
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromEffectDefaults();
		Projectile->FinishSpawning(SpawnTransform);
		return Projectile;
	}
	return nullptr;
}

//Don't make this const or it will become BlueprintPure!!
void UAuraProjectileSpell::SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag, AActor* HomingTarget, const float PitchOverride, const FVector OverrideSpawnLocation)
{
	FVector SocketLocation = FVector::ZeroVector;
	if (SocketTag.MatchesTag(FAuraGameplayTags::Get().CombatSocket))
		SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	const FVector LocationToUse = OverrideSpawnLocation.IsZero() ? SocketLocation : OverrideSpawnLocation;
	FRotator Rotation = (TargetLocation - LocationToUse).Rotation();
	Rotation.Pitch = PitchOverride;
	const FVector Forward = Rotation.Vector();
	//NumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());
	const int32 EffectiveNumProjectiles = FMath::Min(NumProjectiles, GetAbilityLevel());
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, EffectiveNumProjectiles);
	FTransform SpawnTransform;
	for (FRotator& Rotator : Rotators)
	{
		SpawnTransform.SetLocation(LocationToUse);
		SpawnTransform.SetRotation(Rotator.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->SetOwner(GetAvatarActorFromActorInfo());
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromEffectDefaults();
		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		} else
		{
			//Needs to be set to a TObjectPtr in order to be GC'd
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(TargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}
		if (bIsRadialDamage)
		{
			Projectile->bRadialDamage = bIsRadialDamage;
			Projectile->RadialDamageInnerRadius = RadialDamageInnerRadius;
			Projectile->RadialDamageOuterRadius = RadialDamageOuterRadius;
		}
		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bHomingProjectiles;
		Projectile->FinishSpawning(SpawnTransform);
	}
}
