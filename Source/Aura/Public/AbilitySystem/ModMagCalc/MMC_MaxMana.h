// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxMana.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_MaxMana();
private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

	float BaseMana = 60.f;

	float ManaMultiplierPerLevel = 20.f;
	
	float ManaMultiplierPerIntelligence = 3.f;
};
