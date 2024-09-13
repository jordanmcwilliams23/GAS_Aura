// Copyright Jordan McWilliams


#include "AbilitySystem/AsyncTasks/ListenForCooldownChange.h"
#include "AbilitySystemComponent.h"

UListenForCooldownChange* UListenForCooldownChange::ListenForCooldownChange(
	UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
	UListenForCooldownChange* ListenForCooldownChange = NewObject<UListenForCooldownChange>();
	ListenForCooldownChange->ASC = AbilitySystemComponent;
	ListenForCooldownChange->CooldownTag = InCooldownTag;
	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		ListenForCooldownChange->EndTask();
		return nullptr;
	}
	//To know when a cooldown has ended
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(ListenForCooldownChange, &UListenForCooldownChange::CooldownTagChanged);
	//To know when a cooldown effect has been applied
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(ListenForCooldownChange, &UListenForCooldownChange::OnActiveEffectAdded);
	return ListenForCooldownChange;
}

void UListenForCooldownChange::EndTask()
{
	if (!IsValid(ASC)) return;
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UListenForCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UListenForCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AllGrantedTags;
	EffectSpec.GetAllGrantedTags(AllGrantedTags);

	if (AllGrantedTags.HasTagExact(CooldownTag))
	{
		const FGameplayEffectQuery EffectQuery = FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(EffectQuery);
		if (TimesRemaining.Num() > 0)
		{
			float HighestTime = TimesRemaining[0];
			for (int32 i = 0; i < TimesRemaining.Num(); i++)
			{
				if (TimesRemaining[i] > HighestTime)
				{
					HighestTime = TimesRemaining[i];
				}
			}
			CooldownStart.Broadcast(HighestTime);
		}
	}
}
