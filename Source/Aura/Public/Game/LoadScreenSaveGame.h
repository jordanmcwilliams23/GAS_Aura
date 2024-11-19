// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

	FSavedActor() {}
	FSavedActor(const FName& InActorName, const FTransform& InTransform): ActorName(InActorName), Transform(InTransform) {}

	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform Transform = FTransform();

	//Serialized variables from the actor - only those marked with SaveGame specifier
	UPROPERTY()
	TArray<uint8> Bytes;

	bool operator==(const FSavedActor& Other) const
	{
		return this->ActorName == Other.ActorName;
	}
};

USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

	FString MapAssetName = FString();

	TArray<FSavedActor> SavedActors;
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(Categories="Abilities"))
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(Categories="Abilities.Status"))
	FGameplayTag AbilityStatus = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(Categories="Input"))
	FGameplayTag AbilitySlot = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(Categories="Abilities.Type"))
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel;

	bool operator==(const FSavedAbility& Other) const
	{
		return this->AbilityTag.MatchesTagExact(Other.AbilityTag);
	}
};
/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;

	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	UPROPERTY()
	FString PlayerName = FString("Default Name");

	UPROPERTY()
	FString MapName = FString("Default Map Name");
	
	UPROPERTY()
	FString MapAssetName = FString("Default Map Asset Name");

	UPROPERTY()
	FName PlayerStartTag;

	/* Player */

	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	int32 XP = 0;

	UPROPERTY()
	int32 SpellPoints = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	float Strength = 0;

	UPROPERTY()
	float Intelligence = 0;

	UPROPERTY()
	float Resilience = 0;

	UPROPERTY()
	float Vigor = 0;

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;
	
	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	FSavedMap GetSavedMapWithMapName(const FString& InMapName);
	bool HasMap(const FString& InMapName);
};
