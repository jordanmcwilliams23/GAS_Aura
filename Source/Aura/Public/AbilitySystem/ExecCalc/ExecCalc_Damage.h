 // Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecCalc_Damage();
	static void HandleDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                          const FGameplayEffectSpec& OwningSpec, const FAggregatorEvaluateParameters& EvaluateParameters);
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	static float CalculateRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle,const float Damage, const AActor* TargetAvatar);
};
