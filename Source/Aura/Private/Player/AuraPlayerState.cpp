// Copyright Jordan McWilliams


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	NetUpdateFrequency = 100.f;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("Attribute Set");
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

void AAuraPlayerState::AddXP(const int32 AddedXP)
{
	XP += AddedXP;
	const int32 PrevLevel = Level;
	while (LevelUpInfo->CanLevelUp(Level, XP))
	{
		Level++;
		const FLevelInfo& LevelInfo = LevelUpInfo->GetLevelInfo(Level);
		if (LevelInfo.AttributePoints != 0)
		{
			AttributePoints += LevelInfo.AttributePoints;
			OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
		}
		if (LevelInfo.SpellPoints != 0)
		{
			SpellPoints += LevelInfo.SpellPoints;
			OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
		}
	}
	if (PrevLevel != Level)
	{
		Cast<UAuraAttributeSet>(AttributeSet)->RefillVitalAttributes();
		GetAuraAbilitySystem()->UpdateAbilityStatuses(Level);
		OnLevelChangedDelegate.Broadcast(Level, true);
	}
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddAttributePoints(const int32 Points)
{
	if ((AttributePoints += Points) < 0.f)
		AttributePoints = 0.f;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetAttributePoints(const int32 Points)
{
	AttributePoints = Points;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddSpellPoints(const int32 Points)
{
	if ((SpellPoints += Points) < 0.f)
		SpellPoints = 0.f;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::SetSpellPoints(const int32 Points)
{
	SpellPoints = Points;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

UAuraAbilitySystemComponent* AAuraPlayerState::GetAuraAbilitySystem()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerState::OnRep_XP(int32 OldXP) const
{
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel) const
{
	OnLevelChangedDelegate.Broadcast(Level, false);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints) const
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints) const
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}
