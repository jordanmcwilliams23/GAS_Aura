// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "Aura/AuraTypes.h"
#include "AuraPlayerState.generated.h"

class UAuraAbilitySystemComponent;
class UGameplayEffect;
class UAttributeSet;
class ULevelUpInfo;
class UAbilitySystemComponent;
class UAuraAttributeSet;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChanged, int32 /* Current Level */, bool /* LevelUp? */)
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE int32 GetPlayerLevel() const {return Level;}
	FORCEINLINE int32 GetPlayerXP() const {return XP;}
	FORCEINLINE int32 GetAttributePoints() const {return AttributePoints;}
	FORCEINLINE int32 GetSpellPoints() const {return SpellPoints;}
	
	FOnInt32ChangedSignature OnXPChangedDelegate;
	FOnLevelChanged OnLevelChangedDelegate;
	FOnInt32ChangedSignature OnAttributePointsChangedDelegate;
	FOnInt32ChangedSignatureDyn OnSpellPointsChangedDelegate;

	void SetXP(const int32 NewXP) { XP = NewXP; OnXPChangedDelegate.Broadcast(XP); }
	void AddXP(const int32 AddedXP);

	void AddAttributePoints(const int32 Points);
	void SetAttributePoints(const int32 Points);
	void AddSpellPoints(const int32 Points);
	void SetSpellPoints(const int32 Points);

	void SetLevel(const int32 NewLevel) { Level = NewLevel; OnLevelChangedDelegate.Broadcast(Level, false);}
	void AddLevel(const int32 AddedLevels) { Level += AddedLevels; OnLevelChangedDelegate.Broadcast(Level, true);}

	ULevelUpInfo* GetLevelUpInfo() const {return LevelUpInfo; };
	

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> SetPrimaryAttributesGameplayEffect;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:

	UAuraAbilitySystemComponent* GetAuraAbilitySystem();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 0;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 SpellPoints = 0;
	
	UFUNCTION()
	void OnRep_XP(int32 OldXP) const;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel) const;

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints) const;

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints) const;
};
