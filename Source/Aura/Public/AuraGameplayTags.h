// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#define DECLARE_DAMAGE_RESISTANCE_DEBUFF_TYPE_TAGS(AttName, DebuffName)\
	FGameplayTag Damage_##AttName; \
	FGameplayTag Attributes_Resistance_##AttName; \
	FGameplayTag Debuff_##DebuffName; \

#define INITIALIZE_DAMAGE_RESISTANCE_DEBUFF_TYPE_TAGS(AttName, DebuffName)\
{\
	FString TagDamage = "Damage."#AttName; \
	FString TagResistance = "Attributes.Resistance."#AttName; \
	FString TagDebuff = "Debuff."#DebuffName; \
	GameplayTags.Damage_##AttName = UGameplayTagsManager::Get().AddNativeGameplayTag(*TagDamage, TagDamage); \
	GameplayTags.Attributes_Resistance_##AttName = UGameplayTagsManager::Get().AddNativeGameplayTag(*TagResistance, TagResistance); \
	GameplayTags.Debuff_##DebuffName = UGameplayTagsManager::Get().AddNativeGameplayTag(*TagDebuff, FString("Debuff for "#AttName" Damage")); \
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_##AttName, GameplayTags.Attributes_Resistance_##AttName); \
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_##AttName, GameplayTags.Debuff_##DebuffName); \
\
};

/**
 * AuraGameplayTags
 * Singleton class that contains all the native gameplay tags used in the game.
 */
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	/* Primary Attributes */
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;
	/* End Primary Attributes */

	/* Secondary Attributes */
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	/* End Secondary Attributes */
	
	/* Meta Attributes */
	FGameplayTag Attributes_Meta_IncomingXP;
	/* End Meta Attributes */

	/* Input Tags */
	FGameplayTag InputTag;
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;
	/* End Input Tags */

	/* Damage Types & Resistances */
	FGameplayTag Damage;

	DECLARE_DAMAGE_RESISTANCE_DEBUFF_TYPE_TAGS(Fire, Burn);
	DECLARE_DAMAGE_RESISTANCE_DEBUFF_TYPE_TAGS(Lightning, Stun);
	DECLARE_DAMAGE_RESISTANCE_DEBUFF_TYPE_TAGS(Arcane, Arcane);
	DECLARE_DAMAGE_RESISTANCE_DEBUFF_TYPE_TAGS(Physical, Physical);

	FGameplayTag Debuff_Chance;
	FGameplayTag Debuff_Damage;
	FGameplayTag Debuff_Frequency;
	FGameplayTag Debuff_Duration;
	/* End Damage Types & Resistances */

	/* Ability Tags */
	FGameplayTag Abilities;
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;
	
	FGameplayTag Abilities_HitReact;

	FGameplayTag Abilities_Status;
	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Equipped;

	FGameplayTag Abilities_Type_None;
	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	
	FGameplayTag Abilities_Fire_FireBolt;
	FGameplayTag Abilities_Fire_FireBlast;

	FGameplayTag Abilities_Lightning_Electrocute;

	FGameplayTag Abilities_Arcane_ArcaneShards;

	FGameplayTag Abilities_Passive;
	FGameplayTag Abilities_Passive_HaloOfProtection;
	FGameplayTag Abilities_Passive_LifeSiphon;
	FGameplayTag Abilities_Passive_ManaSiphon;
	/* End Ability Tags */

	/* Data Tags */
	FGameplayTag Data_Cooldown;
	/* End Data Tags */

	/* Combat Socket Tags */
	
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_Tail;
	

	/* End Combat Socket Tags */

	/* Montage Tags */
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

	/* End Montage Tags */

	FGameplayTagContainer DamageTypes;
	
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;
	TMap<FGameplayTag, FName> TagToSocketName;
	
	FGameplayTag Effects_HitReact;

	/* Player Tags */
	FGameplayTag Player_Block_InputPressed;
	FGameplayTag Player_Block_InputHeld;
	FGameplayTag Player_Block_InputReleased;
	FGameplayTag Player_Block_CursorTrace;
	
protected:
private:
	static FAuraGameplayTags GameplayTags;
};
