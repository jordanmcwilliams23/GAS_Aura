// Copyright Jordan McWilliams


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(FGameplayTag& Tag, bool bLogIfNotFound) const
{
	for (const auto& [InputAction, InputTag]: AbilityInputActions)
	{
		if (InputTag.MatchesTagExact(Tag))
		{
			return InputAction;
		}
	}
	if (bLogIfNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("The tag in AuraInputConfig [%s] was not found"), *Tag.ToString());
	}
	return nullptr;
}
