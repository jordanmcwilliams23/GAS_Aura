// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "Aura/AuraTypes.h"
#include "AuraPlayerState.generated.h"

class UAttributeSet;
class ULevelUpInfo;
class UAbilitySystemComponent;
class UAuraAttributeSet;

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
	
	FOnInt32ChangedSignature OnXPChangedDelegate;
	FOnInt32ChangedSignature OnLevelChangedDelegate;

	void SetXP(const int32 NewXP) { XP = NewXP; OnXPChangedDelegate.Broadcast(XP); }
	void AddXP(const int32 AddedXP);

	void SetLevel(const int32 NewLevel) { Level = NewLevel; OnLevelChangedDelegate.Broadcast(Level);}
	void AddLevel(const int32 AddedLevels) { Level += AddedLevels; OnLevelChangedDelegate.Broadcast(Level);}

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 0;

	UFUNCTION()
	void OnRep_XP(int32 OldXP) const;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel) const;
};
