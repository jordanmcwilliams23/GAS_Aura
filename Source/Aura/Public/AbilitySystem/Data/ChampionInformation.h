// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Engine/DataAsset.h"
#include "ChampionInformation.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UENUM()
enum class EChampionType : uint8
{
	Regenerator = 0,
	Shooter = 1,
	Speedy = 2,
	Splitter = 3
};

//Struct to hold data for champion settings
USTRUCT(BlueprintType)
struct FChampionInformation
{
	GENERATED_BODY()

	//General

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Champion")
	bool bChampionsEnabled = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Champion")
	FVector2f ChampionAttributeMultiplierRange = FVector2f(1.2f, 2.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Champion")
	float ScaleMultiplier = 1.2f;
	
	UPROPERTY(EditDefaultsOnly, Category="Champion")
	TObjectPtr<UNiagaraSystem> BaseNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, Category="Champion")
	TMap<EChampionType, FLinearColor> ChampionParticleColors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Champion")
	int32 ExtraNumLootItems = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Champion")
	float ExtraSpawnLootChance = 5.f;

	//Regenerator
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	float Regenerator_HealthPercentThreshold = 33.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	float Regenerator_RegenDuration = 4.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	float Regenerator_RegenPeriod = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	float Regenerator_RegenPercent = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	TSubclassOf<UGameplayEffect> RegenerationGameplayEffectClass;

	//Shooter
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	float Shooter_FireRate = 4.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	TSubclassOf<UGameplayAbility> Shooter_FireProjectileClass;
	
	//Speedy
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	float Speedy_SpeedMultiplier = 1.35f;

	//Splitter
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	uint8 Splitter_NumSplitInto = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	float Splitter_SpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	float Splitter_CloneScaleMultiplier = 0.75f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Champion")
	TObjectPtr<UNiagaraSystem> GroundSummonNiagaraSystem;
};
