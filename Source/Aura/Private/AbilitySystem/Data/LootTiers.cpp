// Copyright Jordan McWilliams


#include "AbilitySystem/Data/LootTiers.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"

TArray<FLootItem> ULootTiers::GetLootItems() const
{
	TArray<FLootItem> ReturnItems;

	for (const FLootItem& Item: LootItems)
	{
		for (uint8 i = 0; i < Item.MaxNumToSpawn; ++i)
		{
			if (UAuraAbilitySystemLibrary::RNGRoll(Item.ChanceToSpawn))
			{
				FLootItem NewItem;
				NewItem.LootClass = Item.LootClass;
				NewItem.bLootLevelOverride = Item.bLootLevelOverride;
				ReturnItems.Add(NewItem);
			}
		}
	}
	
	return ReturnItems;
}
