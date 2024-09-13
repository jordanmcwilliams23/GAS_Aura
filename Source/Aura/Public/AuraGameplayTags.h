// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#define DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(T)\
	FGameplayTag Damage_##T; \
	FGameplayTag Attributes_Resistance_##T; \

#define INITIALIZE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(T)\
{\
	FString TagDamage = "Damage."#T; \
	FString TagResistance = "Attributes.Resistance."#T; \
	GameplayTags.Damage_##T = UGameplayTagsManager::Get().AddNativeGameplayTag(*TagDamage, TagDamage); \
	GameplayTags.Attributes_Resistance_##T = UGameplayTagsManager::Get().AddNativeGameplayTag(*TagResistance, TagResistance); \
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_##T, GameplayTags.Attributes_Resistance_##T); \
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
	/* End Input Tags */

	/* Damage Types & Resistances */
	FGameplayTag Damage;

	DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(Fire);
	DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(Lightning);
	DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(Arcane);
	DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(Physical);

	/* End Damage Types & Resistances */

	/* Ability Tags */
	FGameplayTag Abilities;
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;

	FGameplayTag Abilities_Fire_FireBolt;
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
	TMap<FGameplayTag, FName> TagToSocketName;
	
	FGameplayTag Effects_HitReact;
protected:
private:
	static FAuraGameplayTags GameplayTags;
};
