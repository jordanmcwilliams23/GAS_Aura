// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/TargetInterface.h"
#include "Aura/AuraTypes.h"
#include "Interaction/HighlightInterface.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public ITargetInterface, public IHighlightInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();

	/** Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnhighlightActor_Implementation() override;
	virtual bool SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/** End Highlight Interface */
	
	/** Target Interface */
	FORCEINLINE virtual void SetCombatTarget_Implementation(AActor* NewCombatTarget) override;
	FORCEINLINE virtual AActor* GetCombatTarget_Implementation() const override;
	/** End Target Interface */

	/** Combat Interface */
	virtual int32 GetCharacterLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	/** End Combat Interface */

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float LifeSpan = 5.f;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	void SetLevel(const int InLevel) { Level = InLevel; }

protected:
	
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnFloatChangedSignatureDyn OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnFloatChangedSignatureDyn OnMaxHealthChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBarWidget;

	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();
private:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HealthBarWidgetClass;
	
};
