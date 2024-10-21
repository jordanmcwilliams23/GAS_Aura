// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interaction/GameStateInterface.h"
#include "AuraGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameStateBase : public AGameStateBase, public IGameStateInterface
{
	GENERATED_BODY()
public:
	virtual UAbilityInfo* GetAbilityInfo() const override;
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilityInfo> AbilityInfo;
};
