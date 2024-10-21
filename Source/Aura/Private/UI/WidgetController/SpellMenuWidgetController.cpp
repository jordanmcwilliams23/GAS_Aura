// Copyright Jordan McWilliams


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChanged.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level)
	{
		if (AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});
	
	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
	GetAuraPS()->OnSpellPointsChangedDelegate.AddDynamic(this, &USpellMenuWidgetController::OnSpellPointsChanged);
	/*GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](const int32 SpellPoints)
		{
			SpellPointsChangedDelegate.Broadcast(SpellPoints);
		}); */
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::SpendPointButtonPressed(const FGameplayTag& AbilityTag)
{
	GetAuraASC()->ServerSpendSpellPoint(AbilityTag);
	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::EquipButtonPressed(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	GetAuraASC()->ServerEquipAbility(AbilityTag, InputTag);
}

bool USpellMenuWidgetController::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
                                                             FString& OutNextDescription)
{
	return GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, OutDescription, OutNextDescription);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag) const
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	
	FAuraAbilityInfo LastSlotInfo = FAuraAbilityInfo();
	LastSlotInfo.StatusTag = Tags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PrevInputTag;
	LastSlotInfo.AbilityTag = Tags.Abilities;
	//Broadcast empty info if Previous Slot is a valid slot. Only if equipping on already-equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);
	
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = InputTag;
	AbilityInfoDelegate.Broadcast(Info);
	SuccessfullyEquippedAbility.Broadcast();
}

void USpellMenuWidgetController::OnSpellPointsChanged(const int32 SpellPoints)
{
	SpellPointsChangedDelegate.Broadcast(SpellPoints);
}
