// Copyright Jordan McWilliams


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

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
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	for (FAuraAttributeInfo& Info: AttributeInfo.Get()->AttributeInformation)
	{
		BroadcastAttributeInfo(Info.AttributeTag);
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeTagInfo(Tag);
	Info.AttributeValue = Info.AttributeGetter.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
