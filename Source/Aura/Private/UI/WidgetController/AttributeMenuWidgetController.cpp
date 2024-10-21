// Copyright Jordan McWilliams


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AS);
	for (FAuraAttributeInfo& Info: AttributeInfo.Get()->AttributeInformation)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Info.AttributeGetter).AddLambda(
			[this, Info](const FOnAttributeChangeData& ChangeData)
			{
				BroadcastAttributeInfo(Info.AttributeTag);
			}
			);
	}
	GetAuraPS()->OnAttributePointsChangedDelegate.AddLambda(
			[this](const int32 AttributePoints)
		{
			AttributePointsChangedDelegate.Broadcast(AttributePoints);
		});
	/* GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](const int32 SpellPoints)
	{
		SpellPointsChangedDelegate.Broadcast(SpellPoints);
	}); */
	GetAuraPS()->OnSpellPointsChangedDelegate.AddDynamic(this, &UAttributeMenuWidgetController::OnSpellPointsChanged);
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	check(AttributeInfo);
	for (FAuraAttributeInfo& Info: AttributeInfo.Get()->AttributeInformation)
	{
		BroadcastAttributeInfo(Info.AttributeTag);
	}
	AttributePointsChangedDelegate.Broadcast(GetAuraPS()->GetAttributePoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag, const float Amount)
{
	CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent)->UpgradeAttribute(AttributeTag, Amount);
}

void UAttributeMenuWidgetController::SetAttributesBack(const TMap<FGameplayTag, int>& PreviousStatValues,
                                                       const int PreviousAttributePoints)
{
	CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent)->SetAttributesBack(PreviousStatValues, PreviousAttributePoints);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeTagInfo(Tag);
	Info.AttributeValue = Info.AttributeGetter.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::OnSpellPointsChanged(const int32 SpellPoints)
{
	SpellPointsChangedDelegate.Broadcast(SpellPoints);
}
