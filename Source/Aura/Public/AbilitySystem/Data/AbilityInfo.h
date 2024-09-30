// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilityInfo.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories = "Abilities"))
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories = "Abilities.Type"))
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, meta=(Categories = "InputTag"))
	FGameplayTag InputTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, meta=(Categories = "Abilities.Status"))
	FGameplayTag StatusTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> BackgroundMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories = "Damage"))
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name = FText();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText NextLevelDescription = FText();
};
/**
 * 
 */
UCLASS()
class AURA_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AbilityInformation")
	TArray<FAuraAbilityInfo> AbilityInformation;

	UFUNCTION(BlueprintCallable)
	FAuraAbilityInfo FindAbilityInfoForTag(const FGameplayTag& AbilityTag, const bool bLogIfNotFound = false) const;
};
