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

	DamageTypes2Resistances.Add(Damage_Fire, Attribute_Resistance_Fire);
	DamageTypes2Resistances.Add(Damage_Lightning, Attribute_Resistance_Lightning);
	DamageTypes2Resistances.Add(Damage_Arcane, Attribute_Resistance_Arcane);
	DamageTypes2Resistances.Add(Damage_Physical, Attribute_Resistance_Physical);


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
