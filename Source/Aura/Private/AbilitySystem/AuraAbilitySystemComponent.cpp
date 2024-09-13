// Copyright Jordan McWilliams


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"

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
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast(this);
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass: PassiveAbilities)
	{
		FGameplayAbilitySpec Spec(AbilityClass, 1.f);
		GiveAbilityAndActivateOnce(Spec);
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

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__)
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (!AbilitySpec.Ability) return FGameplayTag();
	for (FGameplayTag Tag: 	AbilitySpec.Ability.Get()->AbilityTags)
	{
		if (Tag.MatchesTag(FAuraGameplayTags::Get().Abilities))
			return Tag;
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (!AbilitySpec.Ability) return FGameplayTag();
	for (FGameplayTag Tag: 	AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FAuraGameplayTags::Get().InputTag))
			return Tag;
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast(this);
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) const
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
