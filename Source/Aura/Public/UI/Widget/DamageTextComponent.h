// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetDamageText(float Damage, const bool bIsBlockedHit, const bool bIsCriticalHit);
	
};
