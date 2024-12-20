// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/ChampionInformation.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/TargetInterface.h"
#include "Aura/AuraTypes.h"
#include "Interaction/HighlightInterface.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;

DECLARE_MULTICAST_DELEGATE_OneParam(FDamageReceivedSignature, float /* Damage */)
/**
 * 
 */


UCLASS(Abstract)
class AURA_API AAuraEnemy : public AAuraCharacterBase, public ITargetInterface, public IHighlightInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();

	/* Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnhighlightActor_Implementation() override;
	virtual bool SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/* End Highlight Interface */
	
	/* Target Interface */
	FORCEINLINE virtual void SetCombatTarget_Implementation(AActor* NewCombatTarget) override;
	FORCEINLINE virtual AActor* GetCombatTarget_Implementation() const override;
	/* End Target Interface */

	/* Combat Interface */
	virtual int32 GetCharacterLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual void ReceivedDamage_Implementation(const float Damage) override;
	virtual bool IsChampion_Implementation() const override { return bIsChampion; }
	virtual bool CanBeCCed_Implementation() const override;
	/* End Combat Interface */

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

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Champion")
	bool bForceSpawnAsChampion = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Champion")
	EChampionType ForceChampionType = EChampionType::Regenerator;

	UPROPERTY(BlueprintReadOnly, Category="Champion")
	bool bIsChampion = false;

protected:
	
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnFloatChangedSignatureDyn OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnFloatChangedSignatureDyn OnMaxHealthChanged;

	FDamageReceivedSignature OnDamageReceived;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults", meta=(ExposeOnSpawn="true"))
	int32 Level = 1;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBarWidget;

	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAuraAIController> AuraAIController;

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Champion", meta=(ExposeOnSpawn="true"))
	bool bCanBeChampion = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Champion")
	FScalableFloat ChanceToBeChampion;

	UFUNCTION(BlueprintCallable, Category="Champion")
	bool RollIsChampion() const;

	UFUNCTION(BlueprintCallable, Category="Champion")
	static EChampionType GetRandomChampionType();

	UFUNCTION()
	void DebugTestRandomChampType();
	UPROPERTY()
	TMap<EChampionType, int32> ChampionTypeFrequency;

	void Regenerate(const float Damage);

	void ShooterAbility() const;

	UFUNCTION(BlueprintImplementableEvent)
	void Split(AActor* DeadCharacter);

	void SetupChampion(const FChampionInformation& ChampionInfo);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int MaxCrowdControlStacks = 4;
private:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HealthBarWidgetClass;
	
};
