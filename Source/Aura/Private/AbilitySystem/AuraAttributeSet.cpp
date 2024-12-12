// Copyright Jordan McWilliams


#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/GameModeInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet() { }

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/* Primary Attributes*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	/* Secondary Attributes*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	/* Vital Attributes*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	/* Resistance Attributes */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, const float OldValue, const float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	} else if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
                                            FEffectProperties& Props)
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(Props.SourceASC.Get())
		&& Props.SourceASC->AbilityActorInfo.IsValid()
		&& Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* SourcePawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = SourcePawn->GetController();
			}
		}
		if (Props.SourceController != nullptr)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceAvatarActor);
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor.Get());
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Props, const float Damage, const bool bIsBlockedHit, const bool bIsCriticalHit)
{
	Cast<AAuraPlayerController>(UGameplayStatics::GetPlayerController(Props.SourceCharacter, 0))
	->ShowDamageNumber(Damage, Props.TargetCharacter.Get(), bIsBlockedHit, bIsCriticalHit);
	/* //For showing damage numbers to all players, not used since currently single-player focused
	const TArray<AAuraPlayerController*>& PlayerControllers = UAuraAbilitySystemLibrary::GetAllPlayerControllers(Props.SourceCharacter);
	for (AAuraPlayerController* AuraPlayerController: PlayerControllers)
	{
		AuraPlayerController->ShowDamageNumber(Damage, Props.TargetCharacter.Get(), bIsBlockedHit, bIsCriticalHit);
	}
	*/
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& EffectProperties) const
{
	if (EffectProperties.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetCharacterLevel(EffectProperties.TargetCharacter);
		float XPReward = UAuraAbilitySystemLibrary::GetXPAmount(this, ICombatInterface::Execute_GetCharacterClass(EffectProperties.TargetCharacter.Get()), TargetLevel);
		if (ICombatInterface::Execute_IsChampion(EffectProperties.TargetCharacter))
		{
			XPReward *= UAuraAbilitySystemLibrary::GetChampionInfo(EffectProperties.TargetCharacter).ExperienceMultiplier;
		}
		FGameplayEventData GameplayEventData = FGameplayEventData();
		GameplayEventData.EventTag = FAuraGameplayTags::Get().Attributes_Meta_IncomingXP;
		GameplayEventData.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			EffectProperties.SourceCharacter.Get(),
			FAuraGameplayTags::Get().Attributes_Meta_IncomingXP,
			GameplayEventData);
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	
	//return if character is dead
	if (Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	} else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	} else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	} else if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	const bool appliedDebuff = UAuraAbilitySystemLibrary::DidApplyDebuff(Props.EffectContextHandle);
	if (UAuraAbilitySystemLibrary::DidApplyDebuff(Props.EffectContextHandle)) Debuff(Props);
	if (LocalIncomingDamage <= 0.f)
	{
		return;
	}
	const float NewHealth = GetHealth() - LocalIncomingDamage;
	SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
	if (NewHealth > 0.f)
	{
		//Check if should hit react and is not being shocked
		if (UAuraAbilitySystemLibrary::GetShouldHitReact(Props.EffectContextHandle) &&
			Props.TargetCharacter->Implements<UCombatInterface>() &&
			!ICombatInterface::Execute_IsBeingShocked(Props.TargetCharacter))
		{
			//Hit React
			Props.TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FAuraGameplayTags::Get().Abilities_HitReact));
		}

		//Check Knockback
		const FVector KnockbackForce = UAuraAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
        const float KnockbackChance = UAuraAbilitySystemLibrary::GetKnockbackChance(Props.EffectContextHandle);
		if (UAuraAbilitySystemLibrary::RNGRoll(KnockbackChance) && !KnockbackForce.IsNearlyZero(1.f))
		{
			Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
		}
		if (Props.TargetAvatarActor->Implements<UCombatInterface>())
		{
			ICombatInterface::Execute_ReceivedDamage(Props.TargetAvatarActor, LocalIncomingDamage);
		}
	} else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor))
	{
		CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
		SendXPEvent(Props);
	}
	
	const bool bBlock = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
	const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
	ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);
	if (Props.SourceCharacter->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter.Get(), LocalIncomingXP);
	}
}

void UAuraAttributeSet::Debuff(const FEffectProperties& Props)
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Props.SourceCharacter);
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(Props.EffectContextHandle.Get());
	const FGameplayTag DamageType = *AuraGameplayEffectContext->GetDamageType();
	const FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	const float DebuffDamage = AuraGameplayEffectContext->GetDebuffDamage();
	const float DebuffDuration = AuraGameplayEffectContext->GetDebuffDuration();
	const float DebuffFrequency = AuraGameplayEffectContext->GetDebuffFrequency();
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));
	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->bExecutePeriodicEffectOnApplication = false;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
	//Debuff granted tag
	FInheritedTagContainer InheritedTagContainer = FInheritedTagContainer();
	UTargetTagsGameplayEffectComponent& Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	const FGameplayTag& DebuffTag = Tags.DamageTypesToDebuffs[DamageType];
	InheritedTagContainer.Added.AddTag(DebuffTag);
	//const bool bIsPlayer = GetOwningActor()->Implements<UPlayerInterface>();
	if (Props.TargetCharacter->Implements<UPlayerInterface>() && DebuffTag.MatchesTagExact(Tags.Debuff_Stun))
	{
		InheritedTagContainer.Added.AddTag(Tags.Player_Block_CursorTrace);
		InheritedTagContainer.Added.AddTag(Tags.Player_Block_InputPressed);
		InheritedTagContainer.Added.AddTag(Tags.Player_Block_InputHeld);
		InheritedTagContainer.Added.AddTag(Tags.Player_Block_InputReleased);
	}
	Component.SetAndApplyTargetTagChanges(InheritedTagContainer);
	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;
	const int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];
	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = GetIncomingDamageAttribute();
	if (const FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().Get());
		const TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		AuraContext->SetDamageType(DebuffDamageType);
		AuraContext->SetShouldHitReact(false);
		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
}

void UAuraAttributeSet::RefillVitalAttributes()
{
	SetHealth(GetMaxHealth());
	SetMana(GetMaxMana());
	bTopOffHealth = bTopOffMana = true;
}

//Replication Boilerplate
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

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

