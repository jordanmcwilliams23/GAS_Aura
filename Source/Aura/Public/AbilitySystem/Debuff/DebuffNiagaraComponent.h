// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"

class AAuraCharacterBase;
/**
 * 
 */
UCLASS()
class AURA_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UDebuffNiagaraComponent();
	
	UPROPERTY(EditAnywhere)
	FGameplayTag DebuffTag;

	virtual void Deactivate() override;

	void SetupComponent();
protected:
	UFUNCTION()
	void OnOwnerDeath(AActor* Character);
};
