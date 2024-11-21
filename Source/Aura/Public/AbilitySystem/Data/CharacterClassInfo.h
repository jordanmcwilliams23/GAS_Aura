// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Warrior,
	Ranger
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
	
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> ClassAbilities;

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	FScalableFloat XPAmount;

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TObjectPtr<UCurveTable> ClassPrimaryAttributesTable;
};

//Struct to hold data for champion settings
USTRUCT()
struct FChampionInfo
{
	GENERATED_BODY()

	//Regenerator
	
	UPROPERTY(EditDefaultsOnly, Category="Champion")
	float Regenerator_HealthPercentThreshold = 33.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Champion")
	float Regenerator_RegenDuration = 4.f;

	UPROPERTY(EditDefaultsOnly, Category="Champion")
	float Regenerator_RegenPeriod = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category="Champion")
	float Regenerator_RegenPercent = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Champion")
	TSubclassOf<UGameplayEffect> RegenerationGameplayEffectClass;

	//Shooter
	UPROPERTY(EditDefaultsOnly, Category="Champion")
	float Shooter_FireRate = 4.f;

	UPROPERTY(EditDefaultsOnly, Category="Champion")
	TSubclassOf<UGameplayAbility> Shooter_FireProjectileClass;
	
	//Speedy
	UPROPERTY(EditDefaultsOnly, Category="Champion")
	float Speedy_SpeedMultiplier = 1.35f;
};
/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes_Infinite;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;
	
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes_SetByCaller;

	UPROPERTY(EditDefaultsOnly, Category="Champion")
	FChampionInfo ChampionInfo;
};
