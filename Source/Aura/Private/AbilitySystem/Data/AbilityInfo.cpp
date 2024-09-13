// Copyright Jordan McWilliams


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, const bool bLogIfNotFound) const
{
	for (const FAuraAbilityInfo& AbilityInfo: AbilityInformation)
	{
		if (AbilityInfo.AbilityTag == AbilityTag)
			return AbilityInfo;
	}
	if (bLogIfNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Cant find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}
	
	return FAuraAbilityInfo();
}
