// There is no fucking Copyright


#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::Singleton;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/** Attribute Tags */
	Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Strength"),
		FString("Increases physical damage")
	);
	Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Intelligence"),
		FString("Increases magical damage")
	);
	Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Resilience"),
		FString("Increases Armor and Armor Penetration")
	);
	Attribute_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Vigor"),
		FString("Increases Health")
	);
	Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.Armor"),
		FString("Reduces damage taken, improves Block Chance")
	);
	Attribute_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.ArmorPenetration"),
		FString("Ignored percentage of enemy Armor, increases Critical Hit Chance")
	);
	Attribute_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.BlockChance"),
		FString("Chance to cut incoming damage in half")
	);
	Attribute_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.CriticalHitChance"),
		FString("Chance to double damage plus critical hit bonus")
	);
	Attribute_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.CriticalHitDamage"),
		FString("Bonus damage added when a critical hit scored")
	);
	Attribute_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.CriticalHitResistance"),
		FString("Reduces Critical Hit Chance of attacking enemies")
	);
	Attribute_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.HealthRegeneration"),
		FString("Amount of Health regenerated every 1 second")
	);
	Attribute_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.ManaRegeneration"),
		FString("Amount of Mana regenerated every 1 second")
	);
	Attribute_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.MaxHealth"),
		FString("Maximum amount of Health obtainable")
	);
	Attribute_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.MaxMana"),
		FString("Maximum amount if Mana obtainable")
	);

	Attribute_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Resistance.Fire"),
		FString("Resistance to Fire damage")
	);
	Attribute_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Resistance.Lightning"),
		FString("Resistance to Lightning damage")
	);
	Attribute_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Resistance.Arcane"),
		FString("Resistance to Arcane damage")
	);
	Attribute_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Resistance.Physical"),
		FString("Resistance to Physical damage")
	);

	Attribute_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Meta.IncomingXP"),
		FString("Incoming XP Meta Attribute")
	);


	/** Input Tags */
	InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("Input Tag for Left Mouse Button")
	);
	InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("Input Tag for Right Mouse Button")
	);
	InputTag_Key1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Key1"),
		FString("Input Tag for Key 1")
	);
	InputTag_Key2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Key2"),
		FString("Input Tag for Key 2")
	);
	InputTag_Key3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Key3"),
		FString("Input Tag for Key 3")
	);
	InputTag_Key4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Key4"),
		FString("Input Tag for Key 4")
	);
	InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.1"),
		FString("Input Tag for Passive 1")
	);
	InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.2"),
		FString("Input Tag for Passive 2")
	);


	/** Damage Tags */
	Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"),
		FString("Damage")
	);

	Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Fire"),
		FString("Fire Damage Type")
	);
	Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Lightning"),
		FString("Lightning Damage Type")
	);
	Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Arcane"),
		FString("Arcane Damage Type")
	);
	Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("Physical Damage Type")
	);

	Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Burn"),
		FString("Debuff for Fire Damage")
	);
	Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Stun"),
		FString("Debuff for Lightning Damage")
	);
	Debuff_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Arcane"),
		FString("Debuff for Arcane Damage")
	);
	Debuff_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Physical"),
		FString("Debuff for Physical Damage")
	);

	Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Chance"),
		FString("Debuff Chance")
	);
	Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Damage"),
		FString("Debuff Damage")
	);
	Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Frequency"),
		FString("Debuff Frequency")
	);
	Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Duration"),
		FString("Debuff Duration")
	);

	DamageTypes2Resistances.Add(Damage_Fire, Attribute_Resistance_Fire);
	DamageTypes2Resistances.Add(Damage_Lightning, Attribute_Resistance_Lightning);
	DamageTypes2Resistances.Add(Damage_Arcane, Attribute_Resistance_Arcane);
	DamageTypes2Resistances.Add(Damage_Physical, Attribute_Resistance_Physical);

	DamageTypes2Debuffs.Add(Damage_Fire, Debuff_Burn);
	DamageTypes2Debuffs.Add(Damage_Lightning, Debuff_Stun);
	DamageTypes2Debuffs.Add(Damage_Arcane, Debuff_Arcane);
	DamageTypes2Debuffs.Add(Damage_Physical, Debuff_Physical);


	/** Effect Tags */
	Effect_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effect.HitReact"),
		FString("Tag granted when Hit Reacting")
	);


	/** Ability Tags */
	Ability_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Attack"),
		FString("Attack Ability Tag")
	);
	Ability_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Summon"),
		FString("Summon Ability Tag")
	);
	Ability_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Fire.FireBolt"),
		FString("Fire Bolt Ability Tag")
	);
	Ability_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Lightning.Electrocute"),
		FString("Electrocute Ability Tag")
	);

	Ability_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.HitReact"),
		FString("Hit React Ability Tag")
	);

	Ability_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Status.Locked"),
		FString("Ability Locked Status")
	);
	Ability_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Status.Eligible"),
		FString("Ability Eligible Status")
	);
	Ability_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Status.Unlocked"),
		FString("Ability Unlocked Status")
	);
	Ability_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Status.Equipped"),
		FString("Ability Equipped Status")
	);

	Ability_Type_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Type.Offensive"),
		FString("Ability Offensive Type")
	);
	Ability_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Type.Passive"),
		FString("Ability Passive Type")
	);
	Ability_Type_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Type.None"),
		FString("Ability None Type")
	);


	/** Ability Tags */
	Cooldown_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Fire.FireBolt"),
		FString("Fire Bolt Cooldown Tag")
	);
	Cooldown_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Lightning.Electrocute"),
		FString("Electrocute Cooldown Tag")
	);

	/** Combat Socket Tags */
	CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.Weapon"),
		FString("Weapon Socket")
	);
	CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.LeftHand"),
		FString("Left Hand Socket")
	);
	CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.RightHand"),
		FString("Right Hand Socket")
	);
	CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.Tail"),
		FString("Tail Socket")
	);


	/** Montage Tags */
	Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.1"),
		FString("Attack Montage Tag 1")
	);
	Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.2"),
		FString("Attack Montage Tag 2")
	);
	Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.3"),
		FString("Attack Montage Tag 3")
	);
	Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.4"),
		FString("Attack Montage Tag 4")
	);
}
