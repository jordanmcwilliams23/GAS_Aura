// Copyright Jordan McWilliams


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

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
