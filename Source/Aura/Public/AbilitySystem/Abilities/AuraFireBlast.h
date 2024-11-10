// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireball;
/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireball*> SpawnFireballs();
protected:
	UPROPERTY(EditDefaultsOnly, Category= "FireBlast")
	int32 NumFireBolts = 12;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraFireball> FireballClass;
};
