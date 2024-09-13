// Copyright Jordan McWilliams


#include "AbilitySystem/Data/AttributeInfo.h"

#include "Aura/AuraLogChannels.h"

const FAuraAttributeInfo UAttributeInfo::FindAttributeTagInfo(const FGameplayTag& Tag, const bool bLogNotFound)
{
	for (const FAuraAttributeInfo& Info: AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(Tag))
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find info for attribute tag [%s] on AttributeInfo [%s]."), *Tag.ToString(), *GetNameSafe(this));
	}
	return FAuraAttributeInfo();
}
