// Copyright Jordan McWilliams


#include "AbilitySystem/Data/LootTiers.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Character/AuraEnemy.h"

TArray<FLootItem> ULootTiers::GetLootItems(const AAuraEnemy* Enemy) const
{
	TArray<FLootItem> ReturnItems;

	FChampionInfo ChampionInfo = UAuraAbilitySystemLibrary::GetChampionInfo(Enemy);

	for (const FLootItem& Item: LootItems)
	{
		const float SpawnChance = Item.ChanceToSpawn + Enemy->bIsChampion ? ChampionInfo.ExtraSpawnLootChance : 0.f;
		for (uint8 i = 0; i < Item.MaxNumToSpawn + Enemy->bIsChampion ? ChampionInfo.ExtraNumLootItems : 0; ++i)
		{
			if (UAuraAbilitySystemLibrary::RNGRoll(SpawnChance))
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
