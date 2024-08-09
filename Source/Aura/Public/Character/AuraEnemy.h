// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/TargetInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public ITargetInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();

	/** Target Interface */
	virtual void HighlightActor() override;
	virtual void UnhighlightActor() override;
	/** End Target Interface */

protected:
	
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
private:
};
