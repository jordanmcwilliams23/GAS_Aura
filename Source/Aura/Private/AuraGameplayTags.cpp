// Copyright Jordan McWilliams


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeTags()
{
	/* Primary Attributes */
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Strength"),
		FString("Increases physical damage"));
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Intelligence"),
		FString("Increases magical damage"));
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Resilience"),
		FString("Armor, Armor Penetration, Critical Damage"));
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Vigor"),
		FString("Max Health, Health Regen"));
	
	/* Secondary Attributes */
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"),
		FString("Reduces incoming damage, improves block chance"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ArmorPenetration"),
		FString("Ignore percent of enemy's armor, Increases Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.BlockChance"),
		FString("Chance to cut incoming damage in half"));
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitChance"),
		FString("Chance to double damage plus critical hit bonus"));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitDamage"),
		FString("Bonus damage on critical hit"));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitResistance"),
		FString("Reduce critical hit chance from enemies"));
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.HealthRegeneration"),
		FString("Amount of Health regenerated per second"));
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ManaRegeneration"),
		FString("Amount of Mana regenerated per second"));
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"),
		FString("Maximum Amount of Health obtainable"));
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxMana"),
		FString("Maximum Amount of Mana obtainable"));

	/* Meta Attributes */
	GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Meta.IncomingXP"),
			FString("Incoming XP meta tag"));

	/* Status Tags */
	GameplayTags.Status_Dead = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Status.Dead"),
			FString("Applied to dead character"));
	
	/* Input Tags */
	GameplayTags.InputTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag"),
		FString("Input Tag Parent"));
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LMB"),
		FString("Input tag for left mouse button"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.RMB"),
		FString("Input tag for right mouse button"));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.1"),
		FString("Input tag for 1 key"));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.2"),
		FString("Input tag for 2 key"));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.3"),
		FString("Input tag for 3 key"));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.4"),
		FString("Input tag for 4 key"));
	GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Passive.1"),
		FString("Passive Input tag 1"));
	GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Passive.2"),
		FString("Passive Input tag 2"));

	/* Misc */
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"),
		FString("Damage"));

	INITIALIZE_DAMAGE_RESISTANCE_DEBUFF_TYPE_TAGS(Fire, Burn);
	INITIALIZE_DAMAGE_RESISTANCE_DEBUFF_TYPE_TAGS(Lightning, Stun);
	INITIALIZE_DAMAGE_RESISTANCE_DEBUFF_TYPE_TAGS(Arcane, Arcane);
	INITIALIZE_DAMAGE_RESISTANCE_DEBUFF_TYPE_TAGS(Physical, Physical);

	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Chance"),
		FString("Debuff Chance"));
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Damage"),
		FString("Debuff Damage"));
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Frequency"),
		FString("Debuff Frequency"));
	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Duration"),
		FString("Debuff Duration"));
	
	GameplayTags.DamageTypes = UGameplayTagsManager::Get().RequestGameplayTagChildren(GameplayTags.Damage);

	/* Ability Tags */
	GameplayTags.Abilities = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities"),
		FString("Ability Tag Parent"));
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Attack"),
		FString("Attack Ability Tag"));
	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Summon"),
		FString("Summon Ability Tag"));

	GameplayTags.Abilities_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.HitReact"),
		FString("HitReact Ability"));

	GameplayTags.Abilities_Status = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Status"),
			FString("Ability Status"));
	GameplayTags.Abilities_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Status.Locked"),
			FString("Ability Status Locked"));
	GameplayTags.Abilities_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Status.Eligible"),
			FString("Ability Status Eligible"));
	GameplayTags.Abilities_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Status.Unlocked"),
			FString("Ability Status Unlocked"));
	GameplayTags.Abilities_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Status.Equipped"),
			FString("Ability Status Equipped"));

	GameplayTags.Abilities_Type_None = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Type.None"),
			FString("No Ability Type"));
	GameplayTags.Abilities_Type_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Type.Offensive"),
			FString("Offensive Ability Type"));
	GameplayTags.Abilities_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Type.Passive"),
			FString("Passive Ability Type"));

	/* Offensive Ability Tags */
	GameplayTags.Abilities_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Fire.FireBolt"),
		FString("FireBolt Ability Tag"));
	
	GameplayTags.Abilities_Fire_FireBlast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Fire.FireBlast"),
		FString("FireBlast Ability Tag"));
	
	GameplayTags.Abilities_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Lightning.Electrocute"),
		FString("Electrocute Ability Tag"));
	
	GameplayTags.Abilities_Arcane_ArcaneShards = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Arcane.ArcaneShards"),
		FString("Arcane Shards Ability Tag"));

	
	/* Passive Ability Tags */
	GameplayTags.Abilities_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Passive"),
		FString("Passive Ability Parent Tag"));
	GameplayTags.Abilities_Passive_HaloOfProtection = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Passive.HaloOfProtection"),
		FString("Halo Of Protection Passive Ability"));
	GameplayTags.Abilities_Passive_LifeSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Passive.LifeSiphon"),
		FString("Life Siphon Passive Ability"));
	GameplayTags.Abilities_Passive_ManaSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Passive.ManaSiphon"),
		FString("ManaSiphon Passive Ability"));

	/* Data Tags */
	GameplayTags.Data_Cooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Data.Cooldown"),
		FString("Cooldown Data Tag"));
	
	/* Combat Socket Tags */
	GameplayTags.CombatSocket = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket"),
		FString("Generic tag for testing if using combat socket"));
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.Weapon"),
		FString("Weapon"));
	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.LeftHand"),
    		FString("Left Hand"));
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.RightHand"),
FString("Right Hand"));
	GameplayTags.CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.Tail"),
FString("Tail"));
	
	GameplayTags.TagToSocketName.Add(GameplayTags.CombatSocket_Weapon, FName("WeaponSocket"));
	GameplayTags.TagToSocketName.Add(GameplayTags.CombatSocket_LeftHand, FName("LeftHandSocket"));
	GameplayTags.TagToSocketName.Add(GameplayTags.CombatSocket_RightHand, FName("RightHandSocket"));
	GameplayTags.TagToSocketName.Add(GameplayTags.CombatSocket_Tail, FName("TailSocket"));

	/* Montage Tags */
	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.1"),
		FString("Montage Attack 1"));
	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.2"),
		FString("Montage Attack 2"));
	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.3"),
		FString("Montage Attack 3"));
	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.4"),
		FString("Montage Attack 4"));

	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.HitReact"),
		FString("Hit React Effect"));

	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.CursorTrace"),
		FString("Block cursor tracing"));
	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputHeld"),
		FString("Block Input Held Callback"));
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputPressed"),
		FString("Block Input Pressed Callback"));
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputReleased"),
		FString("Block Input Released Callback"));

	/* Gameplay Cue Tags */
	GameplayTags.GameplayCue_FireBlast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.FireBlast"),
		FString("GameplayCue Tag for FireBlast"));

	/* Champion Tags */
	GameplayTags.Champion_Regenerator_RegenDuration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Champion.Regenerator.RegenDuration"),
		FString("Regeneration Duration"));
	GameplayTags.Champion_Regenerator_RegenAmount = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Champion.Regenerator.RegenAmount"),
		FString("Regen Amount each period"));
}
