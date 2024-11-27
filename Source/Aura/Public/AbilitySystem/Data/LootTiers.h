// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiers.generated.h"
class AAuraEnemy;
class AAuraEffectActor;

USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "LootTiers|Spawning")
	TSubclassOf<AAuraEffectActor> LootClass;

	UPROPERTY(EditAnywhere, Category= "LootTiers|Spawning")
	uint8 ChanceToSpawn = 0;

	UPROPERTY(EditAnywhere, Category= "LootTiers|Spawning")
	uint8 MaxNumToSpawn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "LootTiers|Spawning")
	bool bLootLevelOverride = true;
};
/**
 * 
 */
UCLASS()
class AURA_API ULootTiers : public UDataAsset
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	TArray<FLootItem> GetLootItems(const AAuraEnemy* Enemy = nullptr) const;
	
	UPROPERTY(EditDefaultsOnly, Category= "LootTiers|Spawning")
	TArray<FLootItem> LootItems;
};
