// Copyright Jordan McWilliams


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass: Abilities)
	{
		FGameplayAbilitySpec Spec(AbilityClass, 1.f);
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(Spec.Ability))
		{
			Spec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupGameplayTag);
		}
		GiveAbility(Spec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& Tag)
{
	if (!Tag.IsValid()) return;

	for (FGameplayAbilitySpec& Spec: GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(Tag))
		{
			AbilitySpecInputPressed(Spec);
			if (Spec.IsActive()) continue;
			TryActivateAbility(Spec.Handle);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& Tag)
{
	if (!Tag.IsValid()) return;

	for (FGameplayAbilitySpec& Spec: GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(Tag))
		{
			AbilitySpecInputReleased(Spec);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) const
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
