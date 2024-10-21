// Copyright Jordan McWilliams


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Spec, DamageType, Damage.GetValueAtLevel(GetAbilityLevel()));
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

int32 UAuraDamageGameplayAbility::GetRoundedDamageAtLevel(const int32 Level) const
{
	return Damage.AsInteger(Level);
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromEffectDefaults(AActor* TargetActor,
	FVector RadialDamageOrigin, bool bInOverrideKnockbackDirection, FVector OverrideKnockbackDirection,
	bool bInOverrideDeathImpulseDirection, FVector OverrideDeathImpulseDirection) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceASC = GetAbilitySystemComponentFromActorInfo();
	Params.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(Params.AbilityLevel = GetAbilityLevel());
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotation.Pitch = 45.f;
		if (bInOverrideKnockbackDirection)
		{
			OverrideKnockbackDirection.Normalize();
			Params.KnockbackForce = OverrideKnockbackDirection * KnockbackMagnitude;
		} else
		{
			Params.KnockbackForce = Rotation.Vector() * Params.KnockbackMagnitude;
		}
		
		if (bInOverrideDeathImpulseDirection)
		{
			OverrideKnockbackDirection.Normalize();
			Params.DeathImpulse = OverrideKnockbackDirection * Params.GetDeathImpulseMagnitude();
		} else
		{
			Params.DeathImpulse = Rotation.Vector() * Params.GetDeathImpulseMagnitude();
		}
	}
	
	Params.KnockbackMagnitude = KnockbackMagnitude;
	Params.KnockbackChance = KnockbackChance;
	
	Params.DeathImpulseMagnitudeMinMax = DeathImpulseMagnitudeMinMax;
	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageOrigin = RadialDamageOrigin;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
	}
	return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtCurrentLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}
