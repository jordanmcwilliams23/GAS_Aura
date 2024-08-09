// Copyright Jordan McWilliams


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(50.f);
	InitMaxMana(100.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
	FFullEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SrcProps->ASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(Props.SrcProps->ASC.Get())
		&& Props.SrcProps->ASC->AbilityActorInfo.IsValid()
		&& Props.SrcProps->ASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SrcProps->AvatarActor = Props.SrcProps->ASC->AbilityActorInfo->AvatarActor.Get();
		Props.SrcProps->Controller = Props.SrcProps->ASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SrcProps->Controller == nullptr && Props.SrcProps->AvatarActor != nullptr)
		{
			if (const APawn* SourcePawn = Cast<APawn>(Props.SrcProps->AvatarActor))
			{
				Props.SrcProps->Controller = SourcePawn->GetController();
			}
		}
		if (Props.SrcProps->Controller != nullptr)
		{
			Props.SrcProps->Character = Cast<ACharacter>(Props.SrcProps->AvatarActor);
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetProps->AvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetProps->Controller = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetProps->Character = Cast<ACharacter>(Props.TargetProps->AvatarActor);
		Props.TargetProps->ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetProps->AvatarActor.Get());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FFullEffectProperties Props;
	SetEffectProperties(Data, Props);
}


void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

